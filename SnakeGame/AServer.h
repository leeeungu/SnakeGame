#pragma once

#include "NetworkManager.h"
#include "Actor.h"

class C_OClient;


class C_AServer : public C_Actor
{
private:
	enum  E_MatchingCount
	{
		E_Count = 2
	};
public:
	C_AServer();
	~C_AServer();

protected:
	virtual bool RecvTCPMessage(void* pMessage)  override;
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;

	Network::Host::S_Host* GetEmpthySocket();
	void Matching(Network::Client::S_Client& sClient);
	void LogOut(Network::Client::S_Client& sClient);
private:
	Network::Server::S_Server m_sServer;
	Network::Client::S_Client m_sClients[Network::Client::E_Size];

	Network::Client::S_Client* m_sMatching[E_MatchingCount::E_Count];
	int m_nMatchingCount;
	Network::Client::S_Client m_sDummy;
	int m_nIndex;
};