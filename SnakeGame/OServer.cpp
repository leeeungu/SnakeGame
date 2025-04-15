#include "OServer.h"
#include "DebugMessageManager.h"
#include "NetworkManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include <iostream>
#include "OClient.h"

C_OServer::C_OServer()
{
	NetworkManager::SetServer(&m_sServer, this);
	for (int i = 0; i < Network::Client::E_Size; i++)
	{
		m_sClients[i].nClientID = i;
	}
}

C_OServer::~C_OServer()
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
	UDPManager::Close_UDP(&m_sServer);
	TCPManager::Close_TCP(&m_sServer);
}

bool C_OServer::RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength)
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
		memcpy(&sData, &sLogin.sData, nDataSize);
		TCPManager::TCPSendMessage(*pClient, &sLogin);
		TCPManager::AddSocketSet(pClient);
		if (pClient->nClientID == 1)
		{
			Matching(m_sClients[0]);
			Matching(m_sClients[1]);
		}
	}
	else if (nMessageType == E_LOGOUT)
	{
		S_LogOut sLogOut{};
		TCPManager::RecvSocketMessage(&sLogOut, sLogOut.sData.nMessageSize);
		int nID = sLogOut.sData.nMessageID;
		if (nID >= 0 && nID < Network::Client::E_Size)
		{
			LogOut(m_sClients[nID]);
		}
	}
	else if (nMessageType == E_State)
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
	return true;
}

bool C_OServer::RecvUDPMessage(void* pMessage, int nMessageLength)
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

Network::Host::S_Host* C_OServer::GetEmpthySocket()
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

void C_OServer::Matching(Network::Client::S_Client& sClient)
{
	using namespace TCP::Message;
	S_Matching sMatching{};
	sMatching.sData.nMessageID = sClient.nClientID;
	if (!TCPManager::TCPSendMessage(sClient, &sMatching))
	{
		char temp[19] = " Mathching전송실패";
		temp[0] = sClient.nClientID + '0';
		DebugMessageManager::PrintDebugMesasge(temp);
	}
}

void C_OServer::LogOut(Network::Client::S_Client& sClient)
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
