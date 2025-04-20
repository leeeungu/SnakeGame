#include "ABlock.h"
#include "SpriteManager.h"
#include "RenderManager.h"
#include "SoundManager.h"
#include "UDPManager.h"
#include "AMap.h"

C_ABlock::C_ABlock()
{
	using namespace Sprite::Block;
	Sprite::E_SpriteType eSpriteType = Sprite::E_SpriteType::E_Block;
	m_pSDLSurface = SpriteManager::LoadImageSurface(eSpriteType);
	{
		using namespace Block::Apple;
		m_arColorMode[Block::Apple::E_TypeID::E_None] = {255,255,255};
		m_arColorMode[Block::Apple::E_TypeID::E_Red] = {255,255,255};
		m_arColorMode[Block::Apple::E_TypeID::E_Gold] = {255,255,255};
		m_arColorMode[Block::Apple::E_TypeID::E_Blue] = {0,0,255};
	}
	SetRenderLayer(Actor::E_RenderLayer::E_Map);
	SpriteManager::GetSpriteSourceRect(eSpriteType, m_nSpriteIndex, m_sSourceRect);
	m_bCanReset = false;
}

void C_ABlock::SetSpriteIndex(unsigned int nIndex)
{ 
	using namespace Sprite::Block;
	using namespace Block::Apple;

	if (m_nSpriteIndex != nIndex)
	{
		Actor::E_RenderLayer eLayer = Actor::E_RenderLayer::E_Map;
		if (nIndex != E_SpriteID::E_None)
			eLayer = Actor::E_RenderLayer::E_Actor;
		SetRenderLayer(eLayer);
	}
	m_nSpriteIndex = nIndex; 
	using namespace Sprite;
	E_SpriteType eSpriteType = E_SpriteType::E_Block;
	m_eApple = (E_TypeID)(m_nSpriteIndex - E_RedApple + 1);
	if (m_eApple < 0 || m_eApple >= E_TypeID::E_EnumMax)
		m_eApple = E_TypeID::E_None;
	SendMessage();
	SpriteManager::GetSpriteSourceRect(eSpriteType, m_nSpriteIndex, m_sSourceRect);
}

void C_ABlock::SetSpriteIndex_Map(unsigned int nIndex)
{
	using namespace Sprite::Block;
	using namespace Block::Apple;

	if (m_nSpriteIndex != nIndex)
	{
		Actor::E_RenderLayer eLayer = Actor::E_RenderLayer::E_Map;
		if (nIndex != E_SpriteID::E_None)
			eLayer = Actor::E_RenderLayer::E_Actor;
		SetRenderLayer(eLayer);
	}
	m_nSpriteIndex = nIndex;
	using namespace Sprite;
	E_SpriteType eSpriteType = E_SpriteType::E_Block;
	m_eApple = (E_TypeID)(m_nSpriteIndex - E_RedApple + 1);
	if (m_eApple < 0 || m_eApple >= E_TypeID::E_EnumMax)
		m_eApple = E_TypeID::E_None;
	SpriteManager::GetSpriteSourceRect(eSpriteType, m_nSpriteIndex, m_sSourceRect);
}

void C_ABlock::Render()
{
	SDL_Texture* pTexture = RenderManager::GetTexture(m_pSDLSurface);
	SDL_Color& sColor = m_arColorMode[m_eApple];
	if (pTexture && m_nSpriteIndex != 0)
	{
		SDL_SetTextureColorMod(pTexture, sColor.r, sColor.g, sColor.b); 
		RenderManager::RenderTexture(pTexture, &m_sSourceRect, &m_sRenderRect);
	}
	else
	{
		RenderManager::DrawBorder(m_sRenderRect, { 1 ,1 }, { 0,0,0 });
	}
}

bool C_ABlock::OverlapEvent(C_Actor* pActor)
{
	using namespace Sound::Chunk::Apple;
	using namespace Block::Apple;
	E_DataID arChunk[E_TypeID::E_EnumMax]{};
	arChunk[E_TypeID::E_Red] = E_DataID::E_RedApple;
	arChunk[E_TypeID::E_Gold] = E_DataID::E_GoldApple;
	arChunk[E_TypeID::E_Blue] = E_DataID::E_BlueApple;
	int nIndex = m_eApple;
	if (nIndex < 0 || nIndex >= E_TypeID::E_EnumMax)
		nIndex = 0;
	SoundManager::PlayChannel(arChunk[nIndex]);
	return true;
}

void C_ABlock::Reset()
{
	using namespace Sprite::Block;
	SetSpriteIndex_Map(Sprite::Block::E_None);
	Actor::E_RenderLayer eLayer = Actor::E_RenderLayer::E_Map;
	if (m_nSpriteIndex != E_SpriteID::E_None)
		eLayer = Actor::E_RenderLayer::E_Actor;
	SetRenderLayer(eLayer);
}

bool C_ABlock::RecvUDPMessage(void* pMessage, int nMessageLength)
{
	using namespace UDP::Message;
	Snake::S_Map* sMessage = (Snake::S_Map*)pMessage;
	if (!sMessage)
		return false;
	SetSpriteIndex_Map(sMessage->nSpriteIndex);
	return true;
}

void C_ABlock::SendMessage()
{
	using namespace UDP::Message;
	Snake::S_Map sMessage{};
	sMessage.nX = m_nCoord.x;
	sMessage.nY = m_nCoord.y;
	sMessage.nSpriteIndex = m_nSpriteIndex;
	NetworkManager::SendMessage_2Server(Network::Protocol::E_UDP, &sMessage, sMessage.sData.nMessageSize);
}

