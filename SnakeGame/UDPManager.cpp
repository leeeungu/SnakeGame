#include "UDPManager.h"
#include "Object.h"
#include "DebugMessageManager.h"

UDPManager* UDPManager::m_pInstance = nullptr;

void UDPManager::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new UDPManager{};
	m_pInstance->m_nPacketSize = 512;
}

void UDPManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	delete m_pInstance;
	m_pInstance = nullptr;
}

bool UDPManager::UDPSend(UDPsocket& pSocket, UDPpacket*& pPacket, int nChannel)
{
	if (!pPacket || !pSocket)
		return false;
	int n = SDLNet_UDP_Send(pSocket, nChannel, pPacket);
	if (n == 0)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDP Send Fail");
	}
	return n != 0;
}

bool UDPManager::UDPSend(Network::Host::S_Host& sSrc, void* pMessage, int nLength, int nChannel)
{
	if (!sSrc.pUDPPacket || !sSrc.sUDPSocket || !pMessage || nLength <= 0)
		return false;
	memcpy(sSrc.pUDPPacket->data, pMessage, nLength);
	sSrc.pUDPPacket->len = nLength;
	int n = SDLNet_UDP_Send(sSrc.sUDPSocket, nChannel, sSrc.pUDPPacket);
	if (n == 0)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDP Send Fail");
	}
	return n != 0;
}

bool UDPManager::Open_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize)
{
	sSrc->sUDPSocket = SDLNet_UDP_Open(nPortNum);
	m_pInstance->m_pHost = sSrc;
	m_pInstance->m_pHost->pUDPPacket = SDLNet_AllocPacket(m_pInstance->m_nPacketSize);
	if (!sSrc->sUDPSocket)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDP Open Fail");
	}
	return sSrc->sUDPSocket != nullptr;
}

bool UDPManager::ResolveHost(Network::Host::S_Host* sSrc, const char* host)
{
	return SDLNet_ResolveHost(&sSrc->sUDPAddress, host, UDPManager::GetUDPPort()) == 1;
}

bool UDPManager::Close_UDP(Network::Host::S_Host* pSrc)
{
	if (!pSrc)
		return false;
	if (pSrc->pUDPPacket)
		m_pInstance->FreePacket(pSrc->pUDPPacket);
	pSrc->pUDPPacket = nullptr;
	m_pInstance->CloseSocket(pSrc->sUDPSocket);
	return true;
}

void UDPManager::PrintMessage(UDP::Message::S_Data& sData, const char* strMessage)
{
	using namespace UDP::Message;
	if (sData.eMessageID < E_MessageID::E_None || sData.eMessageID >= E_MessageID::E_EnumMax)
		return;
	const char* arPrints[E_EnumMax] = {
		"E_None",
		"E_SnakeMap",
	};
	std::string str = strMessage;
	str+= arPrints[sData.eMessageID];
	DebugMessageManager::PrintDebugMesasge(str.c_str());
}


bool UDPManager::Init_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize)
{
	sSrc->sUDPSocket = SDLNet_UDP_Open(nPortNum);
	m_pInstance->m_pHost = sSrc;
	m_pInstance->m_pHost->pUDPPacket = SDLNet_AllocPacket(m_pInstance->m_nPacketSize);
	if (!sSrc->sUDPSocket)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDP Open Fail");
	}
	return sSrc->sUDPSocket != nullptr;
}

void UDPManager::CloseSocket(UDPsocket& pSocket)
{
	if (pSocket)
		SDLNet_UDP_Close(pSocket);
	pSocket = nullptr;
}

void UDPManager::FreePacket(UDPpacket*& pPacket)
{
	if (pPacket)
		SDLNet_FreePacket(pPacket);
	pPacket = nullptr;
}

bool UDPManager::CheckPacket(const UDP::Message::S_Data& sData)
{
	return CheckPacket(sData.nClientID, sData.eMessageID);
}

bool UDPManager::CheckPacket(int nClientID, UDP::Message::E_MessageID eMessageID)
{
	if (0 <= nClientID && nClientID < Network::Server::E_ClientSize::E_Size &&
		UDP::Message::E_MessageID::E_None < eMessageID && eMessageID < UDP::Message::E_MessageID::E_EnumMax)
		return true;
	return false;
}

void UDPManager::RecvData()
{
	if (!m_pInstance->m_pHost)
		return;
	using namespace UDP::Message;
	Network::Host::S_Host* pHost = m_pInstance->m_pHost;
	UDPsocket pSocket = pHost->sUDPSocket;
	UDPpacket* pPacket = m_pInstance->m_pHost->pUDPPacket;
	int n = 1;
	while (pSocket && pPacket && n == 1)
	{
		n = SDLNet_UDP_Recv(pSocket, pPacket);
		if (n == 1)
		{
			S_Data sRecv{};
			memcpy(&sRecv, pPacket->data, sRecv.nMessageSize);
			if (sRecv.nMessageSize == pPacket->len && m_pInstance->CheckPacket(sRecv))
			{
				m_pInstance->m_pRecvObject->RecvUDPMessage(pPacket->data, pPacket->len);
			}
		}
	}
	if (n == -1)
		DebugMessageManager::PrintDebugMesasge_Net("UDPError");
}

bool UDPManager::RecvData(UDPsocket& pSocket, UDPpacket*& pPacket)
{
	if (!pSocket || !pPacket)
		return false;
	int n = SDLNet_UDP_Recv(pSocket, pPacket);
	if (n == 1)
		return true;
	if (n == -1)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDPError");
	}
	return false;
}
