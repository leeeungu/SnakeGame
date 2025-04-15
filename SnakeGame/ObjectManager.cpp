#include "ObjectManager.h"

ObjectManager* ObjectManager::m_pInstance = nullptr;

void ObjectManager::CreateInstance()
{
    if (m_pInstance)
        return;
    m_pInstance = new ObjectManager{};
}

void ObjectManager::DestroyInstance()
{
    if (!m_pInstance)
        return;
    m_pInstance->m_setObject.clear();
    delete m_pInstance;
    m_pInstance = nullptr;
}

void ObjectManager::RegisterObject(C_Object* pObject)
{
    m_pInstance->m_bBegin = true;
    if (pObject->GetActiveBegin())
        m_pInstance->m_queBegin.push(pObject);
    m_pInstance->m_setObject.insert(pObject);
    Object::E_UpdateLayer Layer = pObject->GetUpdateLayer();
    if (pObject->GetRegisterUpdate())
        m_pInstance->m_setObjectLayer[Layer].insert(pObject);
}

void ObjectManager::UnRegisterObject(C_Object* pObject)
{
    m_pInstance->m_setObject.erase(pObject);
    pObject->EndPlayObject();
    Object::E_UpdateLayer Layer = pObject->GetUpdateLayer();
    if (pObject->GetRegisterUpdate())
        m_pInstance->m_setObjectLayer[Layer].erase(pObject);
}

void ObjectManager::BeingPlayObject()
{
    if (m_pInstance->m_bBegin)
    {
        m_pInstance->Loop(m_pInstance->m_queBegin, &C_Object::BeginPlayObject);
        m_pInstance->m_bBegin = false;
    }
}

void ObjectManager::UpdateObject(Uint32 fDeltaTick)
{
    using namespace Object;
    E_UpdateLayer Layer{};
    while (Layer !=  E_UpdateLayer::E_EnumMax)
    {
        m_pInstance-> Loop(m_pInstance->m_setObjectLayer[Layer], &C_Object::UpdateObject, fDeltaTick);
        Layer = (E_UpdateLayer)(Layer + 1);
    }
}

void ObjectManager::EndPlayObject()
{
    if (m_pInstance->m_bEnd)
    {
        m_pInstance-> Loop(m_pInstance->m_queEnd, &C_Object::EndPlayObject);
        m_pInstance->m_bEnd = false;
    }
}

void ObjectManager::ResetObject()
{
    m_pInstance->Loop(m_pInstance->m_setObject, &C_Object::ResetObject);
}

void ObjectManager::Loop(std::set< C_Object*>& setObject, void(C_Object::* Func)())
{
    if (!Func)
        return;
    std::set< C_Object*>::iterator pIter = setObject.begin();
    while (pIter != setObject.end())
    {
        ((*pIter)->*Func)();
        pIter++;
    }
}

void ObjectManager::Loop(std::set<C_Object*>& setObject, void(C_Object::* Func)(Uint32), Uint32 fDeltaTick)
{
    if (!Func)
        return;
    std::set< C_Object*>::iterator pIter = setObject.begin();
    while (pIter != setObject.end())
    {
        ((*pIter)->*Func)(fDeltaTick);
        pIter++;
    }
}

void ObjectManager::Loop(std::queue<C_Object*>& queObject, void(C_Object::* Func)())
{
    if (!Func)
        return;
    C_Object* pObject{};
    while (!queObject.empty())
    {
        pObject = queObject.front();
        (pObject->*Func)();
        queObject.pop();
    }
}
