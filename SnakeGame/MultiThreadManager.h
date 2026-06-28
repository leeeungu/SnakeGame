#pragma once

#include "SDL.h"
#include <set>
#include <queue>

/*
queCreate 관련 된거를 큐에 등록 해두고 가능한 스레드가 나오면 그때 create 하는 방시긍로
*/

class MultiThreadManager
{
private:
	struct S_MultiThread
	{
		S_MultiThread() = default;
		int nIndex;
		int (*pFun)(void*);
		void* pData;
		bool bUseable = true;
		bool bRegister = false;
	};
	MultiThreadManager() = default;
	static MultiThreadManager* m_pInstance;
public:
	static void CreateInstance(int nSize);
	static void DestroyInstance();
	static bool GetProgramOver() { return m_pInstance->m_bEnd; }
	static int CreateThread(int (*pFun)(void*), void* pData, const char* strWorkName = "ThreadWork");
	static bool CanCreateThread();
private:
	static int Test(void* data);
	static int ThreadFunc(void* data);
	void RegisterQue(int nIndex);
	void CreateArray(int nSize);
	bool GetEmpthyThreadIndex(int& nIndex);

private:
	int m_nThreadSize;
	int m_nThreadSafeCount;
	S_MultiThread* m_arS_MultiThread;
	SDL_Thread** m_arSDLThread;
	SDL_Thread* m_pDummy;
	std::queue< S_MultiThread*> m_queEmpthy;
	bool m_bEnd;
};