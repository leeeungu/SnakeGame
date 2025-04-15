#pragma once

#include "object.h"
#include "NetworkManager.h"

class C_AOpponent;

class C_OClient : public C_Object
{
public:
	C_OClient();
	~C_OClient();

	void SetMap(C_AOpponent* pOpponent) { m_pOpponent = pOpponent; }

protected:
	virtual bool RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength)  override;
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;
private:
	Network::Client::S_Client m_sClient;
	C_AOpponent* m_pOpponent;
	bool m_bMatching;
};