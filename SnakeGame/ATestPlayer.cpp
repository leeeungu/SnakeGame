#include "ATestPlayer.h"
#include "FrameWork.h"
#include "InputHandleManager.h"

C_ATestPlayer::C_ATestPlayer()
{
	pFunc[E_Title] = &C_ATestPlayer::Update_Title;
	pFunc[E_Game] = &C_ATestPlayer::Update_Game;

	m_arDir[E_Right] = SDL_KeyCode::SDLK_RIGHT;
	m_arDir[E_Down] = SDL_KeyCode::SDLK_DOWN;
	m_arDir[E_Left] = SDL_KeyCode::SDLK_LEFT;
	m_arDir[E_Up] = SDL_KeyCode::SDLK_UP;
	SetRegisterUpdate(true);
	SetUpdate(true);
}

void C_ATestPlayer::ReturnTitle()
{
	InputHandleManager::PushInputMouse(SDL_EventType::SDL_MOUSEBUTTONUP, SDL_BUTTON_LEFT);
	m_fTime = 0;
	m_bMatching = false;
}

void C_ATestPlayer::Update(Uint32 fDeltaTick)
{
	m_eState = E_Title;
	if (FrameWork::GetSceneType() == Framework::Scene::E_SnakeGame)
		m_eState = E_Game;
	(this->*pFunc[m_eState])(fDeltaTick);
}

void C_ATestPlayer::Update_Title(Uint32 fDeltaTick)
{
	if (m_bMatching)
		return;

	m_fTime += fDeltaTick;
	if (m_fTime > 1000)
	{
		m_fTime = 0;
		InputHandleManager::PushInputKeyBoard(SDL_EventType::SDL_MOUSEBUTTONUP, SDL_KeyCode::SDLK_RETURN);
		m_bMatching = true;
	}
}

void C_ATestPlayer::Update_Game(Uint32 fDeltaTick)
{
	m_fTime += fDeltaTick;
	if (m_fTime > 300)
	{
		m_eDirection = (E_PlayerDirection)(m_eDirection + 1);
		if (m_eDirection >= E_Max)
			m_eDirection = E_Right;
		InputHandleManager::PushInputKeyBoard(SDL_EventType::SDL_KEYDOWN, m_arDir[m_eDirection]);
	}
}
