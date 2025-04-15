#pragma once

#include "SDL.h"

class MultiThreadManager
{
private:
	MultiThreadManager() = default;
	static MultiThreadManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static int worker(void* data);
	static void CreateThread(int (*pFun)(void*), const char* strName, void* pData);
	static void CreateArray(int nSize);
private:
	int GetEmpthyThreadIndex();

private:
	int m_nThreadSize = 1;
	int m_nTheardCount;
	SDL_Thread** m_arThread;
};