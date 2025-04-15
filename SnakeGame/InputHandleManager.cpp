#include "InputHandleManager.h"
#include "SDL.h"
#include "Object.h"

InputHandleManager* InputHandleManager::m_pInstance = nullptr;

void InputHandleManager::CreateInstance()
{
	// 새로운 키 입력이 생기면  InputHandle::InputType 늘려주고 m_arSizeY, m_arSizeX 설정해주고 
	//                           custome static Register/UnRegister/GetTypeID, SetType 해주고 setEvent 수정만 해주면됨
	if (m_pInstance)
		return;
	m_pInstance = new InputHandleManager{};
	{
		m_pInstance->m_arSizeY[InputHandle::InputType::E_TypeID::E_None]	 = InputHandle::Dummy::InputType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeY[InputHandle::InputType::E_TypeID::E_KeyBoard] = InputHandle::KeyBoard::InputType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeY[InputHandle::InputType::E_TypeID::E_Mouse]	 = InputHandle::Mouse::InputType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeY[InputHandle::InputType::E_TypeID::E_System]	 = InputHandle::System::InputType::E_TypeID::E_EnumMax;
	}
	{
		m_pInstance->m_arSizeX[InputHandle::InputType::E_TypeID::E_None]	 = InputHandle::Dummy::ValueType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeX[InputHandle::InputType::E_TypeID::E_KeyBoard] = InputHandle::KeyBoard::ValueType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeX[InputHandle::InputType::E_TypeID::E_Mouse]    = InputHandle::Mouse::ValueType::E_TypeID::E_EnumMax;
		m_pInstance->m_arSizeX[InputHandle::InputType::E_TypeID::E_System]   = InputHandle::System::ValueType::E_TypeID::E_EnumMax;
	}
	for (int i = 0; i < InputHandle::InputType::E_TypeID::E_EnumMax; i++)
	{
		m_pInstance->CreateArray((InputHandle::InputType::E_TypeID)i);
	}
}

void InputHandleManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	for (int i = 0; i < InputHandle::InputType::E_TypeID::E_EnumMax; i++)
	{
		m_pInstance->DeleteArray((InputHandle::InputType::E_TypeID)i);
	}
	delete m_pInstance;
	m_pInstance = nullptr;
}


void InputHandleManager::CreateArray(InputHandle::InputType::E_TypeID eType)
{
	int nSizeY = m_arSizeY[eType];
	int nSizeX = m_arSizeX[eType];
	C_Object***& arObject = m_arObjects[eType];
	arObject = new C_Object**[nSizeY] {};
	for (int i = 0; i < nSizeY; i++)
	{
		arObject[i] = new C_Object * [nSizeX] {};
	}
	int a = 0;
}

void InputHandleManager::DeleteArray(InputHandle::InputType::E_TypeID eType)
{
	int nSizeY = m_arSizeY[eType];
	C_Object***& arObject = m_arObjects[eType];
	for (int i = 0; i < nSizeY; i++)
	{
		delete[] arObject[i];
	}
	delete[] arObject;
	arObject = nullptr;
}

C_Object** InputHandleManager::GetRegisterObjectRef(InputHandle::InputType::E_TypeID eType, int nIndexY, int nIndexX)
{
	InputHandle::InputType::E_TypeID eDummy = InputHandle::InputType::E_TypeID::E_None;
	C_Object** pResult = &m_arObjects[eDummy][m_arSizeY[eDummy]][m_arSizeX[eDummy]];
	if (nIndexY >= 0 && nIndexY < m_arSizeY[eType] && nIndexX >= 0 && nIndexX < m_arSizeX[eType])
		pResult = &m_arObjects[eType][nIndexY][nIndexX];
	return pResult;
}

void InputHandleManager::RunObject(const SDL_Event& Event)
{
	using namespace InputHandle;
	int Y{};
	int X{};

	if (m_eEventType == InputType::E_KeyBoard)
	{
		Y = m_eKeyBoardType;
		X = m_eKeyBoardValue;

	}
	else if (m_eEventType == InputType::E_Mouse)
	{
		Y = m_eMouseType;
		X = m_eMouseValue;
	}
	else if (m_eEventType == InputType::E_System)
	{
		Y = m_eSystemType;
	}
	
	C_Object* pObject = *GetRegisterObjectRef(m_eEventType, Y, X);
	if (pObject)
		pObject->HandleEventObject();
}

void InputHandleManager::SetEvent(const SDL_Event& Event)
{
	SDL_EventType eEvent = (SDL_EventType)Event.type;
	SDL_KeyCode eKeyValue = (SDL_KeyCode)Event.key.keysym.sym;
	m_eKeyBoardType = GetKeyBoardType(eEvent);
	m_eMouseType = GetMouseType(eEvent);
	m_eSystemType = GetSystemType(eEvent);

	using namespace InputHandle;
	m_eEventType = InputType::E_None;
	if (m_eKeyBoardType != KeyBoard::InputType::E_None)
	{
		m_eKeyBoardValue = GetKeyBoardValue(eKeyValue);
		m_eEventType = InputType::E_KeyBoard;
	}
	if (m_eMouseType != Mouse::InputType::E_None)
	{
		m_eMouseValue = GetMouseValue(Event.button.button);
		SDL_GetMouseState(&m_sMousePosition.x, &m_sMousePosition.y);
		m_eEventType = InputType::E_Mouse;
	}
	if (m_eSystemType != System::InputType::E_None)
	{
		m_eEventType = InputType::E_System;
	}
}

bool InputHandleManager::RegisterObject(C_Object* pObject, InputHandle::InputType::E_TypeID eType, int nIndexY, int nIndexX)
{
	C_Object*& ppObject = *m_pInstance->GetRegisterObjectRef(eType, nIndexY, nIndexX);
	ppObject = pObject;
	return ppObject == pObject && pObject != nullptr;
}

bool InputHandleManager::UnRegisterObject(C_Object* pObject, InputHandle::InputType::E_TypeID eType, int nIndexY, int nIndexX)
{
	C_Object*& ppObject = *m_pInstance->GetRegisterObjectRef(eType, nIndexY, nIndexX);
	if (ppObject == pObject)
		ppObject = nullptr;
	return ppObject == nullptr;
}

void InputHandleManager::HandleEvent()
{
	using namespace InputHandle;
	SDL_Event event{};
	if (SDL_PollEvent(&event)) {

		m_pInstance->SetEvent(event);
		m_pInstance->RunObject(event);
	}
}

bool InputHandleManager::RegisterKeyBoard(InputHandle::KeyBoard::InputType::E_TypeID eInputID, InputHandle::KeyBoard::ValueType::E_TypeID eValueID, C_Object* pObject)
{
	return RegisterObject(pObject, InputHandle::InputType::E_TypeID::E_KeyBoard, eInputID, eValueID);
}

bool InputHandleManager::UnRegisterKeyBoard(InputHandle::KeyBoard::InputType::E_TypeID eInputID, InputHandle::KeyBoard::ValueType::E_TypeID eValueID, C_Object* pObject)
{
	return UnRegisterObject(pObject, InputHandle::InputType::E_TypeID::E_KeyBoard, eInputID, eValueID);
}

bool InputHandleManager::RegisterMouse(InputHandle::Mouse::InputType::E_TypeID eInputID, InputHandle::Mouse::ValueType::E_TypeID eValueID, C_Object* pObject)
{
	return RegisterObject(pObject, InputHandle::InputType::E_TypeID::E_Mouse, eInputID, eValueID);
}

bool InputHandleManager::UnRegisterMouse(InputHandle::Mouse::InputType::E_TypeID eInputID, InputHandle::Mouse::ValueType::E_TypeID eValueID, C_Object* pObject)
{
	return UnRegisterObject(pObject, InputHandle::InputType::E_TypeID::E_Mouse, eInputID, eValueID);
}

bool InputHandleManager::RegisterSystem(InputHandle::System::InputType::E_TypeID eInputID, C_Object* pObject)
{
	return RegisterObject(pObject, InputHandle::InputType::E_TypeID::E_System, eInputID, InputHandle::System::ValueType::E_TypeID::E_None);
}

bool InputHandleManager::UnRegisterSystem(InputHandle::System::InputType::E_TypeID eInputID, C_Object* pObject)
{
	return UnRegisterObject(pObject, InputHandle::InputType::E_TypeID::E_System, eInputID, InputHandle::System::ValueType::E_TypeID::E_None);
}

InputHandle::KeyBoard::InputType::E_TypeID InputHandleManager::GetKeyBoardType(SDL_EventType eEvent)
{
	using namespace InputHandle::KeyBoard::InputType;
	E_TypeID eResult = E_TypeID::E_None;
	if(eEvent == SDL_EventType::SDL_KEYDOWN)
		eResult = E_TypeID::E_KEYDOWN;
	if(eEvent == SDL_EventType::SDL_KEYUP)
		eResult = E_TypeID::E_KEYUP;
	return eResult;
}

InputHandle::KeyBoard::ValueType::E_TypeID InputHandleManager::GetKeyBoardValue(SDL_KeyCode eEvent)
{
	using namespace InputHandle::KeyBoard::ValueType;
	E_TypeID eResult = E_TypeID::E_None;
	if (eEvent == SDL_KeyCode::SDLK_UP)
		eResult = E_TypeID::E_UP;
	if (eEvent == SDL_KeyCode::SDLK_DOWN)
		eResult = E_TypeID::E_DOWN;
	if (eEvent == SDL_KeyCode::SDLK_LEFT)
		eResult = E_TypeID::E_LEFT;
	if (eEvent == SDL_KeyCode::SDLK_RIGHT)
		eResult = E_TypeID::E_RIGHT;
	if (eEvent == SDL_KeyCode::SDLK_RETURN)
		eResult = E_TypeID::E_Enter;
	return eResult;
}

InputHandle::Mouse::InputType::E_TypeID InputHandleManager::GetMouseType(SDL_EventType eEvent)
{
	using namespace InputHandle::Mouse::InputType;
		
	E_TypeID eResult = E_TypeID::E_None;
	if (eEvent == SDL_EventType::SDL_MOUSEMOTION)
		eResult = E_TypeID::E_MOUSEMOTION;
	if (eEvent == SDL_EventType::SDL_MOUSEBUTTONUP)
		eResult = E_TypeID::E_MOUSEBUTTONUP;
	if (eEvent == SDL_EventType::SDL_MOUSEBUTTONDOWN)
		eResult = E_TypeID::E_MOUSEBUTTONDOWN;
	if (eEvent == SDL_EventType::SDL_MOUSEWHEEL)
		eResult = E_TypeID::E_MOUSEWHEEL;
	return eResult;
}

InputHandle::Mouse::ValueType::E_TypeID InputHandleManager::GetMouseValue(Uint8 eButton)
{
	using namespace InputHandle::Mouse::ValueType;
	E_TypeID eResult = E_TypeID::E_None;
	if (eButton == SDL_BUTTON_LEFT)
		eResult = E_TypeID::E_LEFT;
	if (eButton == SDL_BUTTON_MIDDLE)
		eResult = E_TypeID::E_MIDDLE;
	if (eButton == SDL_BUTTON_RIGHT)
		eResult = E_TypeID::E_RIGHT;
	if (eButton == SDL_BUTTON_X1)
		eResult = E_TypeID::E_BUTTON_X1;
	if (eButton == SDL_BUTTON_X2)
		eResult = E_TypeID::E_BUTTON_X2;
	return eResult;
}

InputHandle::System::InputType::E_TypeID InputHandleManager::GetSystemType(SDL_EventType eEvent)
{
	using namespace InputHandle::System::InputType;
	E_TypeID eResult = E_TypeID::E_None;
	if (eEvent == SDL_EventType::SDL_QUIT)
		eResult = E_TypeID::E_QUIT;
	return eResult;
}


//SDL_Event event;
//if (SDL_PollEvent(&event)) {
//	if (event.type == SDL_QUIT) {
//		DestroyScene();
//		g_flag_running = false;
//		return;
//	}
//	else if (event.type == SDL_KEYDOWN) { 
//		if (g_input != 0 && bReady == true) {
//			gameover = false;
//			bReady = false;
//			pUIActor->SetRender(false);
//		}
//		return;
//	}
//	else if (event.type == SDL_MOUSEBUTTONDOWN) {
//		if (event.button.button == SDL_BUTTON_LEFT && gameover == true && bReady == false)
//			InitGame();
//		return;
//	}
//}