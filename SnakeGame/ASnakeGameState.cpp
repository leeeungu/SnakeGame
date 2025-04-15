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

C_ASnakeGameState::C_ASnakeGameState()
{
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
	CreateEventActorArray(E_EventActor::E_EnumMax);
	SetRegisterRender(false);
}

C_ASnakeGameState::~C_ASnakeGameState()
{
	using namespace InputHandle::Mouse;
	InputHandleManager::UnRegisterMouse(InputType::E_TypeID::E_MOUSEBUTTONUP, ValueType::E_TypeID::E_LEFT, this);
}

void C_ASnakeGameState::SetScore(int nValue)
{ 
	m_nScore = nValue; 
	C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UIScore));
	if (pText)
	{
		std::string Temp = "Score : " + std::to_string(m_nScore);
		pText->SetText(Temp.c_str());
	}
}

void C_ASnakeGameState::SendState()
{
	TCP::Message::S_State sState{};
	sState.m_nSpeed = m_nSpeed;
	sState.m_nScore = m_nScore;
	sState.m_nLength = m_nLength;
	NetworkManager::SendMessage_2Server(Network::Protocol::E_TCP, &sState, sState.sData.nMessageSize);
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
		C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UILevel));
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
	C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UI));
	if (pText)
	{
		pText->SetText("GameOver");
		pText->SetRender(true);
	}
	FrameWork::SetUpdatetick(m_arTicks[0]);
	SoundManager::FadeOutMusic(0);
	SoundManager::FadeInMusic(Sound::Music::E_GameOver, 1);
}

void C_ASnakeGameState::SetUIBodyCount(int nCount)
{
	m_nLength = nCount;
	C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UIBodySize));
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
		m_bCanReset = false;
		FrameWork::SetRset(true);
	}
}

void C_ASnakeGameState::Reset()
{
	C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UI));
	if (pText)
	{
		pText->SetRender(false);
	}
	//SoundManager::FadeInMusic(Sound::Music::E_Background);
	SetLevelCount(10);
	SetScore(0);
	SetUIBodyCount(2);
	SendState();
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
		C_AText* pText = dynamic_cast<C_AText*>(GetEventActor(E_UIScore));
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
