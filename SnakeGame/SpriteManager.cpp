#include "SDL_image.h"
#include "SpriteManager.h"
#include "DebugMessageManager.h"

SpriteManager* SpriteManager::m_pInstance = 0;

void SpriteManager::CreateInstance() {
	if (m_pInstance != 0) return;
	m_pInstance = new SpriteManager();
	//instance_->bDebug = false;
	using namespace Sprite;
	m_pInstance->m_arSpriteIndex[E_SpriteType::E_Block] = Block::E_EnumMax;
	m_pInstance->m_arSpriteIndex[E_SpriteType::E_Background] = Background::E_EnumMax;
	m_pInstance->m_arSprite[E_SpriteType::E_Block] = m_pInstance->m_arSnake;
	m_pInstance->m_arSprite[E_SpriteType::E_Background] = m_pInstance->m_arTexture;
}

void SpriteManager::DestroyInstance() {
	if (m_pInstance == 0) return;
	{
		for (int i = 0; i < Sprite::E_SpriteType::E_EnumMax; i++)
		{
			SDL_Surface*& pSurface = m_pInstance->m_arSpriteSurface[i];
			if (pSurface)
			{
				SDL_FreeSurface(pSurface);
			}
			pSurface = nullptr;
		}
	}
	delete(m_pInstance);
	m_pInstance = nullptr;
}

SDL_Surface* SpriteManager::LoadImageSurface(Sprite::E_SpriteType eSpriteType)
{
	SDL_Surface* pResult = GetImageSurface(eSpriteType);
	if (pResult)
		return pResult;
	std::string File = GetFilePath() + GetImageFile(eSpriteType);
	SDL_Surface*& pRef = m_pInstance->m_arSpriteSurface[eSpriteType];
	pRef = IMG_Load(File.c_str());
	if (pRef)
	{
		std::vector<SDL_Rect> arResult{};
		m_pInstance->GetSourceRect(eSpriteType, arResult);
		int nIndex{};
		for (SDL_Rect& Rect : arResult)
		{
			SpriteManager::RegisterSpriteRect(eSpriteType, nIndex, Rect);
			nIndex++;
		}
	}
	else
	{
		DebugMessageManager::PrintDebugMesasge_Sprite(File.c_str(), "LoadFail");
	}
	return pRef;
}

bool SpriteManager::RegisterSpriteRect(Sprite::E_SpriteType eSpriteType, unsigned int nIndex,const SDL_Rect& sRect)
{
	using namespace Sprite;
	if (m_pInstance->m_arSpriteIndex[eSpriteType] <= nIndex || nIndex < 0)
		return false;
	m_pInstance->m_arSprite[eSpriteType][nIndex] = sRect;
	return true;
}

bool SpriteManager::GetSpriteSourceRect(Sprite::E_SpriteType eSpriteType, unsigned int nIndex, SDL_Rect& sRect)
{
	if (m_pInstance->m_arSpriteIndex[eSpriteType] <= nIndex || nIndex < 0)
		return false;
	sRect = m_pInstance->m_arSprite[eSpriteType][nIndex];
	return true;
}

std::string SpriteManager::GetFilePath()
{
	return "../Resources/Texture/";
}

std::string SpriteManager::GetImageFile(Sprite::E_SpriteType eSpriteType)
{
	using namespace Sprite;
	using namespace std;
	string arData[E_SpriteType::E_EnumMax]{};
	arData[E_SpriteType::E_Block] = "Snake.png";
	arData[E_SpriteType::E_Background] = "SnakeGameTitle.png";
	return arData[eSpriteType];
}

SDL_Surface* SpriteManager::GetImageSurface(Sprite::E_SpriteType eSpriteType)
{
	return m_pInstance->m_arSpriteSurface[eSpriteType];
}

void SpriteManager::GetSourceRect(Sprite::E_SpriteType eSpriteType, std::vector<SDL_Rect>& arResult)
{
	arResult.clear();
	arResult.resize(m_pInstance->m_arSpriteIndex[eSpriteType]);
	switch (eSpriteType)
	{
	case Sprite::E_Block:
		arResult = { SDL_Rect{ 0, 0, 100, 1000 },
			SDL_Rect{ 0, 0, 64, 64 }, SDL_Rect{ 64, 0, 64, 64 }, SDL_Rect{ 128, 0, 64, 64 }, SDL_Rect{ 192 , 0, 64, 64 },
			SDL_Rect{ 256, 0, 64, 64 }, SDL_Rect{ 0, 64, 64, 64 }, SDL_Rect{ 128, 64, 64, 64 }, SDL_Rect{ 192, 64, 64, 64 },
			SDL_Rect{ 256, 64, 64, 64 }, SDL_Rect{ 128, 128, 64, 64 }, SDL_Rect{ 192, 128, 64, 64 }, SDL_Rect{ 256, 128, 64, 64 },
			SDL_Rect{ 192, 192, 64, 64 }, SDL_Rect{ 256, 192, 64, 64 }, SDL_Rect{ 0, 192, 64, 64 }, SDL_Rect{ 64, 192, 64, 64 }, SDL_Rect{ 0, 192, 64, 64 }
		};
		break;
	case Sprite::E_Background:
		arResult = { SDL_Rect{ 0, 0, 0, 0 }, SDL_Rect{ 0, 0, 1700, 830} };
		break;
	}
}
