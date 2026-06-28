#include "AClient.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include "AOpponent.h"
#include "FrameWork.h"
#include "DebugMessageManager.h"
#include "ASnakeGameState.h"

C_AClient ::C_AClient ()
{
	CreateEventActorArray(E_EnumMax);
	TCPManager::Open_TCP(&m_sClient, TCPManager::GetServerIP(), 1);
	UDPManager::Open_UDP(&m_sClient,0, UDPManager::GetPacketSize());
	UDPManager::ResolveHost(&m_sClient, UDPManager::GetServerIP());
	TCPManager::SetObject(this);
	UDPManager::SetObject(this);
	if (!m_sClient.sTCPSocket)
	{
		FrameWork::SetRunning(false);
		return;
	}
	using namespace TCP::Message;
	pTCPRecv[E_MessageID::E_AllocClientID] = &C_AClient::TCPRecv_AllocClientID;
	pTCPRecv[E_MessageID::E_AcceptEnd] = &C_AClient::TCPRecv_AcceptEnd;
	pTCPRecv[E_MessageID::E_OpponentScore] = &C_AClient::TCPRecv_OpponentScore;
	pTCPRecv[E_MessageID::E_Matching] = &C_AClient::TCPRecv_Matching;
	pTCPRecv[E_MessageID::E_GameEnd] = &C_AClient::TCPRecv_GameEnd;
	pTCPRecv[E_MessageID::E_ReturnServer] = &C_AClient::TCPRecv_ReturnServer;
	
	SetRegisterUpdate(true);
	SetUpdate(true);
	m_pPlayer = new C_ATestPlayer{};

	m_SocketSet = SDLNet_AllocSocketSet(2);
	SDLNet_TCP_AddSocket(m_SocketSet, m_sClient.sTCPSocket);
}

C_AClient ::~C_AClient ()
{
	SDLNet_FreeSocketSet(m_SocketSet);
	delete m_pPlayer;
	m_pPlayer = nullptr;
	using namespace TCP::Message;
	m_bMatching = false;
	S_ClientEnd sSend{};
	sSend.sData.nClientID = m_sClient.nClientID;
	TCPManager::SendMessage(m_sClient.sTCPSocket, &sSend, sSend.sData.nMessageSize);
	UDPManager::Close_UDP(&m_sClient);
	TCPManager::Close_TCP(&m_sClient);
}

bool C_AClient::RecvTCPMessage()
{
	using namespace TCP::Message;
	S_Data sData{};
	if (TCPManager::RecvData(m_SocketSet, m_sClient.sTCPSocket, sData) &&
		E_MessageID::E_None < sData.eMessageID && sData.eMessageID < E_MessageID::E_EnumMax &&
		pTCPRecv[sData.eMessageID] != nullptr)
	{
		(this->*pTCPRecv[sData.eMessageID])(&sData);
	}
	
	return true;
}

bool C_AClient::RecvUDPMessage()
{
	using namespace UDP::Message;
	while (UDPManager::RecvData(m_sClient.sUDPSocket, m_sClient.pUDPPacket))
	{
		S_Data sRecv{};
		memcpy(&sRecv, m_sClient.pUDPPacket->data, sRecv.nMessageSize);
		if (0 <= sRecv.nClientID &&
			sRecv.nMessageSize == m_sClient.pUDPPacket->len &&
			UDPManager::CheckPacket(sRecv) && m_pOpponent)
		{
			m_pOpponent->SetMap(m_sClient.pUDPPacket->data, sRecv.nMessageSize);
		}
	}
	return true;
}

void C_AClient::DelegateEventActor(int nIndex)
{
	m_pOpponent = (C_AOpponent*)GetEventActor(E_AOpponent);
	m_pGameState = (C_ASnakeGameState*)GetEventActor(E_AGameState);
	if (m_pGameState)
	{
		m_pGameState->SetClientID(m_sClient.nClientID);
	}
}

void C_AClient::TCPRecv_AcceptEnd(void* pMessage)
{
	using namespace TCP::Message;
	S_ClientEnd sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		FrameWork::SetRunning(false);
	}
}

void C_AClient::TCPRecv_AllocClientID(void* pMessage)
{
	using namespace TCP::Message; 
	S_AllocClientID sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		m_sClient.nClientID = sRecv.nClientID;
		if (m_pGameState)
		{
			m_pGameState->SetClientID(m_sClient.nClientID);
		}
	}
}

void C_AClient::TCPRecv_OpponentScore(void* pMessage)
{
	using namespace TCP::Message;
	S_OpponentScore sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize) && m_pOpponent)
	{
		m_pOpponent->SetState(sRecv.nSpeed, sRecv.nScore, sRecv.nLength);
	}
}

void C_AClient::TCPRecv_Matching(void* pMessage)
{
	using namespace TCP::Message;
	S_Matching sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		m_bMatching = true;
		m_nOpponentID = sRecv.nMatchingID;
		FrameWork::ChangeScene(Framework::Scene::E_Type::E_SnakeGame);
		if (m_pOpponent)
		{
			m_pOpponent->SetOpponentID(sRecv.nMatchingID);
			m_pOpponent->SendAll();
		}
	}
}

void C_AClient::TCPRecv_GameEnd(void* pMessage)
{
	using namespace TCP::Message;
	S_GameEnd sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		m_bMatching = false;
		if (m_pGameState)
		{
			if (m_pPlayer)
				m_pPlayer->ReturnTitle();
			m_pGameState->GameResult(sRecv.bGameResult);
		}
	}
}

void C_AClient::TCPRecv_ReturnServer(void* pMessage)
{
	using namespace TCP::Message;
	S_ReturnServer sRecv{};
	if (TCPManager::MakePacket(m_sClient.sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
	{
		TCPManager::Close_TCP(&m_sClient);
		TCPManager::Open_TCP(&m_sClient, TCPManager::GetServerIP(), 1);
	}
}

void C_AClient::Update(Uint32 fDeltaTick)
{
	RecvTCPMessage();
	RecvUDPMessage();
}