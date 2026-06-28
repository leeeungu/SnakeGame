#pragma once

#include "Actor.h"
#include "SDL_net.h"
#include "NetworkManager.h"
#include "TCPManager.h"
#include <queue>
#include <set>
#include <mutex>

class C_AMatchingThread;

class C_AThreadServer :public C_Actor
{
private:
	enum E_ThreadCount
	{
		E_Size = 5
	};
public:
	C_AThreadServer();
	~C_AThreadServer();

	void SendPacket(std::queue<UDPpacket*>& quePacket);
	void EmpthyPacket(std::queue<UDPpacket*>& quePacket);

protected:
	Network::Client::S_Client* GetClient(int nIndex);
	Network::Client::S_Client* GetEmpthyClient();
	void PushEmpthyClient(int nIndex);
	void PopEmpthyClient();

	Network::Client::S_Client* GetMatchingClient();
	void PushMacthcingClient(int nIndex);
	void PopEMacthcingClient();

	void CloseClient(Network::Client::S_Client& pClient);
	void SendClientEnd(Network::Client::S_Client& pClient);
	
	void Matching();

protected:
	virtual void Update(Uint32 fDeltaTick) override;
	static int UDPRecv(void* pThreadServer);

	int TCPRecv_Accept(void* pMessage);
	int TCPRecv_ClientEnd(void* pMessage);
	int TCPRecv_MatchingRegister(void* pMessage);

private:
	Network::Server::S_Server m_sServer;
	SDLNet_SocketSet m_ServerSocketSet;
	int (C_AThreadServer::* pTCPRecv[TCP::Message::E_MessageID::E_EnumMax])(void*);
	UDPpacket** m_arUDPPacket;

	Network::Client::S_Client* m_sClients;
	int* m_sThreadIndex;
	Network::Client::S_Client m_sDummy;
	std::set<int> m_setClient;
	std::queue<int> m_queEmpthyClient;
	std::queue<int> m_queMatching;
	std::mutex m_UDPMutex;
	std::queue<UDPpacket*> m_queUDPPacket;

	C_AMatchingThread** m_arThread;
	C_AMatchingThread** m_arClientThread;
	int m_nReadyThread;
};