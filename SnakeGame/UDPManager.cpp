#include "UDPManager.h"
#include "NetworkManager.h"
#include "fstream"
#include "DebugMessageManager.h"


UDPManager* UDPManager::m_pInstance = nullptr;

void UDPManager::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new UDPManager{};
}

void UDPManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
		
	delete m_pInstance;
	m_pInstance = nullptr;
}

bool UDPManager::UDPSend(Network::Host::S_Host& sSrc, void* pMessage, int nLength, int nChannel)
{
	if (!pMessage || nLength <= 0)
		return false;
	memcpy(sSrc.pUDPPacket->data, pMessage, nLength);
	sSrc.pUDPPacket->len = nLength;
	sSrc.pUDPPacket->address = sSrc.sUDPAddress;
	int n = SDLNet_UDP_Send(sSrc.sUDPSocket, nChannel, sSrc.pUDPPacket);
	if (n == 0)
	{
		DebugMessageManager::PrintDebugMesasge_Net("UDP Send Fail");
	}
	return n != 0;
}

void UDPManager::RecvData()
{
	using namespace UDP::Message;
	std::set< Network::Host::S_Host*>::iterator pIter = m_pInstance->m_setHost.begin();
	while (pIter != m_pInstance->m_setHost.end())
	{
		UDPsocket& pSocket = (*pIter)->sUDPSocket;
		UDPpacket*& pPacket = (*pIter)->pUDPPacket;
		int n{};
		while (n = SDLNet_UDP_Recv(pSocket, pPacket))
		{
			S_Data* pData = (S_Data*)(pPacket->data);
			if (pData)
				NetworkManager::RunObject(Network::Protocol::E_UDP, 0,0, pPacket->data, pPacket->len);
		}
		if (n == -1)
		{
			DebugMessageManager::PrintDebugMesasge_Net("UDP Recv Fail");
		}
		pIter++;
	}
}

bool UDPManager::Open_Server(Network::Server::S_Server* sSrc, int nPacketSize)
{
	bool bResult = m_pInstance->Init_UDP(sSrc, NetworkManager::GetPortNum(), nPacketSize);
	return bResult;
}

bool UDPManager::Open_Client(Network::Client::S_Client* sSrc, int nPacketSize)
{
	bool bResult = m_pInstance->Init_UDP(sSrc, 0, nPacketSize);
	bResult &= 0 == SDLNet_ResolveHost(&sSrc->sUDPAddress, NetworkManager::GetServerIP(), NetworkManager::GetPortNum());
	return bResult;
}

bool UDPManager::Close_UDP(Network::Host::S_Host* pSrc)
{
	if (!pSrc)
		return false;
	m_pInstance->m_setHost.erase(pSrc);
	m_pInstance->FreePacket(pSrc->pUDPPacket);
	m_pInstance->CloseSocket(pSrc->sUDPSocket);
	return true;
}

bool UDPManager::Init_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize)
{
	sSrc->sUDPSocket = SDLNet_UDP_Open(nPortNum);
	sSrc->pUDPPacket = SDLNet_AllocPacket(nPacketSize);
	if (sSrc->sUDPSocket != nullptr)
		m_setHost.insert(sSrc);
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
