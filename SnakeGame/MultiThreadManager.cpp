#include "MultiThreadManager.h"
#include "FrameWork.h"
#include "DebugMessageManager.h"

MultiThreadManager* MultiThreadManager::m_pInstance = nullptr;

void MultiThreadManager::CreateInstance(int nSize)
{
	if (m_pInstance)
		return;
	m_pInstance = new MultiThreadManager{};
	m_pInstance->m_nThreadSafeCount = 5;
	m_pInstance->CreateArray(nSize);
	//CreateThread(&MultiThreadManager::Test, (void*)"WorkerThread1", "스레드 실행 중");
}

void MultiThreadManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	int nTheardCount{};
	m_pInstance->m_bEnd = true;
	for (int i = 0; i < m_pInstance->m_nThreadSize; i++)
	{
		if (m_pInstance->m_arSDLThread[i])
		{
			SDL_WaitThread(m_pInstance->m_arSDLThread[i], &nTheardCount);
			m_pInstance->m_arSDLThread[i] = nullptr;
		}
	}
	delete[] m_pInstance->m_arSDLThread;
	m_pInstance->m_arSDLThread = nullptr;
	delete[] m_pInstance->m_arS_MultiThread;
	m_pInstance->m_arS_MultiThread = nullptr;
	delete m_pInstance;
	m_pInstance = nullptr;
}

int MultiThreadManager::CreateThread(int(*pFun)(void*), void* pData, const char* strWorkName)
{
	int nIndex{};
	if (!m_pInstance->GetEmpthyThreadIndex(nIndex))
	{
		DebugMessageManager::PrintDebugMesasge("No EmpthyThread");
		return false;
	}
	S_MultiThread* pThread = &m_pInstance->m_arS_MultiThread[nIndex];
	SDL_Thread** ppThread = &m_pInstance->m_arSDLThread[nIndex];
	pThread->pFun = pFun;
	pThread->pData = pData;
	pThread->bUseable = false;
	pThread->bRegister = false;
	*ppThread = SDL_CreateThread(MultiThreadManager::ThreadFunc, "Thread", pThread);
	return true;
}

bool MultiThreadManager::CanCreateThread()
{
	int nIndex{};
	if (!m_pInstance->GetEmpthyThreadIndex(nIndex))
	{
		DebugMessageManager::PrintDebugMesasge("No EmpthyThread");
		return false;
	}
	return true;
}

int MultiThreadManager::Test(void* data) {
	const char* msg = static_cast<const char*>(data);
	for (int i = 0; i < 2 && FrameWork::GetRunning(); ++i) {
		std::cout << "[Worker] " << msg << " - " << i << "\n";
		SDL_Delay(500);
	}
	return 0;
}	

int MultiThreadManager::ThreadFunc(void* data)
{
	S_MultiThread* pThread = (S_MultiThread*)data;
	if (pThread && pThread->pFun)
	{
		std::cout << "[Thead] " << pThread->nIndex << " Start" << "\n";
		pThread->pFun(pThread->pData);
		std::cout << "[Thead] " << pThread->nIndex << " End" << "\n";
		pThread->bUseable = true;
		m_pInstance->RegisterQue(pThread->nIndex);
	}
	return 0;
}

void MultiThreadManager::RegisterQue(int nIndex)
{
	S_MultiThread* pThread = &m_arS_MultiThread[nIndex];
	if (pThread->bRegister == false && pThread->bUseable == true)
	{
		m_pInstance->m_queEmpthy.push(pThread);
		pThread->bRegister = true;
	}
}

void MultiThreadManager::CreateArray(int nSize)
{
	m_nThreadSize = nSize + m_pInstance->m_nThreadSafeCount;
	m_arSDLThread = new SDL_Thread * [m_pInstance->m_nThreadSize] {};
	m_arS_MultiThread = new S_MultiThread[m_pInstance->m_nThreadSize];
	for (int i = 0; i < m_pInstance->m_nThreadSize; i++)
	{
		m_pInstance->m_arS_MultiThread[i].nIndex = i;
		RegisterQue(i);
	}
}

bool MultiThreadManager::GetEmpthyThreadIndex(int& nIndex)
{
	bool bResult{};
	S_MultiThread* pThread{};
	nIndex = -1;
	if (!m_queEmpthy.empty() && m_queEmpthy.size() > m_pInstance->m_nThreadSafeCount)
	{
		pThread = m_queEmpthy.front();
		m_queEmpthy.pop();
		nIndex = pThread->nIndex;
		return true;
	}
	return false;
}

//int MultiThreadManager::GetEmpthyThreadIndex()
//{
//	int nIndex = m_nTheardCount;
//	int nReturn{};
//	SDL_Thread*& pThread = m_arThread[0];
//
//	do
//	{
//		nIndex++;
//		if (nIndex >= m_nThreadSize)
//			nIndex = 0;
//		pThread = m_arThread[nIndex];
//		if (pThread)
//		{
//			SDL_WaitThread(pThread, &nReturn);
//			pThread = nullptr;
//		}
//	} while (pThread && nIndex != m_nTheardCount);
//	//for (int i = m_nTheardCount; i < m_nThreadSize && nIndex; i++)
//	//{
//	//	pThread = m_arThread[i];
//	//	if (!pThread)
//	//		nIndex = i;
//	//	else
//	//	{
//	//		SDL_WaitThread(pThread, &nReturn);
//	//		nIndex = i;
//	//	}
//	//
//	//}
//	return nIndex;
//}
//
