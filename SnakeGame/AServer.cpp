#include "AServer.h"
#include "DebugMessageManager.h"
#include "UDPManager.h"
#include "TCPManager.h"

C_AServer::C_AServer()
{
	TCPManager::Open_TCP(&m_sServer, NULL,  m_sServer.nClientSize + 1);
	UDPManager::Open_UDP(&m_sServer, UDPManager::GetUDPPort(),  UDPManager::GetPacketSize());
	TCPManager::SetObject(this);
	UDPManager::SetObject(this);
	m_sClients = new Network::Client::S_Client[m_sServer.nClientSize]{};
	for (int i = 0; i < Network::Server::E_Size; i++)
	{
		m_sClients[i].nClientID = i;
		m_sClients[i].pUDPPacket = m_sServer.pUDPPacket;
	}
	m_nOpponent = new int[m_sServer.nClientSize] {};

	using namespace TCP::Message;
	pTCPRecv[E_Accept] = &C_AServer::TCPRecv_Accept;
	pTCPRecv[E_ClientEnd] = &C_AServer::TCPRecv_ClientEnd;
	pTCPRecv[E_MatchingRegister] = &C_AServer::TCPRecv_MatchingRegister;
	pTCPRecv[E_GameEnd] = &C_AServer::TCPRecv_GameEnd;
	pTCPRecv[E_OpponentScore] = &C_AServer::TCPRecv_OpponentScore;

	SetRegisterUpdate(true);
	SetUpdate(true);
}

C_AServer::~C_AServer()
{
	using namespace TCP::Message;
	delete[] m_nOpponent;
	m_nOpponent = nullptr;
	S_AcceptEnd sSend{};
	for (int i = 0; i < Network::Server::E_Size; i++)
	{
		if (m_sClients[i].sTCPSocket)
		{
			sSend.sData.nClientID = m_sClients[i].nClientID;
			TCPManager::SendMessage(m_sClients[i].sTCPSocket, &sSend, sSend.sData.nMessageSize);
		}
	}
	UDPManager::Close_UDP(&m_sServer);
	TCPManager::Close_TCP(&m_sServer);
	delete[] m_sClients;
	m_sClients = nullptr;
}

bool C_AServer::RecvTCPMessage(void* pMessage)
{
	using namespace TCP::Message;
	S_Data sData{};
	memcpy(&sData, pMessage, sData.nMessageSize);
	if (0 <= sData.nClientID  && sData.nClientID < m_sServer.nClientSize
		&& E_MessageID::E_None < sData.eMessageID && sData.eMessageID < E_MessageID::E_EnumMax
		&& pTCPRecv[sData.eMessageID])
	{
		(this->*pTCPRecv[sData.eMessageID])(pMessage);
		return true;
	}
	return false;
}

bool C_AServer::RecvUDPMessage(void* pMessage, int nMessageLength)
{
	using namespace UDP::Message;
	S_Data sRecv{};
	memcpy(&sRecv, pMessage, sRecv.nMessageSize);
	if (sRecv.nMessageSize == nMessageLength)
	{
		UDPpacket* pPacket = m_sServer.pUDPPacket;
		m_sClients[sRecv.nClientID].sUDPAddress = pPacket->address;
		pPacket->address = m_sClients[m_nOpponent[sRecv.nClientID]].sUDPAddress;
		UDPManager::PrintMessage(sRecv, "Recv : ");
		UDPManager::UDPSend(m_sClients[m_nOpponent[sRecv.nClientID]], pMessage, nMessageLength);
	}
	return false;
}

Network::Host::S_Host* C_AServer::GetEmpthySocket()
{
	int i{};
	Network::Host::S_Host* pHost = &m_sDummy;
	while (i < Network::Server::E_Size && m_sClients[i].sTCPSocket)
	{
		i++;
	}
	if (i < Network::Server::E_Size && !m_sClients[i].sTCPSocket)
		pHost = &m_sClients[i];
	return pHost;
}

Network::Client::S_Client* C_AServer::GetWaitingClient()
{
	Network::Client::S_Client* pSrc{};
	while (!pSrc && !m_queMatching.empty())
	{
		int nIndex = m_queMatching.front();
		m_queMatching.pop();
		pSrc = &m_sClients[nIndex];
		if (pSrc && !pSrc->sTCPSocket)
			pSrc = nullptr;
	}
	return pSrc;
}

void C_AServer::TCPRecv_MatchingRegister(void* pMessage)
{
	using namespace TCP::Message;
	S_MatchingRegister sRecv{};
	TCPManager::MakePacket(&sRecv, pMessage);
	int nClientID = sRecv.sData.nClientID;
	Network::Client::S_Client* sClient = &m_sClients[nClientID];
	m_queMatching.push(sClient->nClientID);
}

void C_AServer::TCPRecv_Accept(void* pMessage)
{
	using namespace TCP::Message;
	Network::Client::S_Client* pClient = (Network::Client::S_Client*)GetEmpthySocket();
	/*if (!TCPManager::AcceptTCPSocket( m_sServer.sTCPSocket, pClient->sTCPSocket, pClient->sTCPAddress))
		return;*/

	if (pClient == &m_sDummy)
	{
		S_AcceptEnd sSend{};
		TCPManager::SendMessage(pClient->sTCPSocket, &sSend, sSend.sData.nMessageSize);
		return;
	}
	TCPManager::AddSocketSet(pClient->sTCPSocket);
	S_AllocClientID sSend{};
	sSend.sData.nClientID = pClient->nClientID;
	sSend.nClientID = pClient->nClientID;
	TCPManager::SendMessage(pClient->sTCPSocket, &sSend, sSend.sData.nMessageSize);
}

void C_AServer::TCPRecv_ClientEnd(void* pMessage)
{
	using namespace TCP::Message;
	S_ClientEnd sRecv{};
	TCPManager::MakePacket(&sRecv, pMessage);
	int nClientID = sRecv.sData.nClientID;
	if (nClientID < 0 || nClientID >= Network::Server::E_Size)
		return;
	Network::Client::S_Client& sClient = m_sClients[nClientID];
	TCPManager::Close_TCP(&sClient);
}

void C_AServer::TCPRecv_OpponentScore(void* pMessage)
{
	using namespace TCP::Message;
	S_OpponentScore sRecv{};
	TCPManager::MakePacket(&sRecv, pMessage);
	Network::Client::S_Client* pPre = &m_sClients[sRecv.sData.nClientID];
	Network::Client::S_Client* pNext = &m_sClients[m_nOpponent[sRecv.sData.nClientID]];
	if (pNext)
	{
		sRecv.sData.nClientID = pNext->nClientID;
		TCPManager::SendMessage(pNext->sTCPSocket, &sRecv, sRecv.sData.nMessageSize);
	}
}

void C_AServer::TCPRecv_GameEnd(void* pMessage)
{
	using namespace TCP::Message;
	S_GameEnd sRecv{};
	TCPManager::MakePacket(&sRecv, pMessage);
	Network::Client::S_Client* pNext = &m_sClients[m_nOpponent[sRecv.sData.nClientID]];
	if (pNext)
	{
		sRecv.sData.nClientID = pNext->nClientID;
		sRecv.bGameResult = !sRecv.bGameResult;
		TCPManager::SendMessage(pNext->sTCPSocket, &sRecv, sRecv.sData.nMessageSize);
	}
}

void C_AServer::Update(Uint32 fDeltaTick)
{
	TCPRecv_Accept(&fDeltaTick);
	TCPManager::RecvData();
	Matching();
	UDPManager::RecvData();
}

void C_AServer::Matching()
{
	if (m_queMatching.size() >= 2)
	{
		Network::Client::S_Client* pSrc = GetWaitingClient();
		Network::Client::S_Client* pDst = GetWaitingClient();
		if (pSrc && pDst)
		{
			m_nOpponent[pSrc->nClientID] = pDst->nClientID;
			m_nOpponent[pDst->nClientID] = pSrc->nClientID;
			pSrc->sUDPSocket = m_sServer.sUDPSocket;
			pDst->sUDPSocket = m_sServer.sUDPSocket;

			using namespace TCP::Message;
			S_Matching sSend{};
			sSend.sData.nClientID = pSrc->nClientID;
			sSend.nMatchingID = pDst->nClientID;
			TCPManager::SendMessage(pSrc->sTCPSocket, &sSend, sSend.sData.nMessageSize);
			sSend.sData.nClientID = pDst->nClientID;
			sSend.nMatchingID = pSrc->nClientID;
			TCPManager::SendMessage(pDst->sTCPSocket, &sSend, sSend.sData.nMessageSize);
			
		}
		else
		{
			if (pSrc)
				m_queMatching.push(pSrc->nClientID);
			if (pDst)
				m_queMatching.push(pDst->nClientID);
		}
	}
}
