#pragma once

#include "SDL_net.h"
#include "NetworkManager.h"
#include <set>
#include <string>
#include <queue>

class C_Object;

namespace TCP
{
	namespace Message
	{
		enum E_MessageID
		{
			E_None,
			E_Accept,				 // tcp 연결
			E_AcceptEnd,			 // tcp 연결 실패
			E_AllocClientID,		 // tcp 연결 성공 이후 id 배분
			E_ClientEnd,			 // 클라 종료
			E_MatchingRegister,		 // 클라에서 보내져서 matching 큐에 등록
			E_Matching,				// Matching끝이후 서버로 돌아 갈때
			E_OpponentScore,		 // 상대방 점수
			E_GameEnd,				 // 대결 종료
			E_ReturnServer,
			E_EnumMax
		};
		struct S_Data
		{
			S_Data()
				:nClientID{}, nMessageSize{}, eMessageID{}
			{
				nMessageSize = sizeof(S_Data);
			}
			int nClientID;
			int nMessageSize;
			E_MessageID eMessageID;
		};

		struct S_Accept
		{
			S_Accept()
				: sData{}, pSocket{}, pAddess{}
			{
				sData.nMessageSize = sizeof(S_Accept);
				sData.eMessageID = E_MessageID::E_Accept;
			}
			S_Data sData;
			TCPsocket pSocket;
			IPaddress* pAddess;
		};

		struct S_AcceptEnd
		{
			S_AcceptEnd()
				: sData{}, nDummy{}
			{
				sData.nMessageSize = sizeof(S_AcceptEnd);
				sData.eMessageID = E_MessageID::E_AcceptEnd;
			}
			S_Data sData;
			int nDummy;
		};
		
		struct S_AllocClientID
		{
				S_AllocClientID()
				: sData{}, nClientID{}
			{
				sData.nMessageSize = sizeof(S_AllocClientID);
				sData.eMessageID = E_MessageID::E_AllocClientID;
			}
			S_Data sData;
			int nClientID;
		};
		
		struct S_MatchingRegister
		{
			S_MatchingRegister()
				: sData{}, sUDPAddress{}
			{
				sData.nMessageSize = sizeof(S_MatchingRegister);
				sData.eMessageID = E_MessageID::E_MatchingRegister;
			}
			S_Data sData;
			IPaddress sUDPAddress;
		};
			
		struct S_Matching
		{
			S_Matching()
				: sData{}, nMatchingID{}
			{
				sData.nMessageSize = sizeof(S_Matching);
				sData.eMessageID = E_MessageID::E_Matching;
			}
			S_Data sData;
			int nMatchingID;
		};

		struct S_OpponentScore
		{	
			S_OpponentScore()
				: sData{}, nSpeed {}, nScore{}, nLength{}
			{
				sData.nMessageSize = sizeof(S_OpponentScore);
				sData.eMessageID = E_MessageID::E_OpponentScore;
			}
			S_Data sData;
			int nSpeed;
			int nScore;
			int nLength;
		};
		struct S_GameEnd
		{
			S_GameEnd()
				: sData{}, bGameResult{}
			{
				sData.nMessageSize = sizeof(S_GameEnd);
				sData.eMessageID = E_MessageID::E_GameEnd;
			}
			S_Data sData;
			bool bGameResult;
		};

		struct S_ClientEnd
		{
			S_ClientEnd()
				: sData{}, nDummy{}
			{
				sData.nMessageSize = sizeof(S_ClientEnd);
				sData.eMessageID = E_MessageID::E_ClientEnd;
			}
			S_Data sData;
			int nDummy;
		}; 

		struct S_ReturnServer
		{
			S_ReturnServer()
				: sData{}, nDummy{}
			{
				sData.nMessageSize = sizeof(S_ReturnServer);
				sData.eMessageID = E_MessageID::E_ReturnServer;
			}
			S_Data sData;
			int nDummy;
		};
	}
}

class TCPManager
{
public:
	//struct S_Packet
	//{
	//	S_Packet() = default;
	//	TCP::Message::S_Data sData;
	//	void* pPacket;
	//	int nIndex;
	//};
private:
	TCPManager() = default;
	static TCPManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static void RecvData();
	static bool RecvData(const SDLNet_SocketSet& SocketSet, const TCPsocket& pSocket, TCP::Message::S_Data& sData);
	//static bool TCPSendMessage(Network::Host::S_Host& pSrc, void* pMessage, int nLen);

	static bool Open_TCP(Network::Host::S_Host* pSrc, const char* strHost, int nSocketSetSize);
	static bool Close_TCP(Network::Host::S_Host* pSrc);
	static bool AddSocketSet(TCPsocket& pSocekt);
	static bool AcceptTCPSocket(SDLNet_SocketSet& sSocketSet, TCPsocket& pServer, TCPsocket& newClient, IPaddress& newRemoteip);
	static void MakePacket(void* pPacket, const void* sData);
	static bool MakePacket(const TCPsocket& pSocket, void* pDst, const void* pSrc, int nSrcSize);
	static bool SendMessage(TCPsocket& pSocekt, void* pMessage, int nLen);
	
private:
	bool RecvMessage(TCPsocket& pSocekt, void* pMessage, int nLen);
	static void PrintMessageType(void* pMessage, const char* strMessage);
	void CloseSocket(TCPsocket& pSocket);
protected:
	static int GetHeaderSize() { return  sizeof(TCP::Message::S_Data); }
public:
	static void SetTCPPort(Uint16 nTCPPortNum) { m_pInstance->m_nTCPPortNum = nTCPPortNum; }
	static Uint16 GetTCPPort() { return m_pInstance->m_nTCPPortNum; }
	static TCPsocket* GetRecvSocket() { return m_pInstance->m_pRecvSocket; }
	static Uint32 GetTimeOut() { return m_pInstance->m_nNetTimeout; }
	static void SetObject(C_Object* pUDPRecv) { m_pInstance->m_pRecvObject = pUDPRecv; }
	static TCPsocket* GetHostSocket() { m_pInstance->m_pHost->sTCPSocket; }
	static Network::Host::S_Host* GetHost() { return m_pInstance->m_pHost; }
	static void PrintMessageType(TCP::Message::E_MessageID eMessageType, const char* strMessage);
	static void SetServerIP(std::string& strServerIP) { m_pInstance->m_strServerIP = strServerIP; }
	static const char* GetServerIP() { return m_pInstance->m_strServerIP.c_str(); }
private:
	TCP::Message::S_Data m_sHeader;
	Network::Host::S_Host* m_pHost;
	std::set<TCPsocket*> m_setHost;
	SDLNet_SocketSet m_SocketSet;
	TCPsocket* m_pRecvSocket;
	C_Object* m_pRecvObject;
	std::string m_strServerIP;
	Uint32 m_nNetTimeout;
	Uint16 m_nTCPPortNum;
};