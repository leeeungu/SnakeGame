#include "OWindow.h"
#include "InputHandleManager.h"
#include "FrameWork.h"

C_OWindow::C_OWindow()
{
	using namespace InputHandle;
	InputHandleManager::RegisterSystem(System::InputType::E_TypeID::E_QUIT, this);
	//InputHandleManager::RegisterKeyBoard(KeyBoard::InputType::E_KEYUP, KeyBoard::ValueType::E_Enter, this);
}

C_OWindow::~C_OWindow()
{
	using namespace InputHandle;
	//InputHandleManager::UnRegisterKeyBoard(KeyBoard::InputType::E_KEYUP, KeyBoard::ValueType::E_Enter, this);
	InputHandleManager::UnRegisterSystem(System::InputType::E_TypeID::E_QUIT, this);
}

void C_OWindow::HandleEvent()
{
	using namespace InputHandle;
	if (InputHandleManager::GetCurrentSystemType() == System::InputType::E_TypeID::E_QUIT)
	{
		FrameWork::SetRunning(false);
	}
}
