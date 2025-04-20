#include "ASnakeGameState.h"
#include "SpriteManager.h"
#include "FrameWork.h"
#include  "AText.h"
#include "ASnake.h"
#include "ABlock.h"
#include "SoundManager.h"
#include "InputHandleManager.h"
#include "NetworkManager.h"
#include "AAppleSpawner.h"
#include "TCPManager.h"
#include "DebugMessageManager.h"
#include "ATexture.h"
#include "AMap.h"

C_ASnakeGameState::C_ASnakeGameState()
{
	CreateEventActorArray(E_EventActor::E_EnumMax);
	{
		using namespace InputHandle::Mouse;
		InputHandleManager::RegisterMouse(InputType::E_TypeID::E_MOUSEBUTTONUP, ValueType::E_TypeID::E_LEFT, this);
	}
	
	using namespace GameState;
	m_arTicks[E_Level::E_None]	 = 33;
	m_arTicks[E_Level::E_Level1] = 33 *4;
	m_arTicks[E_Level::E_Level2] = 33 * 4 - 20;
	m_arTicks[E_Level::E_Level3] = 33 * 3 -10;
	m_arTicks[E_Level::E_Level4] = 33 * 3;
	m_arTicks[E_Level::E_Level5] = 33 *2;
	m_arTicks[E_Level::E_Level6] = 33 * 2 -10;
	m_arTicks[E_Level::E_Level7] = 33;
	SetRegisterRender(false);
	SetRegisterUpdate(true);
	SetUpdate(true);

	m_eTexture[0] = Sprite::E_SpriteType::E_CountDown3;
	m_eTexture[1] = Sprite::E_SpriteType::E_CountDown2;
	m_eTexture[2] = Sprite::E_SpriteType::E_CountDown1;
	m_eTexture[3] = Sprite::E_SpriteType::E_GaemStart;
	
	{
		using namespace FontData;
		m_pText[GameState::SnakeUI::E_Score] = new C_AText{ E_FontID::E_cutetat , 30, {100,0} };
		m_pText[GameState::SnakeUI::E_Length] = new C_AText{ E_FontID::E_cutetat , 30, {300,0} };
		m_pText[GameState::SnakeUI::E_Speed] = new C_AText{ E_FontID::E_cutetat , 30, {500,0} };
	}
	m_pGameUI = new C_ATexture{}; 
	m_pGameUI->SetRenderSize({ 75,110 });
	m_pGameUI->SetRenderCenter(true);
	m_pGameUI->SetRenderLayer(Actor::E_UI);
}

C_ASnakeGameState::~C_ASnakeGameState()
{
	using namespace InputHandle::Mouse;
	InputHandleManager::UnRegisterMouse(InputType::E_TypeID::E_MOUSEBUTTONUP, ValueType::E_TypeID::E_LEFT, this);

	for (int i = 0; i < GameState::SnakeUI::E_EnumMax; i++)
	{
		if (m_pText[i])
			delete m_pText[i];
		m_pText[i] = nullptr;
	}
	if (m_pGameUI)
		delete m_pGameUI;
	m_pGameUI = nullptr;
}

void C_ASnakeGameState::SetScore(int nValue)
{ 
	m_nScore = nValue; 
	C_AText* pText = m_pText[GameState::SnakeUI::E_Score];
	if (pText)
	{
		std::string Temp = "Score : " + std::to_string(m_nScore);
		pText->SetText(Temp.c_str());
	}
}

void C_ASnakeGameState::GameEnd()
{
	m_pGameUI->SetRender(true);
	m_pSnake->SetUpdate(false);
	m_pSpawner->SetUpdate(false);


}

void C_ASnakeGameState::DelegateEventActor(int nIndex)
{
	m_pMap = (C_AMap*)GetEventActor(E_Map);
	m_pSnake = (C_ASnake*)GetEventActor(E_Snake);
	if (m_pGameUI)
		m_pGameUI->SetTexture(m_eTexture[m_nIndex], 1);
	m_pSpawner = (C_AAppleSpawner*)GetEventActor(E_Spawner);
}

void C_ASnakeGameState::Update(Uint32 fDeltaTick)
{
	m_nCount += fDeltaTick;
	if (m_nCount >= 800)
	{
		m_nIndex++;
		m_nCount = 0;
		if (m_nIndex < 4)
		{
			if (m_pGameUI)
				m_pGameUI->SetTexture(m_eTexture[m_nIndex], 1);
			if (m_nIndex == 3)
			{
				m_pGameUI->SetRenderSize({ 800,300 });
			}
		}
		else if (m_nIndex == 5)
		{
			m_pGameUI->SetRender(false);
			m_pSnake->SetUpdate(true);
			m_pSpawner->SetUpdate(true);
		}
	}
}

void C_ASnakeGameState::SendState()
{
	TCP::Message::S_State sState{};
	sState.m_nSpeed = m_nSpeed;
	sState.m_nScore = m_nScore;
	sState.m_nLength = m_nLength;
	NetworkManager::SendMessage_2Server(Network::Protocol::E_TCP, &sState, sState.sData.nMessageSize);

	if (m_bDead)
	{
		TCP::Message::S_GameEnd sGame{};
		sGame.m_bGaemResult = false;
		NetworkManager::SendMessage_2Server(Network::Protocol::E_TCP, &sGame, sGame.sData.nMessageSize);
	}
}

void C_ASnakeGameState::SetLevelCount(int nCount)
{
	m_nLevelCount = nCount;
	if (m_nLevelCount < 10)
		m_nLevelCount = 10;
	if (m_nLevelCount % 10 == 0)
	{
		GameState::E_Level eLevel = (GameState::E_Level)( m_nLevelCount / 10);
		if (GameState::E_Level::E_EnumMax <= eLevel || eLevel <= GameState::E_Level::E_None)
			return;
		m_nSpeed = eLevel;
		FrameWork::SetUpdatetick(m_arTicks[m_nSpeed]);
		C_AText* pText = m_pText[GameState::SnakeUI::E_Speed];
		if (pText)
		{
			std::string Temp = "Speed : " + std::to_string(m_nSpeed);
			pText->SetText(Temp.c_str());
		}
	}
}

void C_ASnakeGameState::SetDead()
{
	m_bCanReset = true;
	m_bDead = true;
	FrameWork::SetUpdatetick(m_arTicks[0]);
	FrameWork::SetUpdate(false);
	SoundManager::FadeOutMusic(0);
	SoundManager::FadeInMusic(Sound::Music::E_Lose, 1);
}

void C_ASnakeGameState::SetUIBodyCount(int nCount)
{
	m_nLength = nCount;
	C_AText* pText = m_pText[GameState::SnakeUI::E_Length];
	if (pText)
	{
		std::string Temp = "Length : " + std::to_string(nCount);
		pText->SetText(Temp.c_str());
	}
}

void C_ASnakeGameState::HandleEvent()
{
	using namespace InputHandle::Mouse;
	if (m_bCanReset && InputHandleManager::GetCurrentMouseValue() == ValueType::E_TypeID::E_LEFT)
	{
		FrameWork::ChangeScene(Framework::Scene::E_Title);
		FrameWork::SetUpdate(true);
	}
}

void C_ASnakeGameState::Reset()
{
	SoundManager::FadeInMusic(Sound::Music::E_Background);
	SetLevelCount(10);
	SetScore(0);
	SetUIBodyCount(2);
	SendState();
	FrameWork::SetUpdate(true);
	m_bDead = false;
	m_bCanReset = false;
}

bool C_ASnakeGameState::OverlapEvent(C_Actor* pActor)
{	
	C_ASnake* pSnake = dynamic_cast<C_ASnake*>(GetEventActor(E_Snake));
	if (!pSnake)
		return false;
	if (pSnake->GetAppleType() == Block::Apple::E_Blue)
	{
		SetLevelCount((m_nLevelCount / 10 - 1) * 10);
	}
	else
	{
		C_AText* pText = m_pText[GameState::SnakeUI::E_Score];
		if (pSnake->GetAppleType() == Block::Apple::E_Red)
		{
			SetScore(m_nScore + 10);
			SetLevelCount(m_nLevelCount + 1);
		}
		SetUIBodyCount(pSnake->GetBodyCount());
	}
	if (pSnake->GetDead())
	{
		SetDead();
	}

	SendState();
	return true;
}
