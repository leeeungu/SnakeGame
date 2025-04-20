#include "GameInstance.h"

GameInstance* GameInstance::m_pInstance = nullptr;

void GameInstance::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new GameInstance{};
}

void GameInstance::DestroyInstance()
{
	if (!m_pInstance)
		return;

	for (int i = 0; i < E_EnumMax; i++)
	{
		m_pInstance->DeleteObject(m_pInstance->m_arObjects[i]);
	}

	delete m_pInstance;
	m_pInstance = nullptr;
}

bool GameInstance::AddObject(E_InstanceType eType, C_Object* pObject)
{
	if (!m_pInstance->m_arObjects[eType])
	{
		m_pInstance->m_arObjects[eType] = pObject;
		return true;
	}
	return false;
}

C_Object* GameInstance::GetObject(E_InstanceType eType)
{
	return m_pInstance->m_arObjects[eType];
}

void GameInstance::DeleteObject(C_Object*& pObject)
{
	if (pObject)
	{
		delete pObject;
	}
	pObject = nullptr;
}
