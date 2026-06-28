#pragma once

#include "NetworkManager.h"
#include "SDL_net.h"
#include <string>
#include <set>
#include <queue>

class C_Object;

namespace UDP
{
	namespace Message
	{
		enum  E_MessageID
		{
			E_None,
			E_SnakeMap,
			E_EnumMax
		};

		struct S_Data
		{
			S_Data()
				: nClientID{}, nMessageSize {}
			{
				nMessageSize = sizeof(S_Data);
			}
			int nClientID;
			int nMessageSize;
			E_MessageID eMessageID;
		};

		namespace Snake
		{
			struct S_Map
			{
				S_Map() :
					sData{}, nX{}, nY{}, nSpriteIndex{}
				{
					sData.nMessageSize = sizeof(S_Map);
					sData.eMessageID = E_SnakeMap;
				}
				S_Data sData;
				int nX;
				int nY;
				int nSpriteIndex;
			};
		}
	}
}

class UDPManager
{
private:
	UDPManager() = default;
	static UDPManager* m_pInstance;

public:
	static void CreateInstance();
	static void DestroyInstance();

	static bool UDPSend(UDPsocket& pSocket, UDPpacket*& pPacket, int nChannel = -1);
	static bool UDPSend(Network::Host::S_Host& sSrc, void* pMessage, int nLength, int nChannel = -1);
	static void RecvData();
	static bool RecvData(UDPsocket& pSocket, UDPpacket*& pPacket);

	static bool Open_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize);
	static bool ResolveHost(Network::Host::S_Host* sSrc, const char* host);
	static bool Close_UDP(Network::Host::S_Host* pSrc);
	static void PrintMessage(UDP::Message::S_Data& sData, const char* strMessage);
	static bool CheckPacket(const UDP::Message::S_Data& sData);

private:
	bool Init_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize);
	void CloseSocket(UDPsocket& pSocket);
	void FreePacket(UDPpacket*& pPacket);
	static bool CheckPacket(int nClientID, UDP::Message::E_MessageID eMessageID);

public:
	static void SetUDPPort(Uint16 nUDPPortNum) { m_pInstance->m_nUDPPortNum = nUDPPortNum; }
	static Uint16 GetUDPPort() { return m_pInstance->m_nUDPPortNum; }
	static void SetObject(C_Object* pUDPRecv) { m_pInstance->m_pRecvObject = pUDPRecv; }
	static UDPsocket* GetHostSocket() { m_pInstance->m_pHost->sUDPSocket; }
	static Network::Host::S_Host* GetHost() { return m_pInstance->m_pHost; }
	static int GetPacketSize() { return m_pInstance->m_nPacketSize; }
	static void SetServerIP(std::string& strServerIP) { m_pInstance->m_strServerIP = strServerIP; }
	static const char* GetServerIP() { return m_pInstance->m_strServerIP.c_str(); }

private:
	Network::Host::S_Host* m_pHost;
	C_Object* m_pRecvObject;
	int m_nPacketSize;
	std::string m_strServerIP;
	Uint16 m_nUDPPortNum;
};
