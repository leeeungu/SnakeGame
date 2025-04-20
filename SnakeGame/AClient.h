#pragma once

#include "Actor.h"
#include "NetworkManager.h"

class C_AOpponent;
class C_ASnakeGameState;

class C_AClient : public C_Actor
{
public:
	enum E_EventActor
	{
		E_Map,
		E_AOpponent,
		E_AGameState,
		E_EnumMax
	};
public:
	C_AClient();
	~C_AClient();

protected:
	virtual void DelegateEventActor(int nIndex) override;
	virtual bool RecvTCPMessage(void* pMessage)  override;
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;
private:
	Network::Client::S_Client m_sClient;
	C_AOpponent* m_pOpponent;
	C_ASnakeGameState* m_pGameState;
	bool m_bMatching;
};