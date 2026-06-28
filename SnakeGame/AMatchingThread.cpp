#include "AMatchingThread.h"
#include "MultiThreadManager.h"
#include "UDPManager.h"
#include "AThreadServer.h"

C_AMatchingThread::C_AMatchingThread()
{
	using namespace TCP::Message;
	pTCPRecv[E_OpponentScore] = &C_AMatchingThread::TCPRecv_OpponentScore;
	pTCPRecv[E_GameEnd] = &C_AMatchingThread::TCPRecv_GameEnd;
	pTCPRecv[E_ClientEnd] = &C_AMatchingThread::TCPRecv_ClientEnd;

	m_MatchingSocketSet = SDLNet_AllocSocketSet(E_Client::E_Size);
	SetRegisterUpdate(false);
	SetUpdate(false);
}

C_AMatchingThread::~C_AMatchingThread()
{
	SDLNet_FreeSocketSet(m_MatchingSocketSet);
}

void C_AMatchingThread::Init(C_AThreadServer* pMain, Network::Server::S_Server* pServer, Network::Client::S_Client* pSrc, Network::Client::S_Client* pDst)
{
	m_pMainServer = pMain;
	m_bRunning = true;
	m_bEndGame = false;
	SetClient(pServer, pSrc, pDst, 0);
	SetClient(pServer, pDst, pSrc, 1);
	MultiThreadManager::CreateThread(&C_AMatchingThread::ThreadFunc, this);
}

bool C_AMatchingThread::RecvPacket(UDPpacket* quePacket, int nIndex)
{
	m_UDPMutex.lock();
	if (m_bRunning)
	{
		m_queUDPPacket[nIndex].push(quePacket);
		if (m_pClients[nIndex])
			m_pClients[nIndex]->sUDPAddress = quePacket->address;
	}
	m_UDPMutex.unlock();
	return m_bRunning;
}

void C_AMatchingThread::SetClient(Network::Server::S_Server* pServer, Network::Client::S_Client* pSrc, Network::Client::S_Client* pDst, int nIndex)
{
	SDLNet_TCP_AddSocket(m_MatchingSocketSet, pSrc->sTCPSocket);
	m_pClients[nIndex] = pSrc;
	m_pOpponent[nIndex] = pDst;
	pSrc->sUDPSocket = pServer->sUDPSocket;

	using namespace TCP::Message;
	S_Matching sSend{};
	sSend.sData.nClientID = pSrc->nClientID;
	sSend.nMatchingID = pDst->nClientID;
	TCPManager::SendMessage(pSrc->sTCPSocket, &sSend, sSend.sData.nMessageSize);
}

int C_AMatchingThread::ThreadFunc(void* pMatchingThread)
{
	C_AMatchingThread* pThread = (C_AMatchingThread*)pMatchingThread;
	if (!pThread)
		return -1;
	int nIndex{};
	while (!pThread->m_bEndGame)
	{
		pThread->TCPFunc(nIndex);
		pThread->UDPFunc(nIndex);
		nIndex++;
		if (nIndex >= E_Client::E_Size)
			nIndex = 0;
	}
	pThread->m_pMainServer->EmpthyPacket(pThread->m_queUDPPacket[0]);
	pThread->m_pMainServer->EmpthyPacket(pThread->m_queUDPPacket[1]);
	pThread->m_bRunning = false;
	return 0;
}

void C_AMatchingThread::TCPFunc(int nIndex)
{
	Network::Client::S_Client* pSrc = m_pClients[nIndex];
	using namespace TCP::Message;
	S_Data sData{};
	if (TCPManager::RecvData(m_MatchingSocketSet, pSrc->sTCPSocket, sData) &&
		pSrc->nClientID == sData.nClientID &&
		E_MessageID::E_None < sData.eMessageID && 
		sData.eMessageID < E_MessageID::E_EnumMax &&
		pTCPRecv[sData.eMessageID] != nullptr)
	{
		(this->*pTCPRecv[sData.eMessageID])(&nIndex);
	}
}

void C_AMatchingThread::UDPFunc(int nIndex)
{
	using namespace UDP::Message;
	Network::Client::S_Client* pSrc = m_pClients[nIndex];
	Network::Client::S_Client* pDst = m_pOpponent[nIndex];
	if (!pSrc || !pDst)
		return;

	m_UDPMutex.lock();
	std::queue<UDPpacket*> quePacket{};
	if (!m_queUDPPacket[nIndex].empty())
	{
		int nSize = (int)m_queUDPPacket[nIndex].size();
		while (nSize >= 0)
		{
			UDPpacket* pPacket = m_queUDPPacket[nIndex].front();
			m_queUDPPacket[nIndex].pop();
			m_queUDPPacket[nIndex].push(pPacket);
			S_Data sRecv{};
			memcpy(&sRecv, pPacket->data, sRecv.nMessageSize);
			if (pSrc->nClientID == sRecv.nClientID &&
				E_MessageID::E_None < sRecv.eMessageID && sRecv.eMessageID < E_MessageID::E_EnumMax)
			{
				pPacket->address = pDst->sUDPAddress;
				quePacket.push(pPacket);
			}
			nSize--;
		}
	}
	m_pMainServer->SendPacket(quePacket);
	m_pMainServer->EmpthyPacket(m_queUDPPacket[nIndex]);
	m_UDPMutex.unlock();
}

void C_AMatchingThread::CloseClient(Network::Client::S_Client* pSrc)
{
	SDLNet_TCP_DelSocket(m_MatchingSocketSet, pSrc->sTCPSocket);
	if (pSrc->sTCPSocket)
		SDLNet_TCP_Close(pSrc->sTCPSocket);
	pSrc->sTCPSocket = nullptr;
}

int C_AMatchingThread::TCPRecv_OpponentScore(void* pMessage)
{
	int nIndex = *(int*)pMessage;
	Network::Client::S_Client* pSrc = m_pClients[nIndex];
	Network::Client::S_Client* pDst = m_pOpponent[nIndex];
	if (!pSrc || !pDst)
		return -1;
	using namespace TCP::Message;
	S_OpponentScore sRecv{};
	if (TCPManager::MakePacket(pSrc->sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		sRecv.sData.nClientID = pDst->nClientID;
		TCPManager::SendMessage(pDst->sTCPSocket, &sRecv, sRecv.sData.nMessageSize);
	}
	return 0;
}

int C_AMatchingThread::TCPRecv_GameEnd(void* pMessage)
{
	int nIndex = *(int*)pMessage;
	Network::Client::S_Client* pSrc = m_pClients[nIndex];
	Network::Client::S_Client* pDst = m_pOpponent[nIndex];
	if (!pSrc || !pDst)
		return -1;

	using namespace TCP::Message;
	S_GameEnd sRecv{};
	if (TCPManager::MakePacket(pSrc->sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		sRecv.sData.nClientID = pDst->nClientID;
		sRecv.bGameResult = !sRecv.bGameResult;
		TCPManager::SendMessage(pDst->sTCPSocket, &sRecv, sRecv.sData.nMessageSize);
		{
			S_ReturnServer sSend{};
			if (pSrc && pSrc->sTCPSocket)
			{
				sSend.sData.nClientID = pSrc->nClientID;
				TCPManager::SendMessage(pSrc->sTCPSocket, &sSend, sSend.sData.nMessageSize);
			}
			if (pDst && pDst->sTCPSocket)
			{
				sSend.sData.nClientID = pDst->nClientID;
				TCPManager::SendMessage(pDst->sTCPSocket, &sSend, sSend.sData.nMessageSize);
			}
		}
		CloseClient(pSrc);
		CloseClient(pDst);
		m_bEndGame = true;
	}
	return 0;
}

int C_AMatchingThread::TCPRecv_ClientEnd(void* pMessage)
{
	int nIndex = *(int*)pMessage;
	Network::Client::S_Client* pSrc = m_pClients[nIndex];
	Network::Client::S_Client* pDst = m_pOpponent[nIndex];
	if (!pSrc || !pDst)
		return -1;
	using namespace TCP::Message;
	S_ClientEnd sRecv{};
	if (TCPManager::MakePacket(pSrc->sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		{
			S_GameEnd sSend{};
			sSend.bGameResult = !m_bEndGame;
			sSend.sData.nClientID = pDst->nClientID;
			TCPManager::SendMessage(pDst->sTCPSocket, &sSend, sSend.sData.nMessageSize);
		}
		{
			S_ReturnServer sSend{};
			sSend.sData.nClientID = pDst->nClientID;
			TCPManager::SendMessage(pDst->sTCPSocket, &sSend, sSend.sData.nMessageSize);
		}
		CloseClient(pSrc);
		CloseClient(pDst);
		m_bEndGame = true;
	}
	return 0;
}
