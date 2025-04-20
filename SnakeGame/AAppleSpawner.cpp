#include "AAppleSpawner.h"
#include "AMap.h"
#include "SpriteManager.h"

C_AAppleSpawner::C_AAppleSpawner()
{
	CreateEventActorArray(E_EventActor::E_EnumMax);
    m_bUpdate = false;
    m_bRender = false;
    SetRegisterUpdate(true);
    SetRegisterRender(false);
    m_fSpawnTime = 500;
}

void C_AAppleSpawner::DelegateEventActor(int nIndex)
{
    m_pMap = (C_AMap*)GetEventActor(C_AAppleSpawner::E_EventActor::E_Map);
}

void C_AAppleSpawner::Update(Uint32 fDeltaTick)
{
    m_fCurrentTime += fDeltaTick;
    if (m_fCurrentTime > m_fSpawnTime)
    {
        m_fCurrentTime = 0;
        SpawnApple();
    }
}

bool C_AAppleSpawner::OverlapEvent(C_Actor* pActor)
{
    m_bUpdate = true;
    return false;
}

void C_AAppleSpawner::SpawnApple()
{
    if (!m_pMap )
        return;

    using namespace Map;
    using namespace Sprite::Block;
    int count = 0, indexy = 0, indexx = 0;
    int AppleKind = 0;
    while (count++ < 5) {
        indexy = rand() % E_MapData::E_MapSize;
        indexx = rand() % E_MapData::E_MapSize;
        AppleKind = rand() % 100;
        C_ABlock* pActor = m_pMap->GetBlock({ indexx, indexy });
        if (pActor)
        {
            E_SpriteID eBlock = (E_SpriteID)pActor->GetSpriteIndex();
            if (eBlock == E_SpriteID::E_None)
            {
                eBlock = E_RedApple;
                if (AppleKind < 10)
                    eBlock = E_BlueApple;
                else if (AppleKind < 20)
                    eBlock = E_GoldApple;
                count = 5;
            }
            pActor->SetSpriteIndex(eBlock);
        }
    }
}