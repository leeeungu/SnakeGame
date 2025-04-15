#pragma once

#include <map>
#include "SDL.h"

class C_Object;

namespace InputHandle
{
    namespace InputType
    {
        enum E_TypeID
        {
            E_None,
            E_KeyBoard,
            E_Mouse,
            E_System,
            E_EnumMax
        };
    }
    namespace Dummy
    {
        namespace InputType
        {
            enum E_TypeID
            {
                E_None,
                E_EnumMax
            };
        }

        namespace ValueType
        {
            enum E_TypeID
            {
                E_None,
                E_EnumMax
            };
        }
    }
    namespace KeyBoard
    {
        namespace InputType
        {
            enum E_TypeID
            {
                E_None,
                E_KEYDOWN,
                E_KEYUP,
                E_EnumMax
            };
        }

        namespace ValueType
        {
            enum E_TypeID
            {
                E_None,
                E_RIGHT,
                E_LEFT,
                E_DOWN,
                E_UP,
                E_Enter,
                E_EnumMax
            };
        }
    }
    namespace Mouse
    {
        namespace InputType
        {
            enum E_TypeID
            {
                E_None,
                E_MOUSEMOTION,
                E_MOUSEBUTTONDOWN,
                E_MOUSEBUTTONUP,
                E_MOUSEWHEEL,
                E_EnumMax
            };
        }

        namespace ValueType
        {
            enum E_TypeID
            {
                E_None,
                E_LEFT = SDL_BUTTON_LEFT,
                E_MIDDLE = SDL_BUTTON_MIDDLE,
                E_RIGHT = SDL_BUTTON_RIGHT,
                E_BUTTON_X1 = SDL_BUTTON_X1,
                E_BUTTON_X2 = SDL_BUTTON_X2,
                E_EnumMax
            };
        }
    }
    namespace System
    {
        namespace InputType
        {
            enum E_TypeID
            {
                E_None,
                E_QUIT,
                E_EnumMax
            };
        }
        namespace ValueType
        {
            enum E_TypeID
            {
                E_None,
                E_EnumMax
            };
        }
    }
}

class InputHandleManager
{
private:
	InputHandleManager() = default;
	static InputHandleManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();
private:
    void CreateArray(InputHandle::InputType::E_TypeID eType);
    void DeleteArray(InputHandle::InputType::E_TypeID eType);
    C_Object** GetRegisterObjectRef(InputHandle::InputType::E_TypeID eType = InputHandle::InputType::E_TypeID::E_None, int nIndexY = 0, int nIndexX = 0);
    void RunObject(const SDL_Event& Event);
    void SetEvent(const SDL_Event& Event);
    static bool RegisterObject(C_Object* pObject, InputHandle::InputType::E_TypeID eType = InputHandle::InputType::E_TypeID::E_None, int nIndexY = 0, int nIndexX = 0);
    static bool UnRegisterObject(C_Object* pObject, InputHandle::InputType::E_TypeID eType = InputHandle::InputType::E_TypeID::E_None, int nIndexY = 0, int nIndexX = 0);
public:
	static void HandleEvent();

    // KeyBoard
    static bool RegisterKeyBoard(InputHandle::KeyBoard::InputType::E_TypeID eInputID, InputHandle::KeyBoard::ValueType::E_TypeID eValueID, C_Object* pObject);
    static bool UnRegisterKeyBoard(InputHandle::KeyBoard::InputType::E_TypeID eInputID, InputHandle::KeyBoard::ValueType::E_TypeID eValueID, C_Object* pObject);
    // Mouse
    static bool RegisterMouse(InputHandle::Mouse::InputType::E_TypeID eInputID, InputHandle::Mouse::ValueType::E_TypeID eValueID, C_Object* pObject);
    static bool UnRegisterMouse(InputHandle::Mouse::InputType::E_TypeID eInputID, InputHandle::Mouse::ValueType::E_TypeID eValueID, C_Object* pObject);
    // System
    static bool RegisterSystem(InputHandle::System::InputType::E_TypeID eInputID, C_Object* pObject);
    static bool UnRegisterSystem(InputHandle::System::InputType::E_TypeID eInputID, C_Object* pObject);

    static InputHandle::InputType::E_TypeID GetCurrentEventType() { return m_pInstance->m_eEventType; }
    static InputHandle::KeyBoard::InputType::E_TypeID GetCurrentKeyBoardType() { return m_pInstance->m_eKeyBoardType; }
    static InputHandle::KeyBoard::ValueType::E_TypeID GetCurrentKeyBoardValue() { return m_pInstance->m_eKeyBoardValue; }
    static InputHandle::Mouse::ValueType::E_TypeID GetCurrentMouseValue() { return m_pInstance->m_eMouseValue; }
    static InputHandle::Mouse::InputType::E_TypeID GetCurrentMouseType() { return m_pInstance->m_eMouseType; }
    static void GetMousePosition(SDL_Point& sPoint) { sPoint = m_pInstance-> m_sMousePosition;  }
    static InputHandle::System::InputType::E_TypeID GetCurrentSystemType() { return m_pInstance->m_eSystemType; }
private:
    // 전체 SDL 키를 사용하는 게 아니라서 따로 걸러줌 (SDL이 배열로 하기에는 너무 커 if로 함)
    InputHandle::KeyBoard::InputType::E_TypeID GetKeyBoardType(SDL_EventType eEvent) ;
    InputHandle::KeyBoard::ValueType::E_TypeID GetKeyBoardValue(SDL_KeyCode eEvent) ;
    InputHandle::Mouse::InputType::E_TypeID GetMouseType(SDL_EventType eEvent) ;
    InputHandle::Mouse::ValueType::E_TypeID GetMouseValue(Uint8 eButton);
    InputHandle::System::InputType::E_TypeID GetSystemType(SDL_EventType eEvent);
private:
    C_Object*** m_arObjects[InputHandle::InputType::E_TypeID::E_EnumMax];
    int m_arSizeY[InputHandle::InputType::E_TypeID::E_EnumMax];
    int m_arSizeX[InputHandle::InputType::E_TypeID::E_EnumMax];

    InputHandle::InputType::E_TypeID m_eEventType;
    InputHandle::KeyBoard::InputType::E_TypeID m_eKeyBoardType;
    InputHandle::KeyBoard::ValueType::E_TypeID m_eKeyBoardValue;
    InputHandle::Mouse::InputType::E_TypeID m_eMouseType;
    InputHandle::Mouse::ValueType::E_TypeID m_eMouseValue;
    InputHandle::System::InputType::E_TypeID m_eSystemType;
    SDL_Point m_sMousePosition;
};
