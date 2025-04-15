#include "MultiThreadManager.h"
#include "FrameWork.h"
#include <iostream>
MultiThreadManager* MultiThreadManager::m_pInstance = nullptr;

int MultiThreadManager::worker(void* data) {
	const char* msg = static_cast<const char*>(data);
	for (int i = 0; i < 5 && FrameWork::GetRunning(); ++i) {
		std::cout << "[Worker] " << msg << " - " << i << "\n";
		SDL_Delay(500);
	}
	return 0;
}

void MultiThreadManager::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new MultiThreadManager{};

	//CreateThread(worker, "WorkerThread", (void*)"스레드 실행 중");
}

void MultiThreadManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	for (int i = 0; i < m_pInstance->m_nThreadSize; i++)
	{
		if (m_pInstance->m_arThread[i])
			SDL_WaitThread(m_pInstance->m_arThread[i], &m_pInstance->m_nTheardCount);
	}
	delete[] m_pInstance->m_arThread;
	m_pInstance->m_arThread = nullptr;
	delete m_pInstance;
	m_pInstance = nullptr;
}

void MultiThreadManager::CreateThread(int(*pFun)(void*), const char* strName, void* pData)
{
	int nIndex = m_pInstance->GetEmpthyThreadIndex();
	m_pInstance->m_arThread[nIndex] = SDL_CreateThread(pFun, strName, pData);
	std::cout << "[Worker] "<< nIndex << " : " << strName << "\n";
}

void MultiThreadManager::CreateArray(int nSize)
{
	m_pInstance->m_nThreadSize = nSize;
	m_pInstance->m_arThread = new SDL_Thread * [m_pInstance->m_nThreadSize] {};
}

int MultiThreadManager::GetEmpthyThreadIndex()
{
	int nIndex = m_nTheardCount;
	int nReturn{};
	SDL_Thread*& pThread = m_arThread[0];

	do
	{
		nIndex++;
		if (nIndex >= m_nThreadSize)
			nIndex = 0;
		pThread = m_arThread[nIndex];
		if (pThread)
		{
			SDL_WaitThread(pThread, &nReturn);
			pThread = nullptr;
		}
	} while (pThread && nIndex != m_nTheardCount);
	//for (int i = m_nTheardCount; i < m_nThreadSize && nIndex; i++)
	//{
	//	pThread = m_arThread[i];
	//	if (!pThread)
	//		nIndex = i;
	//	else
	//	{
	//		SDL_WaitThread(pThread, &nReturn);
	//		nIndex = i;
	//	}
	//
	//}
	return nIndex;
}

