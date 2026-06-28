#pragma once

#include "Actor.h"
#include "NetworkManager.h"
#include "TCPManager.h"
#include <atomic>
#include <queue>
#include <mutex>

class C_AThreadServer;

class C_AMatchingThread : public C_Actor
{
private:
	enum E_Client
	{
		E_Size = 2
	};
public:
	C_AMatchingThread();
	~C_AMatchingThread();

	void Init(C_AThreadServer* pMain, Network::Server::S_Server* pServer, Network::Client::S_Client* pSrc, Network::Client::S_Client* pDst);

	bool RecvPacket(UDPpacket* quePacket, int nIndex);
	bool GetRunning() { return m_bRunning.load(); }
	void SetServeEnd() { m_bEndGame = true; }
protected:
	void SetClient(Network::Server::S_Server* pServer, Network::Client::S_Client* pSrc, Network::Client::S_Client* pDst, int nIndex);
	void TCPFunc(int nIndex);
	void UDPFunc(int nIndex);
	static int ThreadFunc(void* pMatchingThread);
	void CloseClient(Network::Client::S_Client* pSrc);

	int TCPRecv_OpponentScore(void* pMessage);
	int TCPRecv_GameEnd(void* pMessage);
	int TCPRecv_ClientEnd(void* pMessage);

private:
	C_AThreadServer* m_pMainServer;
	SDLNet_SocketSet m_MatchingSocketSet;
	Network::Client::S_Client* m_pClients[E_Client::E_Size];
	Network::Client::S_Client* m_pOpponent[E_Client::E_Size];
	int (C_AMatchingThread::* pTCPRecv[TCP::Message::E_MessageID::E_EnumMax])(void*);
	std::queue<UDPpacket*> m_queUDPPacket[E_Client::E_Size];
	std::mutex m_UDPMutex;
	std::atomic< bool> m_bRunning;
	bool m_bEndGame;
};