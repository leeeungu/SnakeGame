#include "NetworkManager.h"
//
//NetworkManager* NetworkManager::m_pInstance = nullptr;
//
//void NetworkManager::CreateInstance()
//{
//	if (m_pInstance)
//		return;
//	SDLNet_Init();
//	m_pInstance = new NetworkManager{};
//	m_pInstance->SetServerIPPort();
//}
//
//void NetworkManager::DestroyInstance()
//{
//	if (!m_pInstance)
//		return;
//	delete m_pInstance;
//	m_pInstance = nullptr;
//	SDLNet_Quit();
//}
//
//void NetworkManager::SetServerIPPort()
//{
//	std::vector<std::string> arData{};
//	arData.resize(2);
//	if (FileReader::ReadFile("../Resources/ServerIP/ServerIP.txt", arData))
//	{
//		m_pInstance->m_strServer = arData[0];
//		TCPManager::SetTCPPort(std::stoi(arData[1]));
//		UDPManager::SetUDPPort(std::stoi(arData[1]));
//	}
//}

//Network::Client::S_Client* pClient = (Network::Client::S_Client*)TCPManager::GetHost();
//if (pClient)
//{
//	sSend.sData.nClientID = pClient->nClientID;
//	TCPManager::TCPSendMessage(*pClient, &sSend);
//}
//	if (eSocketType == Network::Protocol::E_ProtocolType::E_UDP)
//	{
//		using namespace UDP::Message;
//		S_Data* pData = (S_Data*)(pMessage);
//		pData->nClientID = m_pInstance->m_pClient->nClientID;
//		if (m_pInstance->m_pClient->pUDPPacket)
//			m_pInstance->m_pClient->pUDPPacket->address = m_pInstance->m_pClient->sUDPAddress;
//		bResult = UDPManager::UDPSend(*m_pInstance->m_pClient, pMessage, nMessageLength);
//	}
//	else if (eSocketType == Network::Protocol::E_ProtocolType::E_TCP)
//	{
//		using namespace TCP::Message;
//		S_Data sData{};
//		int nSize = sData.nMessageSize;
//		memcpy(&sData, pMessage, sData.nMessageSize);
//		sData.nMessageID = m_pInstance->m_pClient->nClientID;
//		memcpy(pMessage, &sData, nSize);
//		bResult = TCPManager::TCPSendMessage(*m_pInstance->m_pClient, pMessage);
//	}
//	return bResult;
//}
// 
// 
//bool NetworkManager::SendMessage_2Server(Network::Protocol::E_ProtocolType eSocketType, void* pMessage, int nMessageLength)
//{
//	if (!m_pInstance->m_pClient)
//		return false;
//	bool bResult{};
//	
//	if (eSocketType == Network::Protocol::E_ProtocolType::E_UDP)
//	{
//		using namespace UDP::Message;
//		S_Data* pData = (S_Data*)(pMessage);
//		pData->nClientID = m_pInstance->m_pClient->nClientID;
//		if (m_pInstance->m_pClient->pUDPPacket)
//			m_pInstance->m_pClient->pUDPPacket->address = m_pInstance->m_pClient->sUDPAddress;
//		bResult = UDPManager::UDPSend(*m_pInstance->m_pClient, pMessage, nMessageLength);
//	}
//	else if (eSocketType == Network::Protocol::E_ProtocolType::E_TCP)
//	{
//		using namespace TCP::Message;
//		S_Data sData{};
//		int nSize = sData.nMessageSize;
//		memcpy(&sData, pMessage, sData.nMessageSize);
//		sData.nMessageID = m_pInstance->m_pClient->nClientID;
//		memcpy(pMessage, &sData, nSize);
//		bResult = TCPManager::TCPSendMessage(*m_pInstance->m_pClient, pMessage);
//	}
//	return bResult;
//}

