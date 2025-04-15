#pragma once

#include "NetworkManager.h"
#include "SDL_net.h"
#include <set>

namespace UDP
{
	namespace Message
	{
		struct S_Data
		{
			S_Data()
				: nClientID{}, nMessageSize {}
			{
				nMessageSize = sizeof(S_Data);
			}
			int nClientID;
			int nMessageSize;
		};

		namespace Snake
		{
			struct S_Map
			{
				S_Map() :
					sData{}, nX{}, nY{}, nSpriteIndex{}
				{
					sData.nMessageSize = sizeof(S_Map);
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
	
	static bool UDPSend(Network::Host::S_Host& sSrc, void* pMessage, int nLength, int nChannel = -1);
	static void RecvData();

	static bool Open_Server(Network::Server::S_Server* sSrc, int nPacketSize = 512);
	static bool Open_Client(Network::Client::S_Client* sSrc, int nPacketSize = 512);
	static bool Close_UDP(Network::Host::S_Host* pSrc);

private:
	bool Init_UDP(Network::Host::S_Host* sSrc, Uint16 nPortNum, int nPacketSize);
	void CloseSocket(UDPsocket& pSocket);
	void FreePacket(UDPpacket*& pPacket);

private:
	std::set< Network::Host::S_Host*> m_setHost;
};