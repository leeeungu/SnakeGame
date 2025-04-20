#pragma once

#include "SDL_net.h"
#include "NetworkManager.h"
#include <set>

class C_Object;

namespace TCP
{
	namespace Message
	{
		enum E_Message
		{
			E_NONE,
			E_Accept,
			E_LOGIN,
			E_LOGOUT,
			E_Matching,
			E_State,
			E_GameEnd,
			E_EnumMax
		};
		struct S_Data
		{
			S_Data()
				:nMessageID{}, nMessageSize{}, nMessageType{}
			{
				nMessageSize = sizeof(S_Data);
			}
			int nMessageID;
			int nMessageSize;
			E_Message nMessageType;
		};
		//bool bHasNext;

		struct S_Accept
		{
			S_Accept()
				: sData{}, pSocket{}, pAddess{}
			{
				sData.nMessageSize = sizeof(S_Accept);
				sData.nMessageType = E_Message::E_Accept;
			}
			S_Data sData;
			TCPsocket pSocket;
			IPaddress* pAddess;
		};

		struct S_LogIn
		{
			S_LogIn()
				: sData{}, nDummy{}
			{
				sData.nMessageSize = sizeof(S_LogIn);
				sData.nMessageType = E_Message::E_LOGIN;
			}
			S_Data sData;
			int nDummy;
		};

		struct S_LogOut
		{
			S_LogOut()
				: sData{}, nDummy{}
			{
				sData.nMessageSize = sizeof(S_LogOut);
				sData.nMessageType = E_Message::E_LOGOUT;
			}
			S_Data sData;
			int nDummy;
		};

		struct S_Matching
		{
			S_Matching()
				: sData{}, bMatchingID{}
			{
				sData.nMessageSize = sizeof(S_Matching);
				sData.nMessageType = E_Message::E_Matching;
			}
			S_Data sData;
			int bMatchingID;
		};

		struct S_State
		{	
			S_State()
				: sData{}, m_nSpeed{}, m_nScore {}, m_nLength{}
			{
				sData.nMessageSize = sizeof(S_State);
				sData.nMessageType = E_Message::E_State;
			}
			S_Data sData;
			int m_nSpeed;
			int m_nScore;
			int m_nLength;
		};

		struct S_GameEnd
		{
			S_GameEnd()
				: sData{}, m_bGaemResult{}
			{
				sData.nMessageSize = sizeof(S_GameEnd);
				sData.nMessageType = E_Message::E_GameEnd;
			}
			S_Data sData;
			bool m_bGaemResult;
		};
		//struct S_
	}
}

class TCPManager
{
private:
	TCPManager() = default;
	static TCPManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static bool TCPSend(Network::Host::S_Host& pSrc, void* pMessage, int nLength, int nTCPIndex = 0);
	static void RecvData();
	static bool Open_Server(Network::Server::S_Server* pSrc);
	static bool Open_Client(Network::Client::S_Client* pSrc);
	static bool Close_TCP(Network::Host::S_Host* pSrc);
	static Network::Server::S_Server* GetServer() { return m_pInstance->m_pServer; }

	static bool RecvSocketMessage(void* pMessage, int nLen);
	static TCPsocket* GetRecvSocket() { return m_pInstance->m_pRecvSocket; }
	static bool TCPSendMessage(Network::Host::S_Host& pSrc, void* pMessage);
	static void AddSocketSet(Network::Host::S_Host* pHost);
	

	static TCP::Message::E_Message GetMessageType() { return m_pInstance->m_sData.nMessageType; }
	static bool GetRecvMessage(void* ppResult);
	static int GetMessageSize() { return m_pInstance->m_sData.nMessageSize; }
	static int GetMessageID() { return m_pInstance->m_sData.nMessageID; }
	static int RecvData(TCPsocket& pSocket);
private:
	bool Open_TCP(Network::Host::S_Host* pSrc, const char* strHost, Uint16 nPortNum);
	bool AcceptTCPSocket();
	void PrintMessageType(TCP::Message::E_Message eMessageType, const char* strMessage); 
	void PrintMessageType(void* pMessage, const char* strMessage);
	bool SendMessage(TCPsocket& pSocekt, void* pMessage, int nLen);
	bool RecvMessage(TCPsocket& pSocekt, void* pMessage, int nLen);
	void CloseSocket(TCPsocket& pSocket);

private:
	std::set<TCPsocket*> m_setHost;
	Network::Server::S_Server* m_pServer;
	TCPsocket* m_pRecvSocket;
	Uint32 m_nNetTimeout;
	SDLNet_SocketSet m_SocketSet;

	TCP::Message::S_Data m_sData;
	void* m_pRecvMessage;
};