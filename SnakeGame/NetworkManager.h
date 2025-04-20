#pragma once

#include "SDL_net.h"
#include <set>
#include <string>

class TCPManager;
class UDPManager;
class C_Object;

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
		enum E_HostType
		{
			E_Client,
			E_Server,
			E_EnumMax
		};

		struct S_Host
		{
			S_Host() = default;
			virtual ~S_Host() = default;

			IPaddress sTCPAddress;
			IPaddress sUDPAddress;
			TCPsocket sTCPSocket;
			UDPsocket sUDPSocket;
			UDPpacket* pUDPPacket;
			int nSocketSetCount = 5;
		};
	}
	namespace Server
	{
		enum E_ServerSize
		{
			E_Size = 1
		};

		struct S_Server: public Network::Host::S_Host
		{
			~S_Server()
			{
			}
			int nClientSize;
		};
	}

	namespace Client
	{
		enum E_ClientSize
		{
			E_Size = 10
		};
		struct S_Client : public Network::Host::S_Host
		{
			int nClientID;
		};
	}
	namespace Message
	{
		namespace Snake
		{
			enum E_ObjectType
			{
				E_Object,
				E_EnumMax
			};
		}
	}
}

class NetworkManager
{
	friend TCPManager;
	friend UDPManager;
private:
	NetworkManager() = default;
	static NetworkManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();
	static void TickUpdate();

	static void SetServerObject(Network::Protocol::E_ProtocolType eSocketType, C_Object* pObject);
	static void SetRecvObject(Network::Protocol::E_ProtocolType eSocketType, C_Object* pObject, int nIndex = 0);
	static void CreateEventArray(int nSize);
	static void SetServer(Network::Server::S_Server* pSrc, C_Object* pObject);
	static void SetClient(Network::Client::S_Client* pSrc, C_Object* pObject);
	static void Close_Server(Network::Server::S_Server* pSrc, C_Object* pObject);
	static void Close_Client(Network::Client::S_Client* pSrc, C_Object* pObject);
	static const char* GetServerIP() { return m_pInstance->m_strServer.c_str(); }
	static Uint16 GetPortNum() { return m_pInstance->m_nPortNum; }

	static bool SendMessage_2Server(Network::Protocol::E_ProtocolType eSocketType, void* pMessage, int nMessageLength);
protected:
	static void RunObject(Network::Protocol::E_ProtocolType eSocketType, int nIndex, int nMessageType, void* pMessage, int MessageLength);
	void RunObject_Internal(Network::Protocol::E_ProtocolType eSocketType, int nIndex, int nMessageType, void* pMessage, int MessageLength);

	void SetServerIPPort();
private:
	int m_nObjectSize;
	std::string m_strServer;
	Uint16 m_nPortNum;
	int m_nClientID;
	Network::Client::S_Client* m_pClient;
	C_Object*** m_arRecvObject;
};