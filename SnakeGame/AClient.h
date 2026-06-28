#pragma once

#include "Actor.h"
#include <mutex>
#include "NetworkManager.h"
#include <atomic>
#include "TCPManager.h"
#include "ATestPlayer.h"

class C_AOpponent;
class C_ASnakeGameState;

class C_AClient : public C_Actor
{
public:
	enum E_EventActor
	{
		E_AOpponent,
		E_AGameState,
		E_EnumMax
	};
public:
	C_AClient();
	~C_AClient();

	//virtual bool RecvTCPMessage(void* pMessage)  override;
	bool RecvTCPMessage();
	bool RecvUDPMessage();
	//virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;
protected:
	virtual void DelegateEventActor(int nIndex) override;

	void TCPRecv_AcceptEnd(void* pMessage);
	void TCPRecv_AllocClientID(void* pMessage);
	void TCPRecv_OpponentScore(void* pMessage);
	void TCPRecv_Matching(void* pMessage);
	void TCPRecv_GameEnd(void* pMessage);
	void TCPRecv_ReturnServer(void* pMessage);
	virtual void Update(Uint32 fDeltaTick) override;

private:
	Network::Client::S_Client m_sClient;
	C_AOpponent* m_pOpponent;
	C_ASnakeGameState* m_pGameState;
	void (C_AClient::* pTCPRecv[TCP::Message::E_MessageID::E_EnumMax])(void*);
	int m_nOpponentID;
	std::atomic<bool> m_bMatching;
	C_ATestPlayer* m_pPlayer;
	SDLNet_SocketSet m_SocketSet;
};