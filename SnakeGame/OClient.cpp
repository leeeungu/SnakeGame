#include "OClient.h"
#include "NetworkManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include "AOpponent.h"
#include "FrameWork.h"
#include "DebugMessageManager.h"

C_OClient::C_OClient()
{
	NetworkManager::SetClient(&m_sClient, this);
}

C_OClient::~C_OClient()
{
	using namespace TCP::Message;
	S_LogOut sLogOut{};
	sLogOut.sData.nMessageID = m_sClient.nClientID;
	TCPManager::TCPSendMessage(m_sClient, &sLogOut);
	UDPManager::Close_UDP(&m_sClient);
	TCPManager::Close_TCP(&m_sClient);
}

bool C_OClient::RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength)
{
	using namespace TCP::Message;
	Network::Client::S_Client* pClient = &m_sClient;
	S_Data sData{};
	int nDataSize = sData.nMessageSize;
	memcpy(&sData, pMessage, sData.nMessageSize);

	if (nMessageType == E_LOGIN)
	{
		S_LogIn sLogin{};
		TCPManager::RecvSocketMessage(&sLogin, sLogin.sData.nMessageSize);
		m_sClient.nClientID = sLogin.sData.nMessageID;
	}
	else if (nMessageType == E_Matching)
	{
		S_Matching sMatching{};
		memcpy(&sMatching, pMessage, nMessageLength);
		m_bMatching = true;
		if (m_bMatching)
		{
			FrameWork::SetUpdate(true);
			if (m_pOpponent)
				m_pOpponent->SendAll();
		}
	}
	else if (nMessageType == E_LOGOUT)
	{
		FrameWork::SetRunning(false);
	}
	else if (nMessageType == E_State)
	{
		S_State sState{};
		TCPManager::RecvSocketMessage(&sState, sState.sData.nMessageSize);
		if (m_pOpponent)
			m_pOpponent->SetState(sState.m_nSpeed, sState.m_nScore, sState.m_nLength);
	}
	return true;
}

bool C_OClient::RecvUDPMessage(void* pMessage, int nMessageLength)
{
	if (!m_pOpponent)
		return false;
	m_pOpponent->SetMap(pMessage, nMessageLength);
	return true;
}
