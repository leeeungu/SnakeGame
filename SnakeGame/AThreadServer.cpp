#include "AThreadServer.h"
#include "UDPManager.h"
#include "FrameWork.h"
#include "AMatchingThread.h"
#include "MultiThreadManager.h"
#include <string>

C_AThreadServer::C_AThreadServer()
{
    TCPManager::Open_TCP(&m_sServer, NULL, m_sServer.nClientSize + 1);
    m_ServerSocketSet = SDLNet_AllocSocketSet(m_sServer.nClientSize + 1);
    UDPManager::Open_UDP(&m_sServer, UDPManager::GetUDPPort(), UDPManager::GetPacketSize());
    {
        using namespace Network::Client;
        m_sClients = new S_Client[m_sServer.nClientSize]{};
     

        for (int i = 0; i < m_sServer.nClientSize; i++)
        {
            m_sClients[i].nClientID = i;
            PushEmpthyClient(i);
        }
        int nSize = E_ThreadCount::E_Size;
        m_arThread = new C_AMatchingThread*[nSize]{};
        for (int i = 0; i < nSize; i++)
        {
            m_arThread[i] = new C_AMatchingThread{};
        }
        m_arClientThread = new C_AMatchingThread * [m_sServer.nClientSize];
        m_sThreadIndex = new int[m_sServer.nClientSize] {};

        m_arUDPPacket = new UDPpacket * [10000] {};
        for (int i = 0; i < 10000; i++)
        {
            m_arUDPPacket[i] = SDLNet_AllocPacket(UDPManager::GetPacketSize());
            m_queUDPPacket.push(m_arUDPPacket[i]);
        }
    }
    using namespace TCP::Message;
    pTCPRecv[E_ClientEnd] = &C_AThreadServer::TCPRecv_ClientEnd;
    pTCPRecv[E_MatchingRegister] = &C_AThreadServer::TCPRecv_MatchingRegister;
    //pTCPRecv[E_GameEnd] = &C_AThreadServer::TCPRecv_GameEnd;
    //pTCPRecv[E_OpponentScore] = &C_AThreadServer::TCPRecv_OpponentScore;


    SDLNet_TCP_AddSocket(m_ServerSocketSet, m_sServer.sTCPSocket);
    SetRegisterUpdate(true);
    SetUpdate(true);
    MultiThreadManager::CreateThread(&C_AThreadServer::UDPRecv, this);
}

C_AThreadServer::~C_AThreadServer()
{
    for (int i = 0; i < m_sServer.nClientSize && m_sClients; i++)
    {
        SendClientEnd(m_sClients[i]);
    }

    for (int i = 0; i < 10000; i++)
    {
        if (m_arUDPPacket[i])
            SDLNet_FreePacket(m_arUDPPacket[i]);
        m_arUDPPacket[i] = nullptr;
    }
    if (m_arUDPPacket)
        delete[]   m_arUDPPacket;
    m_arUDPPacket = nullptr;

    if (m_sThreadIndex)
        delete[] m_sThreadIndex;
    m_sThreadIndex = nullptr;
    if (m_arClientThread)
        delete[] m_arClientThread;
    m_arClientThread = nullptr;

    int nSize = E_ThreadCount::E_Size;
    for (int i = 0; i < nSize; i++)
    {
        if (m_arThread[i])
        {
            m_arThread[i]->SetServeEnd();
        }
    }
    for (int i = 0; i < nSize; i++)
    {
        if (m_arThread[i] && !m_arThread[i]->GetRunning())
        {
            delete   m_arThread[i];
        }
        m_arThread[i] = nullptr;
    }
    if (m_arThread)
        delete[] m_arThread;
    m_arThread = nullptr;

    for (int i = 0; i < m_sServer.nClientSize && m_sClients; i++)
    {
        SendClientEnd(m_sClients[i]);
        CloseClient(m_sClients[i]);
    }
    if (m_sClients)
        delete[] m_sClients;
    m_sClients = nullptr;
    UDPManager::Close_UDP(&m_sServer);

    if (m_ServerSocketSet)
        SDLNet_FreeSocketSet(m_ServerSocketSet);
    m_ServerSocketSet = nullptr;
    TCPManager::Close_TCP(&m_sServer);
}

void C_AThreadServer::SendPacket(std::queue<UDPpacket*>& quePacket)
{
  m_UDPMutex.lock();
  while (!quePacket.empty())
  {
      UDPManager::UDPSend(m_sServer.sUDPSocket, quePacket.front());
      quePacket.pop();
  }
  m_UDPMutex.unlock();
}

void C_AThreadServer::EmpthyPacket(std::queue<UDPpacket*>& quePacket)
{
    m_UDPMutex.lock();
    while (!quePacket.empty())
    {
        m_queUDPPacket.push(quePacket.front());
        quePacket.pop();
    }
    m_UDPMutex.unlock();
}

Network::Client::S_Client* C_AThreadServer::GetClient(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_sServer.nClientSize)
        return &m_sDummy;
    return &m_sClients[nIndex];
}

Network::Client::S_Client* C_AThreadServer::GetEmpthyClient()
{
    if (m_queEmpthyClient.empty())
        return &m_sDummy;
    return GetClient(m_queEmpthyClient.front());
}

void C_AThreadServer::PushEmpthyClient(int nIndex)
{
    m_queEmpthyClient.push(nIndex);
}

void C_AThreadServer::PopEmpthyClient()
{
    if (m_queEmpthyClient.empty())
        return;
    m_queEmpthyClient.pop();
}

Network::Client::S_Client* C_AThreadServer::GetMatchingClient()
{
    Network::Client::S_Client* pClient  = &m_sDummy;
    if (!m_queMatching.empty())
    {
        pClient = GetClient(m_queMatching.front());
        if (!pClient->sTCPSocket)
            pClient = &m_sDummy;
    }
    return pClient;
}

void C_AThreadServer::PushMacthcingClient(int nIndex)
{
    m_queMatching.push(nIndex);
}

void C_AThreadServer::PopEMacthcingClient()
{
    if (m_queMatching.empty())
        return;
    m_queMatching.pop();
}

void C_AThreadServer::CloseClient(Network::Client::S_Client& pClient)
{
    if (!pClient.sTCPSocket)
        return;
    SDLNet_TCP_DelSocket(m_ServerSocketSet, pClient.sTCPSocket);
    SDLNet_TCP_Close(pClient.sTCPSocket);
    pClient.sTCPSocket = nullptr;
}
    
void C_AThreadServer::SendClientEnd(Network::Client::S_Client& pClient)
{
    if (!pClient.sTCPSocket)
        return;
    using namespace TCP::Message;
    S_AcceptEnd sSend{};
    sSend.sData.nClientID = pClient.nClientID;
    TCPManager::SendMessage(pClient.sTCPSocket, &sSend, sSend.sData.nMessageSize);
}

void C_AThreadServer::Matching()
{
    if (m_queMatching.size() >= 2)
    {
        if (m_nReadyThread < 0 || m_nReadyThread >= E_ThreadCount::E_Size)
        {
            m_nReadyThread = 0;
            while (m_nReadyThread < E_ThreadCount::E_Size && m_arThread[m_nReadyThread]->GetRunning())
            {
                m_nReadyThread++;
            }
        }
        if (m_nReadyThread < 0 || m_nReadyThread >= E_ThreadCount::E_Size || m_arThread[m_nReadyThread]->GetRunning())
            return;
        C_AMatchingThread* m_pReadyThread = m_arThread[m_nReadyThread];


        Network::Client::S_Client* pSrc = GetMatchingClient();
        PopEMacthcingClient();
        Network::Client::S_Client* pDst = GetMatchingClient();
        PopEMacthcingClient();
        if (pSrc != &m_sDummy && pDst != &m_sDummy)
        {
            m_arClientThread[pSrc->nClientID] = m_pReadyThread;
            m_arClientThread[pDst->nClientID] = m_pReadyThread;

            m_sThreadIndex[pSrc->nClientID] = 0;
            m_sThreadIndex[pDst->nClientID] = 1;
            SDLNet_TCP_DelSocket(m_ServerSocketSet, pDst->sTCPSocket);
            SDLNet_TCP_DelSocket(m_ServerSocketSet, pSrc->sTCPSocket);
            m_setClient.erase(pSrc->nClientID);
            m_setClient.erase(pDst->nClientID);
            m_pReadyThread->Init(this, &m_sServer, pSrc, pDst);

            m_nReadyThread = -1;
        }
        else
        {
            if (pSrc != &m_sDummy)
                PushMacthcingClient(pSrc->nClientID);
            if (pDst != &m_sDummy)
                PushMacthcingClient(pDst->nClientID);
        }
    }
}

void C_AThreadServer::Update(Uint32 fDeltaTick)
{
    TCPRecv_Accept(&fDeltaTick);
    std::set<int>::iterator pCurrnet{};
    std::set<int>::iterator pNext = m_setClient.begin();
    while (pNext != m_setClient.end())
    {
        pCurrnet = pNext;
        pNext++;
        Network::Client::S_Client* pClient = GetClient(*pCurrnet);
        if (pClient != &m_sDummy)
        {
            using namespace TCP::Message;
            S_Data sData{};
            if (TCPManager::RecvData(m_ServerSocketSet, pClient->sTCPSocket, sData)  && *pCurrnet == sData.nClientID &&
                E_MessageID::E_None < sData.eMessageID && sData.eMessageID < E_MessageID::E_EnumMax  &&
                pTCPRecv[sData.eMessageID] != nullptr)
            {
                TCPManager::PrintMessageType(sData.eMessageID, "Recv : ");
                (this->*pTCPRecv[sData.eMessageID])(&sData.nClientID);
            }
        }
    }
    Matching();
}

int C_AThreadServer::UDPRecv(void* pThreadServer)
{
    C_AThreadServer* pAThreadServer = (C_AThreadServer*)pThreadServer;
    if (!pThreadServer)
        return -1;
    using namespace UDP::Message;
    UDPpacket* pPacket{};

    while (FrameWork::GetRunning())
    {
        if (pPacket && UDPManager::RecvData(pAThreadServer->m_sServer.sUDPSocket, pPacket))
        {
            S_Data sRecv{};
            memcpy(&sRecv, pPacket->data, sRecv.nMessageSize);
            if (0<= sRecv.nClientID  && sRecv.nClientID < pAThreadServer->m_sServer.nClientSize &&
                sRecv.nMessageSize == pPacket->len && UDPManager::CheckPacket(sRecv) && pAThreadServer->m_arClientThread[sRecv.nClientID])
            {
                if (!pAThreadServer->m_arClientThread[sRecv.nClientID]->GetRunning())
                {
                    pAThreadServer->PushEmpthyClient(sRecv.nClientID);
                    pAThreadServer->m_arClientThread[sRecv.nClientID] = nullptr;
                }
                else if (pAThreadServer->m_arClientThread[sRecv.nClientID]->RecvPacket(pPacket, pAThreadServer->m_sThreadIndex[sRecv.nClientID]))
                {
                    //UDPManager::PrintMessage(sRecv, (std::to_string(sRecv.nClientID) + " : ").c_str());
                    pPacket = nullptr;
                }
            }
           
        } 
        if (!pPacket)
        {
            pAThreadServer->m_UDPMutex.lock();
            if (!pAThreadServer->m_queUDPPacket.empty())
            {
                pPacket = pAThreadServer->m_queUDPPacket.front();
                pAThreadServer->m_queUDPPacket.pop();
            }
            pAThreadServer->m_UDPMutex.unlock();
        }
    }
    return 0;
}

int C_AThreadServer::TCPRecv_Accept(void* pMessage)
{
    using namespace TCP::Message;
    Network::Client::S_Client* pClient = GetEmpthyClient();
    if (!TCPManager::AcceptTCPSocket(m_ServerSocketSet, m_sServer.sTCPSocket, pClient->sTCPSocket, pClient->sTCPAddress))
        return 0;

    if (pClient == &m_sDummy)
    {
        SendClientEnd(m_sDummy);
        return 0;
    }
    PopEmpthyClient();
    SDLNet_TCP_AddSocket(m_ServerSocketSet, pClient->sTCPSocket);
    S_AllocClientID sSend{};
    sSend.sData.nClientID = pClient->nClientID;
    sSend.nClientID = pClient->nClientID;
    m_setClient.insert(pClient->nClientID);
    TCPManager::SendMessage(pClient->sTCPSocket, &sSend, sSend.sData.nMessageSize);
    return 0;
}

int C_AThreadServer::TCPRecv_ClientEnd(void* pMessage)
{
    using namespace TCP::Message;   
    int nIndex = *(int*)pMessage;
    S_ClientEnd sRecv{};
    Network::Client::S_Client* pClient = GetClient(nIndex);
    if (TCPManager::MakePacket(pClient->sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
    {
        m_setClient.erase(nIndex);
        CloseClient(*pClient);
        PushEmpthyClient(nIndex);
    }
    return 0;
}

int C_AThreadServer::TCPRecv_MatchingRegister(void* pMessage)
{
    using namespace TCP::Message;
    int nIndex = *(int*)pMessage;
    S_MatchingRegister sRecv{};
    Network::Client::S_Client* pClient = GetClient(nIndex);
    if (TCPManager::MakePacket(pClient->sTCPSocket, &sRecv, &sRecv, sRecv.sData.nMessageSize))
    {
        PushMacthcingClient(nIndex);
    }
    return 0;
}
