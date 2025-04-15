#include "WindowManager.h"
#include "SDL.h"


WindowManager* WindowManager::m_pInstance = nullptr;

void WindowManager::CreateInstance()
{
	if (m_pInstance)
	return;
	m_pInstance = new WindowManager{};
}

void WindowManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

bool WindowManager::CreateWindow(const char* strWindwoName, const SDL_Point& sSize, int& nWindowIndex, const SDL_Point& sPosition)
{
	int nIndex = m_pInstance->GetEmpthy();
	if (nIndex == E_WindowSize::E_Size)
		return false;
	S_Window* pWindow = &m_pInstance->m_arWindow[nIndex];
	pWindow->sPosition = sPosition;
	pWindow->sSize = sSize;
	m_pInstance->CreateWindow(strWindwoName, pWindow);
	nWindowIndex = nIndex;
	return true;
}

bool WindowManager::DestoryWindow(int& nIndex)
{
	if (!(nIndex >= 0 && nIndex < E_WindowSize::E_Size))
		return false;
	SDL_DestroyWindow(m_pInstance->m_arWindow[nIndex].pWindow);
	m_pInstance->m_arWindow[nIndex].pWindow = nullptr;
	return true;
}

SDL_Window* WindowManager::GetWindow(int& nIndex)
{
	SDL_Window* pWindow{};
	if (nIndex >= 0 && nIndex < E_WindowSize::E_Size)
		pWindow = m_pInstance->m_arWindow[nIndex].pWindow;
	return pWindow;
}

void WindowManager::CreateWindow(const char* strWindwoName, S_Window* pWindow)
{
	pWindow->pWindow = SDL_CreateWindow(strWindwoName, pWindow->sPosition.x, pWindow->sPosition.y, pWindow->sSize.x, pWindow->sSize.y, 0);
}

int WindowManager::GetEmpthy()
{
	int nIndex = -1;
	SDL_Window* pWindow{};
	do
	{
		nIndex++;
		pWindow = m_arWindow[nIndex].pWindow;
	} while (pWindow && nIndex < E_WindowSize::E_Size);
	return nIndex;
}
