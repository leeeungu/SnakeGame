#include "AMap.h"
#include "SpriteManager.h"
#include "UDPManager.h"
#include "ActorManager.h"
#include "AClient.h"

C_AMap::C_AMap()
{
    using namespace Map;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            m_arBlockActor[i][j] = new C_ABlock{};
            C_ABlock* pActor = m_arBlockActor[i][j];
            pActor->SetCoord({ i,j });
            pActor->SetRenderLayer(Actor::E_RenderLayer::E_Map);
        }
    }
    SetMapPosition({0,30.0f});
    m_bUpdate = false;
    m_bRender = false;
    SetRegisterUpdate(false);
    SetRegisterRender(false);
}

C_AMap::~C_AMap()
{
    using namespace Map;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            delete  m_arBlockActor[i][j];
            m_arBlockActor[i][j] = nullptr;
        }
    }
}

C_ABlock* C_AMap::GetBlock(const SDL_Point& sPoint)
{
    C_ABlock* pActor{};
    if (sPoint.x >= 0 && sPoint.x < Map::E_MapData::E_MapSize && sPoint.y >= 0 && sPoint.y < Map::E_MapData::E_MapSize)
        pActor = m_arBlockActor[sPoint.x][sPoint.y];
    return pActor;
}

void C_AMap::SetMapPosition(const SDL_FPoint& sPoint)
{
    using namespace Map;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            C_ABlock* pActor = m_arBlockActor[i][j];
            pActor->SetRenderRector({ (float)(E_MapData::E_RectSize * i) + sPoint.x,  sPoint.y + (float)(E_MapData::E_RectSize * j), E_MapData::E_RectSize, E_MapData::E_RectSize });
        }
    }
}

void C_AMap::SetRegisterRender(bool bValue)
{
    if (GetRegisterRender() == bValue)
        return;
    C_Actor::SetRegisterRender(bValue);
    using namespace Map;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            C_ABlock* pActor = m_arBlockActor[i][j];
            pActor->SetRegisterRender(bValue);
        }
    }
}

void C_AMap::RecvUDPMapMessage(void* pMessage, int nMessageLength)
{
    RecvUDPMessage(pMessage, nMessageLength);
}

void C_AMap::SendAll()
{
    using namespace Map;
    using namespace Sprite::Block;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            C_ABlock* pActor = m_arBlockActor[i][j];
            if (pActor)
                pActor->SendMessage();
        }
    }
}

void C_AMap::ResetMap()
{
    using namespace Map;
    using namespace Sprite::Block;
    for (int i = 0; i < E_MapData::E_MapSize; i++)
    {
        for (int j = 0; j < E_MapData::E_MapSize; j++)
        {
            C_ABlock* pActor = m_arBlockActor[i][j];
            if (pActor)
                pActor->Reset();
        }
    }
}

void C_AMap::Reset()
{
    ResetMap();
}

bool C_AMap::RecvUDPMessage(void* pMessage, int nMessageLength)
{
    using namespace UDP::Message;
    Snake::S_Map* sMessage = (Snake::S_Map*)pMessage;
    if (!sMessage)
        return false;
    C_ABlock* pBlock = GetBlock({ sMessage->nX, sMessage->nY });
    pBlock->RecvObject(Network::Protocol::E_UDP,0, pMessage, nMessageLength);
    return true;
}
