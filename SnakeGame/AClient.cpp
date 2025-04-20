#include "AClient.h"
#include "NetworkManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include "AOpponent.h"
#include "FrameWork.h"
#include "DebugMessageManager.h"
#include "ASnakeGameState.h"

C_AClient ::C_AClient ()
{
	CreateEventActorArray(E_EnumMax);
	NetworkManager::SetClient(&m_sClient, this);
}

C_AClient ::~C_AClient ()
{
	using namespace TCP::Message;
	S_LogOut sLogOut{};
	sLogOut.sData.nMessageID = m_sClient.nClientID;
	TCPManager::TCPSendMessage(m_sClient, &sLogOut);
	NetworkManager::Close_Client(&m_sClient, this);
}

void C_AClient::DelegateEventActor(int nIndex)
{
	m_pOpponent = (C_AOpponent*)GetEventActor(E_AOpponent);
	m_pGameState = (C_ASnakeGameState*)GetEventActor(E_AGameState);
}

bool C_AClient ::RecvTCPMessage(void* pMessage)
{
	using namespace TCP::Message;
	Network::Client::S_Client* pClient = &m_sClient;
	S_Data sData{};
	int nDataSize = sData.nMessageSize;
	memcpy(&sData, pMessage, sData.nMessageSize);

	if (sData.nMessageType == E_LOGIN)
	{
		S_LogIn sState{};
		TCPManager::RecvSocketMessage(&sState, sState.sData.nMessageSize);
		m_sClient.nClientID = sData.nMessageID;
	}
	else if (sData.nMessageType == E_Matching)
	{
		S_Matching sMatching{};
		TCPManager::RecvSocketMessage(&sMatching, sMatching.sData.nMessageSize);
		m_bMatching = true;
		if (m_bMatching)
		{
			using namespace::Framework::Scene;
			FrameWork::ChangeScene(E_Type::E_SnakeGame);
			FrameWork::SetUpdate(true);
			if (m_pOpponent)
				m_pOpponent->SendAll();
		}
	}
	else if (sData.nMessageType == E_LOGOUT)
	{
		FrameWork::SetRunning(false);
	}
	else if (sData.nMessageType == E_State)
	{
		S_State sState{};
		TCPManager::RecvSocketMessage(&sState, sState.sData.nMessageSize);
		if (m_pOpponent)
			m_pOpponent->SetState(sState.m_nSpeed, sState.m_nScore, sState.m_nLength);
		//if (m_pGameState && sState.m_bGameEnd)
		//{
		//	m_pGameState->GameEnd();
		//}
	}
	return true;
}

bool C_AClient ::RecvUDPMessage(void* pMessage, int nMessageLength)
{
	if (!m_pOpponent)
		return false;
	m_pOpponent->SetMap(pMessage, nMessageLength);
	return true;
}
