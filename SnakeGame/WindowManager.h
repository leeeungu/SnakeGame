#pragma once

#include "SDL.h"
#include <queue>

class WindowManager
{
private:
	enum E_WindowSize
	{
		E_Size = 2
	};

	struct S_Window
	{
		SDL_Window* pWindow;
		SDL_Point sPosition;
		SDL_Point sSize;
	};
	WindowManager() = default;
	static WindowManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static bool CreateWindow(const char* strWindwoName, const SDL_Point& sSize, int& nWindowIndex, const SDL_Point& sPosition = {200,100});
	static bool DestoryWindow(int& nIndex);
	static SDL_Window* GetWindow(int& nIndex);
private:
	void CreateWindow(const char* strWindwoName, S_Window* pWindow);
	int GetEmpthy();
private:
	S_Window m_arWindow[E_WindowSize::E_Size];
};