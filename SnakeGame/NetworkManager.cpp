#include "NetworkManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include "DebugMessageManager.h"
#include "Object.h"
#include "FileReader.h"
#include <iostream>

NetworkManager* NetworkManager::m_pInstance = nullptr;

void NetworkManager::CreateInstance()
{
	if (m_pInstance)
		return;
	SDLNet_Init();
	m_pInstance = new NetworkManager{};
	TCPManager::CreateInstance();
	UDPManager::CreateInstance();
	m_pInstance->SetServerIPPort();
	m_pInstance->CreateEventArray(Network::Message::Snake::E_EnumMax);
}

void NetworkManager::DestroyInstance()
{
	if (!m_pInstance)
		return;

	for (int i = 0; i < Network::Protocol::E_ProtocolType::E_EnumMax; i++)
	{
		if (m_pInstance->m_arRecvObject[i])
		{
			delete[] m_pInstance->m_arRecvObject[i];
			m_pInstance->m_arRecvObject[i] = nullptr;
		}

	}
	delete[] m_pInstance->m_arRecvObject;
	m_pInstance->m_arRecvObject = nullptr;

	UDPManager::DestroyInstance();
	TCPManager::DestroyInstance();
	delete m_pInstance;
	m_pInstance = nullptr;
	SDLNet_Quit();
}

void NetworkManager::TickUpdate()
{
	UDPManager::RecvData();
	TCPManager::RecvData();
}

void NetworkManager::SetServerObject(Network::Protocol::E_ProtocolType eSocketType, C_Object* pObject)
{
	SetRecvObject(eSocketType, pObject, 0);
}

void NetworkManager::SetRecvObject(Network::Protocol::E_ProtocolType eSocketType,  C_Object* pObject, int nIndex)
{
	if (m_pInstance->m_nObjectSize > nIndex && nIndex >= 0)
	{
		m_pInstance->m_arRecvObject[eSocketType][nIndex] = pObject;
	}
}

void NetworkManager::CreateEventArray(int nSize)
{
	if (m_pInstance->m_nObjectSize > 0)
		return;
	m_pInstance->m_nObjectSize = nSize;
	m_pInstance->m_arRecvObject = new C_Object * *[Network::Protocol::E_ProtocolType::E_EnumMax] {};
	m_pInstance->m_arRecvObject[Network::Protocol::E_ProtocolType::E_TCP] = new C_Object * [m_pInstance->m_nObjectSize] {};
	m_pInstance->m_arRecvObject[Network::Protocol::E_ProtocolType::E_UDP] = new C_Object * [m_pInstance->m_nObjectSize] {};
}

void NetworkManager::SetServer(Network::Server::S_Server* pSrc, C_Object* pObject)
{
	SetRecvObject(Network::Protocol::E_UDP, pObject);
	SetRecvObject(Network::Protocol::E_TCP, pObject);
	TCPManager::Open_Server(pSrc);
	UDPManager::Open_Server(pSrc);
}

void NetworkManager::SetClient(Network::Client::S_Client* pSrc, C_Object* pObject)
{
	m_pInstance->m_pClient = pSrc;
	SetRecvObject(Network::Protocol::E_UDP, pObject);
	SetRecvObject(Network::Protocol::E_TCP, pObject);
	TCPManager::Open_Client(pSrc);
	UDPManager::Open_Client(pSrc);
}

bool NetworkManager::SendMessage_2Server(Network::Protocol::E_ProtocolType eSocketType, void* pMessage, int nMessageLength)
{
	if (!m_pInstance->m_pClient)
		return false;
	bool bResult{};
	
	if (eSocketType == Network::Protocol::E_ProtocolType::E_UDP)
	{
		using namespace UDP::Message;
		S_Data* pData = (S_Data*)(pMessage);
		pData->nClientID = m_pInstance->m_pClient->nClientID;
		bResult = UDPManager::UDPSend(*m_pInstance->m_pClient, pMessage, nMessageLength);
	}
	else if (eSocketType == Network::Protocol::E_ProtocolType::E_TCP)
	{
		using namespace TCP::Message;
		S_Data sData{};
		int nSize = sData.nMessageSize;
		memcpy(&sData, pMessage, sData.nMessageSize);
		sData.nMessageID = m_pInstance->m_pClient->nClientID;
		memcpy(pMessage, &sData, nSize);
		bResult = TCPManager::TCPSendMessage(*m_pInstance->m_pClient, pMessage);
	}
	return bResult;
}

void NetworkManager::RunObject(Network::Protocol::E_ProtocolType eSocketType, int nIndex, int nMessageType, void* pMessage, int MessageLength)
{
	m_pInstance->RunObject_Internal(eSocketType, nIndex, nMessageType, pMessage, MessageLength);
}

void NetworkManager::RunObject_Internal(Network::Protocol::E_ProtocolType eSocketType,  int nIndex, int nMessageType, void* pMessage, int MessageLength)
{
	C_Object* pObject{};
	pObject = m_arRecvObject[eSocketType][nIndex]; 
	if (pObject)
	{
		pObject->RecvObject(eSocketType, nMessageType, pMessage, MessageLength);
	}
}

void NetworkManager::SetServerIPPort()
{
	std::vector<std::string> arData{};
	arData.resize(2);
	if (FileReader::ReadFile("../Resources/ServerIP/ServerIP.txt", arData))
	{
		m_strServer = arData[0];
		m_nPortNum = std::stoi(arData[1]);
		//DebugMessageManager::PrintDebugMesasge(m_strServer.c_str());
		//DebugMessageManager::PrintDebugMesasge(arData[1].c_str());
	}
}
