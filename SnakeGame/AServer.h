#pragma once

#include "Actor.h"
#include "NetworkManager.h"
#include <queue>
#include "TCPManager.h"

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

	virtual bool RecvTCPMessage(void* pMessage)  override;
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength)  override;
protected:

	Network::Host::S_Host* GetEmpthySocket();

	Network::Client::S_Client* GetWaitingClient();
	void TCPRecv_MatchingRegister(void* pMessage);
	void TCPRecv_Accept(void* pMessage);
	void TCPRecv_ClientEnd(void* pMessage);
	void TCPRecv_OpponentScore(void* pMessage);
	void TCPRecv_GameEnd(void* pMessage);

	virtual void Update(Uint32 fDeltaTick) override;

	void Matching();
	
private:
	Network::Server::S_Server m_sServer;
	Network::Client::S_Client m_sDummy;
	Network::Client::S_Client* m_sClients;
	void (C_AServer::*pTCPRecv[TCP::Message::E_MessageID::E_EnumMax])(void*);
	std::queue<int> m_queMatching;
	int* m_nOpponent;

};