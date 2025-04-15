#pragma once

#include "NetworkManager.h"
#include "Object.h"

class C_OClient;


class C_OServer : public C_Object
{
public:
	C_OServer();
	~C_OServer();

protected:
	virtual bool RecvTCPMessage(int nMessageType, void* pMessage, int nMessageLength)  override;
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;

	Network::Host::S_Host* GetEmpthySocket();
	void Matching(Network::Client::S_Client& sClient);
	void LogOut(Network::Client::S_Client& sClient);
private:
	Network::Server::S_Server m_sServer;
	Network::Client::S_Client m_sClients[Network::Client::E_Size];
	Network::Client::S_Client m_sDummy;
	int m_nIndex;
};