#include "TCPManager.h"
#include "Object.h"
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

void TCPManager::RecvData()
{
	if (!m_pInstance->m_SocketSet)
		return;

	int nReady = SDLNet_CheckSockets(m_pInstance->m_SocketSet, GetTimeOut());
	if (nReady >= 0)
	{
		std::set<TCPsocket*>::iterator pIter = m_pInstance->m_setHost.begin();
		std::set<TCPsocket*>::iterator pCurrent{};
		while (pIter != m_pInstance->m_setHost.end() )
		{
			pCurrent = pIter;
			pIter++;
			TCPsocket pSrcSocket = *(*pCurrent);
			if (pSrcSocket && SDLNet_SocketReady(pSrcSocket))
			{
				using namespace TCP::Message;
				int nSize = sizeof(TCP::Message::S_Data);
				m_pInstance->m_pRecvSocket = &pSrcSocket;
				if (m_pInstance->RecvMessage(pSrcSocket, &m_pInstance->m_sHeader, nSize))
				{
					PrintMessageType(m_pInstance->m_sHeader.eMessageID, "Recv : ");
					if (m_pInstance->m_pRecvObject)
						m_pInstance->m_pRecvObject->RecvTCPMessage(&m_pInstance->m_sHeader);
				}
			}
		}
	}
}

bool TCPManager::RecvData(const SDLNet_SocketSet& sSocketSet, const TCPsocket& pSocket, TCP::Message::S_Data& sData)
{
	if (!sSocketSet || !pSocket)
		return false;
	int nReady = SDLNet_CheckSockets(sSocketSet, GetTimeOut());
	if (nReady >= 0 && SDLNet_SocketReady(pSocket))
	{
		int nSize = GetHeaderSize();
		int nRecv = SDLNet_TCP_Recv(pSocket, &sData, nSize);
		if (nRecv == nSize)
			return true;
		DebugMessageManager::PrintDebugMesasge_Net("TCP Recv Header Fail");
	}
	return false;
}

bool TCPManager::Close_TCP(Network::Host::S_Host* pSrc)
{
	if (!pSrc)
		return false;
	m_pInstance->CloseSocket(pSrc->sTCPSocket);
	return true;
}

bool TCPManager::Open_TCP(Network::Host::S_Host* pSrc, const char* strHost, int nSocketSetSize)
{
	int n = SDLNet_ResolveHost(&pSrc->sTCPAddress, strHost, TCPManager::GetTCPPort());
	if (n == -1)
	{
		DebugMessageManager::PrintDebugMesasge_Net("TCP Address Fail");
		return false;
	}
	pSrc->sTCPSocket = SDLNet_TCP_Open(&pSrc->sTCPAddress);
	m_pInstance->m_pHost = pSrc;
	//m_pInstance->m_SocketSet = SDLNet_AllocSocketSet(nSocketSetSize);
	//if (m_pInstance->m_SocketSet && pSrc->sTCPSocket)
	//	AddSocketSet(pSrc->sTCPSocket);
	if (!pSrc->sTCPSocket)
		DebugMessageManager::PrintDebugMesasge_Net("TCP Open Fail");
	return pSrc->sTCPSocket != nullptr;
}

void TCPManager::PrintMessageType(TCP::Message::E_MessageID eMessageType, const char* strMessage)
{
	using namespace TCP::Message;
	if (eMessageType < E_MessageID::E_None || eMessageType >= E_MessageID::E_EnumMax)
		return;
	const char* arPrints[E_EnumMax] = {
		"E_None",
		"E_Accept",				
		"E_AcceptEnd",			
		"E_AllocClientID",	
		"E_ClientEnd",
		"E_MatchingRegister",			
		"E_Matching",			
		"E_OpponentScore",	
		"E_GameEnd",
		"E_ReturnServer",
	};
	std::string str = strMessage;
	str += arPrints[eMessageType];
	DebugMessageManager::PrintDebugMesasge(str.c_str());
}

void TCPManager::PrintMessageType(void* pMessage, const char* strMessage)
{
	using namespace TCP::Message;
	S_Data sData{};
	memcpy(&sData, pMessage, sData.nMessageSize);
	PrintMessageType(sData.eMessageID, strMessage);
}

void TCPManager::MakePacket(void* pPacket, const void* sData)
{
	int nSize = GetHeaderSize();
	memcpy(pPacket, sData, nSize);
	if (m_pInstance->m_sHeader.nMessageSize - nSize > 0)
		m_pInstance->RecvMessage(*m_pInstance->m_pRecvSocket, (char*)pPacket + nSize, m_pInstance->m_sHeader.nMessageSize - nSize);
}

bool TCPManager::MakePacket(const TCPsocket& pSocket, void* pDst, const void* pSrc, int nSrcSize)
{
	if (!pSrc)
		return false;
	int nSize = GetHeaderSize();
	int nBody = nSrcSize - nSize;
	memcpy(pDst, pSrc, nSize);
	if (nBody > 0)
	{
		int nRecv = SDLNet_TCP_Recv(pSocket, (char*)pDst + nSize, nBody);
		if (nRecv != nBody)
		{
			DebugMessageManager::PrintDebugMesasge_Net("TCP Recv Body Fail : ");
			return false;
		}
	}
	return true;
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

bool TCPManager::AcceptTCPSocket(SDLNet_SocketSet& sSocketSet, TCPsocket& pServer, TCPsocket& newClient, IPaddress& newRemoteip)
{
	if (SDLNet_CheckSockets(sSocketSet, GetTimeOut()) >= 0 && SDLNet_SocketReady(pServer))
	{
		newClient = SDLNet_TCP_Accept(pServer);
		if (newClient)
		{
			IPaddress* pIAddress = SDLNet_TCP_GetPeerAddress(newClient);
			if (pIAddress)
			{
				newRemoteip = *pIAddress;
				Uint32 ipaddr = SDL_SwapBE32(newRemoteip.host);
				printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
					ipaddr >> 24,
					(ipaddr >> 16) & 0xff,
					(ipaddr >> 8) & 0xff,
					ipaddr & 0xff,
					pIAddress->port);
			}
			else
				DebugMessageManager::PrintDebugMesasge_Net("SDLNet_TCP_GetPeerAddress");
		}
		return newClient != nullptr;
	}
	return false;
}

bool TCPManager::AddSocketSet(TCPsocket& pSocekt)
{
	m_pInstance->m_setHost.insert(&pSocekt);
	if (m_pInstance->m_SocketSet)
		return SDLNet_TCP_AddSocket(m_pInstance->m_SocketSet, pSocekt) >= 0;
	else
		return false;
}

bool TCPManager::RecvMessage(TCPsocket& pSocekt, void* pMessage, int nLen)
{
	if (!pSocekt || nLen == 0)
		return false;
	int nResult = SDLNet_TCP_Recv(pSocekt, pMessage, nLen);
	if (nResult < 0)
		DebugMessageManager::PrintDebugMesasge_Net("TCP Recv Fail");
	return nResult >= 0;
}

void TCPManager::CloseSocket(TCPsocket& pSocket)
{
	if (!pSocket)
		return;
	m_pInstance->m_setHost.erase(&pSocket);
	if (m_SocketSet)
		SDLNet_TCP_DelSocket(m_SocketSet, pSocket);
	SDLNet_TCP_Close(pSocket);
	pSocket = nullptr;
}