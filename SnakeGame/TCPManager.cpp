#include "TCPManager.h"
#include "SDL.h"
#include "Object.h"
#include "NetworkManager.h"
#include <iostream>
#include "DebugMessageManager.h"

TCPManager* TCPManager::m_pInstance = nullptr;

void TCPManager::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new TCPManager{};
	m_pInstance->m_nNetTimeout = 100;
}

void TCPManager::DestroyInstance()
{
	if (!m_pInstance)
		return;

	delete m_pInstance;
	m_pInstance = nullptr;
}

bool TCPManager::TCPSend(Network::Host::S_Host& pSrc, void* pMessage, int nLength, int nTCPIndex)
{
	return m_pInstance->SendMessage(pSrc.sTCPSocket, pMessage, nLength);
}

void TCPManager::RecvData()
{
	if (!m_pInstance->m_SocketSet)
		return;
	int nReady = SDLNet_CheckSockets(m_pInstance->m_SocketSet, m_pInstance->m_nNetTimeout);
	if (nReady >= 0)
	{
		std::set<TCPsocket*>::iterator pIter = m_pInstance->m_setHost.begin();
		std::set<TCPsocket*>::iterator pCurrent{};
		while (pIter != m_pInstance->m_setHost.end())
		{
			pCurrent = pIter;
			pIter++;
			if (m_pInstance->m_pServer)
				m_pInstance->AcceptTCPSocket();
			TCPsocket pSrcSocket = *(*pCurrent);
			if (pSrcSocket && SDLNet_SocketReady(pSrcSocket))
			{
				using namespace TCP::Message;
				m_pInstance->m_pRecvSocket = &pSrcSocket;
				S_Data sData{};
				if (m_pInstance->RecvMessage(pSrcSocket, &sData, sData.nMessageSize))
				{
					NetworkManager::RunObject(Network::Protocol::E_TCP, 0, sData.nMessageType, &sData, sData.nMessageSize);
				}
				if (pIter == m_pInstance->m_setHost.end() || !(*pCurrent))
					pSrcSocket = nullptr;
			}
		}
	}
}

bool TCPManager::GetRecvMessage(void* ppResult)
{
	if (m_pInstance->m_pRecvSocket && m_pInstance->m_pRecvMessage)
	{
		memcpy(ppResult, m_pInstance->m_pRecvMessage, m_pInstance->m_sData.nMessageSize);
		return true;
	}
	return false;
}

int TCPManager::RecvData(TCPsocket& pSocket)
{
	if (m_pInstance->m_pServer && m_pInstance->m_pServer->sTCPSocket == pSocket)
		return 0;
	using namespace TCP::Message;
	m_pInstance->m_pRecvSocket = &pSocket;
	m_pInstance->m_sData.nMessageSize = sizeof(S_Data);
	while (m_pInstance->m_pRecvSocket && SDLNet_SocketReady(m_pInstance->m_pRecvSocket))
	{
		RecvSocketMessage(&m_pInstance->m_sData, m_pInstance->m_sData.nMessageSize);
		NetworkManager::RunObject(Network::Protocol::E_TCP, 0, m_pInstance->m_sData.nMessageType, m_pInstance->m_pRecvMessage, m_pInstance->m_sData.nMessageSize);
		m_pInstance->m_pRecvSocket = nullptr;
	}
	return 0;
}

bool TCPManager::Open_Server(Network::Server::S_Server* pSrc)
{
	m_pInstance->m_pServer = pSrc;
	return m_pInstance->Open_TCP(pSrc, NULL, NetworkManager::GetPortNum());
}

bool TCPManager::Open_Client(Network::Client::S_Client* pSrc)
{
	return  m_pInstance->Open_TCP(pSrc, NetworkManager::GetServerIP(), NetworkManager::GetPortNum());
}

bool TCPManager::Close_TCP(Network::Host::S_Host* pSrc)
{
	if (!pSrc)
		return false;
	m_pInstance->m_setHost.erase(&pSrc->sTCPSocket);
	m_pInstance->CloseSocket(pSrc->sTCPSocket);
	return true;
}

bool TCPManager::RecvSocketMessage(void* pMessage, int nLen)
{
	if (m_pInstance->m_pRecvSocket)
		return m_pInstance->RecvMessage(*m_pInstance->m_pRecvSocket, pMessage, nLen);
	return false;
}

bool TCPManager::TCPSendMessage(Network::Host::S_Host& pSrc, void* pMessage)
{
	using namespace TCP::Message;
	S_Data sData{};
	int nData = sizeof(S_Data);
	memcpy(&sData, pMessage, nData);
	bool bResult{};
	bResult = m_pInstance->SendMessage(pSrc.sTCPSocket, &sData, nData);
	bResult &= m_pInstance->SendMessage(pSrc.sTCPSocket, pMessage, sData.nMessageSize);
	return bResult;
}

void TCPManager::AddSocketSet(Network::Host::S_Host* pHost)
{
	if (pHost && pHost->sTCPSocket)
	{
		int nData = SDLNet_TCP_AddSocket(m_pInstance->m_SocketSet, pHost->sTCPSocket);
		m_pInstance->m_setHost.insert(&pHost->sTCPSocket);
	}
}

void TCPManager::PrintMessageType(TCP::Message::E_Message eMessageType, const char* strMessage)
{
	std::string strPrint = strMessage;
	switch (eMessageType)
	{
	case TCP::Message::E_Accept:
		strPrint += "E_Accept";
		break;
	case TCP::Message::E_LOGIN:
		strPrint += "E_LOGIN";
		break;
	case TCP::Message::E_LOGOUT:
		strPrint += "E_LOGOUT";
		break;
	case TCP::Message::E_Matching:
		strPrint += "E_Matching";
		break;
	case TCP::Message::E_State:
		strPrint += "E_State";
		break;
	case TCP::Message::E_GameEnd:
		strPrint += "E_GameEnd";
		break;
	}
	DebugMessageManager::PrintDebugMesasge(strPrint.c_str());
}

void TCPManager::PrintMessageType(void* pMessage, const char* strMessage)
{
	using namespace TCP::Message;
	S_Data sData{};
	memcpy(&sData, pMessage, sData.nMessageSize);
	PrintMessageType(sData.nMessageType, strMessage);
}

bool TCPManager::Open_TCP(Network::Host::S_Host* pSrc, const char* strHost, Uint16 nPortNum)
{
	int n = SDLNet_ResolveHost(&pSrc->sTCPAddress, strHost, nPortNum);
	if (n == -1)
	{
		DebugMessageManager::PrintDebugMesasge_Net("TCP Address Fail");
		return false;
	}
	pSrc->sTCPSocket = SDLNet_TCP_Open(&pSrc->sTCPAddress);
	m_SocketSet = SDLNet_AllocSocketSet(pSrc->nSocketSetCount);
	if (m_SocketSet && pSrc->sTCPSocket)
		AddSocketSet(pSrc);
	else
	{
		DebugMessageManager::PrintDebugMesasge_Net("TCP Open Fail");
	}
	return pSrc->sTCPSocket != nullptr;
}

bool TCPManager::AcceptTCPSocket()
{
	TCPsocket newClient = SDLNet_TCP_Accept(m_pServer->sTCPSocket);
	IPaddress* newRemoteip{};
	if (newClient)
	{
		newRemoteip = SDLNet_TCP_GetPeerAddress(newClient);
		if (newRemoteip)
		{
			Uint32 ipaddr = SDL_SwapBE32(newRemoteip->host);
			printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
				ipaddr >> 24,
				(ipaddr >> 16) & 0xff,
				(ipaddr >> 8) & 0xff,
				ipaddr & 0xff,
				newRemoteip->port);
			TCP::Message::S_Accept sData{};
			sData.pSocket = newClient;
			sData.pAddess = newRemoteip;
			m_pInstance->m_sData.nMessageSize = sData.sData.nMessageSize;
			m_pInstance->m_sData.nMessageType = sData.sData.nMessageType;
			NetworkManager::RunObject(Network::Protocol::E_TCP, 0, sData.sData.nMessageType, &sData, sData.sData.nMessageSize);
		}
		else
			DebugMessageManager::PrintDebugMesasge_Net("SDLNet_TCP_GetPeerAddress");
	}
	return newRemoteip != nullptr;
}

bool TCPManager::SendMessage(TCPsocket& pSocekt, void* pMessage, int nLen)
{
	if (!pSocekt || !pMessage)
	{
		DebugMessageManager::PrintDebugMesasge("None Socket");
		return false;
	}
	int nResult = SDLNet_TCP_Send(pSocekt, pMessage, nLen);
	if (nResult < 0)
		DebugMessageManager::PrintDebugMesasge_Net("TCP Socket is invalid");
	else if (nResult == nLen)
		PrintMessageType(pMessage, "Send : ");
	return nResult >= 0;
}

bool TCPManager::RecvMessage(TCPsocket& pSocekt, void* pMessage, int nLen)
{
	if (!pSocekt || nLen == 0)
		return false;
	int nResult = SDLNet_TCP_Recv(pSocekt, pMessage, nLen);
	if (nResult < 0)
		DebugMessageManager::PrintDebugMesasge_Net("TCP Recv Fail");
	else if (nResult == nLen)
		PrintMessageType(pMessage, "Recv : ");
	return nResult >= 0;
}

void TCPManager::CloseSocket(TCPsocket& pSocket)
{
	if (!pSocket)
		return;
	SDLNet_TCP_DelSocket(m_SocketSet, pSocket);
	SDLNet_TCP_Close(pSocket);
	pSocket = nullptr;
}