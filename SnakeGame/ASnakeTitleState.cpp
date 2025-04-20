#include "ASnakeTitleState.h"
#include "InputHandleManager.h"
#include "FrameWork.h" 
#include "TCPManager.h"
#include "ATexture.h"

C_ASnakeTitleState::C_ASnakeTitleState()
{
	CreateEventActorArray(E_EventActor::E_EnumMax);
	{
		using namespace InputHandle::KeyBoard;
		InputHandleManager::RegisterKeyBoard(InputType::E_TypeID::E_KEYUP, ValueType::E_TypeID::E_Enter, this);
	}
}

C_ASnakeTitleState::~C_ASnakeTitleState()
{
	{
		using namespace InputHandle::KeyBoard;
		InputHandleManager::UnRegisterKeyBoard(InputType::E_TypeID::E_KEYUP, ValueType::E_TypeID::E_Enter, this);
	}
}

void C_ASnakeTitleState::HandleEvent()
{
	using namespace InputHandle::Mouse;
	if (!m_bMathchingStart)
	{
		using namespace TCP::Message;
		m_bMathchingStart = true;
		S_Matching sState{};
		NetworkManager::SendMessage_2Server(Network::Protocol::E_TCP, &sState, sState.sData.nMessageSize);
		//FrameWork::ChangeScene(Framework::Scene::E_SnakeGame);
		C_ATexture* pText =  (C_ATexture*)(GetEventActor(E_EventActor::E_Text));
		if (pText)
		{
			pText->SetTexture(Sprite::E_Matching);
			SDL_FPoint sPoint{};
			pText->GetRenderPosition(sPoint);
			sPoint.y += 100;
			pText->SetRenderPosition(sPoint);
		}
		pText = (C_ATexture*)(GetEventActor(E_EventActor::E_Spinner));
		if (pText)
		{
			pText->SetRender(true);
			pText->SetUpdate(true);
			pText->SetRegisterUpdate(true);
		}
	}
}