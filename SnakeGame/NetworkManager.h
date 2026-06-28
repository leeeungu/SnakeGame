#pragma once

#include "SDL_net.h"

namespace Network
{
	namespace Protocol
	{
		enum E_ProtocolType
		{
			E_None,
			E_TCP,
			E_UDP,
			E_EnumMax
		};
	}
	namespace Host
	{
		struct S_Host
		{
			S_Host() = default;
			virtual ~S_Host() = default;

			IPaddress sTCPAddress;
			IPaddress sUDPAddress;
			TCPsocket sTCPSocket;
			UDPsocket sUDPSocket;
			UDPpacket* pUDPPacket;
		};
	}
	namespace Client
	{
		
		struct S_Client : public Network::Host::S_Host
		{
			S_Client() = default;
			~S_Client() = default;
			int nClientID;
		};
	}

	namespace Server
	{
		enum E_ClientSize
		{
			E_Size = 10
		};

		struct S_Server: public Network::Host::S_Host
		{
			S_Server() = default;
			~S_Server() = default;
			int nClientSize = E_ClientSize::E_Size;
		};
	}
}
//
//class NetworkManager
//{
//	friend TCPManager;
//	friend UDPManager;
//private:
//	NetworkManager() = default;
//	static NetworkManager* m_pInstance;
//protected:
//	static void CreateInstance();
//	static void DestroyInstance();
//
//	static void SetClient(Network::Client::S_Client* pSrc) { m_pInstance->m_pClient = pSrc; }
//	static const char* GetServerIP() { return m_pInstance->m_strServer.c_str(); }
//	//static bool SendMessage_2Server(Network::Protocol::E_ProtocolType eSocketType, void* pMessage, int nMessageLength);
//	static void SetServerIPPort();
//private:
//	std::string m_strServer;
//	Network::Client::S_Client* m_pClient;
//};