#include "AServer.h"
#include "DebugMessageManager.h"
#include "NetworkManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include <iostream>
#include "AClient.h"

C_AServer::C_AServer()
{
	NetworkManager::SetServer(&m_sServer, this);
	for (int i = 0; i < Network::Client::E_Size; i++)
	{
		m_sClients[i].nClientID = i;
	}
}

C_AServer::~C_AServer()
{
	using namespace TCP::Message;
	S_LogOut sLogOut{};
	for (int i = 0; i < Network::Client::E_Size; i++)
	{
		LogOut(m_sClients[i]);
		if (m_sClients[i].pUDPPacket)
			SDLNet_FreePacket(m_sClients[i].pUDPPacket);
		m_sClients[i].pUDPPacket = nullptr;
	}
	NetworkManager::Close_Server(&m_sServer, this);
}

bool C_AServer::RecvTCPMessage(void* pMessage)
{
	using namespace TCP::Message;
	S_Data sData{};
	int nDataSize = sData.nMessageSize;
	memcpy(&sData, pMessage, sData.nMessageSize);

	if (sData.nMessageType == E_Accept)
	{
		S_Accept sAccept{};
		memcpy(&sAccept, pMessage, sAccept.sData.nMessageSize);
		Network::Client::S_Client* pClient = (Network::Client::S_Client*)GetEmpthySocket();
		pClient->sTCPSocket = sAccept.pSocket;
		pClient->sTCPAddress = *sAccept.pAddess;
		if (pClient == &m_sDummy)
		{
			LogOut(m_sDummy);
			return false;
		}

		S_LogIn sLogin{};
		sLogin.sData.nMessageID = pClient->nClientID;
		TCPManager::TCPSendMessage(*pClient, &sLogin);
		TCPManager::AddSocketSet(pClient);
	}
	else if (sData.nMessageType == E_Matching)
	{
		S_Matching sMatching{};
		TCPManager::RecvSocketMessage(&sMatching, sMatching.sData.nMessageSize);
		int nID = sMatching.sData.nMessageID;
		Matching(m_sClients[nID]);
	}
	else if (sData.nMessageType == E_LOGOUT)
	{
		S_LogOut sLogOut{};
		TCPManager::RecvSocketMessage(&sLogOut, sLogOut.sData.nMessageSize);
		int nID = sLogOut.sData.nMessageID;
		if (nID >= 0 && nID < Network::Client::E_Size)
		{
			LogOut(m_sClients[nID]);
		}
	}
	else if (sData.nMessageType == E_State)
	{
		S_State sState{};
		TCPManager::RecvSocketMessage(&sState, sState.sData.nMessageSize);
		int nID = sState.sData.nMessageID;
		Network::Host::S_Host* pPre = &m_sClients[nID];
		int n = (nID + 1) % Network::Client::E_Size;
		Network::Host::S_Host* pNext = &m_sClients[n];
		if (pNext->pUDPPacket)
		{
			TCPManager::TCPSendMessage(*pNext, &sState);
		}
	}
	//else if (sData.nMessageType == E_State)
	//{
	//	S_State sState{};
	//	TCPManager::GetRecvMessage(&sState);
	//	//TCPManager::RecvSocketMessage(&sState, sState.sData.nMessageSize);
	//	int nID = sState.sData.nMessageID;
	//	Network::Host::S_Host* pPre = &m_sClients[nID];
	//	int n = (nID + 1) % Network::Client::E_Size;
	//	Network::Host::S_Host* pNext = &m_sClients[n];
	//	if (pNext->pUDPPacket)
	//	{
	//		TCPManager::TCPSendMessage(*pNext, &sState);
	//	}
	//}
	return true;
}

bool C_AServer::RecvUDPMessage(void* pMessage, int nMessageLength)
{
	using namespace UDP::Message;
	S_Data* pData = (S_Data*)(pMessage);
	Network::Host::S_Host* pPre = &m_sClients[pData->nClientID];
	pPre->pUDPPacket = m_sServer.pUDPPacket;
	pPre->sUDPSocket = m_sServer.sUDPSocket;
	pPre->sUDPAddress = m_sServer.pUDPPacket->address;

	int n = (pData->nClientID + 1) % Network::Client::E_Size;
	Network::Host::S_Host* pNext = &m_sClients[n];
	m_sServer.pUDPPacket->address = pNext->sUDPAddress;

	if (pNext->pUDPPacket)
	{
		memcpy(pNext->pUDPPacket->data, pMessage, nMessageLength);
		UDPManager::UDPSend(*pNext, pMessage, nMessageLength);
	}
	return true;
}

Network::Host::S_Host* C_AServer::GetEmpthySocket()
{
	int i{};
	Network::Host::S_Host* pHost = &m_sDummy;
	while (i < Network::Client::E_Size && m_sClients[i].sTCPSocket)
	{
		i++;
	}
	if (i < Network::Client::E_Size && !m_sClients[i].sTCPSocket)
		pHost = &m_sClients[i];
	return pHost;
}

void C_AServer::Matching(Network::Client::S_Client& sClient)
{
	if (m_nMatchingCount < E_MatchingCount::E_Count)
	{
		m_sMatching[m_nMatchingCount] = &sClient;
		m_nMatchingCount++;
	}
	if (m_nMatchingCount == E_MatchingCount::E_Count)
	{
		using namespace TCP::Message;
		S_Matching sMatching{};
		Network::Client::S_Client* pClient{};
		for (size_t i = 0; i < E_MatchingCount::E_Count; i++)
		{
			pClient = m_sMatching[i];
			if (pClient)
			{
				sMatching.sData.nMessageID = pClient->nClientID;
				if (!TCPManager::TCPSendMessage(*pClient, &sMatching))
				{
					char temp[19] = " Mathching전송실패";
					temp[0] = pClient->nClientID + '0';
					DebugMessageManager::PrintDebugMesasge(temp);
				}
			}
		}
		m_nMatchingCount = 0;
	}
}

void C_AServer::LogOut(Network::Client::S_Client& sClient)
{
	using namespace TCP::Message;
	S_LogOut sLogOut{};
	sLogOut.sData.nMessageID = sClient.nClientID;
	if (!TCPManager::TCPSendMessage(sClient, &sLogOut))
	{
		char temp[16] = " LogOut전송실패";
		temp[0] = sClient.nClientID + '0';
		DebugMessageManager::PrintDebugMesasge(temp);
	}
	else
	{
		TCPManager::Close_TCP(&sClient);
	}
	sClient.sTCPSocket = nullptr;
}
