#include "ActorManager.h"

ActorManager* ActorManager::m_pInstance = nullptr;

void ActorManager::CreateInstance()
{
    if (m_pInstance)
        return;
    m_pInstance = new ActorManager{};
}

void ActorManager::DestroyInstance()
{
    if (!m_pInstance)
        return;
    Actor::E_RenderLayer Layer{};
    while (Layer != Actor::E_RenderLayer::E_EnumMax)
    {
        m_pInstance->m_setActorLayer[Layer].clear();
        Layer = (Actor::E_RenderLayer)(Layer + 1);
    }
   
    delete m_pInstance;
    m_pInstance = nullptr;
}

void ActorManager::RegisterActor(C_Actor* pActor)
{
    if (!pActor->GetRegisterRender())
        return;
    Actor::E_RenderLayer Layer = pActor->GetRenderLayer();
    m_pInstance->m_setActorLayer[Layer].insert(pActor);
}

void ActorManager::UnRegisterActor(C_Actor* pActor)
{
    if (!pActor->GetRegisterRender())
        return;
    Actor::E_RenderLayer Layer = pActor->GetRenderLayer();
    m_pInstance->m_setActorLayer[Layer].erase(pActor);
}

void ActorManager::RenderActor()
{
    Actor::E_RenderLayer Layer{};
    while (Layer != Actor::E_RenderLayer::E_EnumMax)
    {
        m_pInstance->Loop(m_pInstance->m_setActorLayer[Layer], &C_Actor::RenderActor);
        Layer = (Actor::E_RenderLayer)(Layer + 1);
    }
}
void ActorManager::Loop(std::set<C_Actor*>& setActor, void(C_Actor::* Func)())
{
    if (!Func)
        return;
    std::set< C_Actor*>::iterator pIter = setActor.begin();
    while (pIter != setActor.end())
    {
        ((*pIter)->*Func)();
        pIter++;
    }
}
