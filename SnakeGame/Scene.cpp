#include "Scene.h"
#include "Actor.h"
#include "ObjectManager.h"
#include "ActorManager.h"
#include "InputHandleManager.h"

Scene::~Scene()
{
	if (!m_arObjects)
		return;
	for (int i = 0; i < m_nSize; i++)
	{
		if (m_arObjects[i])
		{
			delete m_arObjects[i];
			m_arObjects[i] = nullptr;
		}
	}
	delete[] m_arObjects;
	m_arObjects = nullptr;
}

void Scene::Reset()
{
	ObjectManager::ResetObject();
}

void Scene::Init()
{
	Reset();
}

void Scene::HandleEvents()
{
	InputHandleManager::HandleEvent();
}

void Scene::Update(Uint32 fDeltaTick)
{
	ObjectManager::BeingPlayObject();
	ObjectManager::UpdateObject(fDeltaTick);
	ObjectManager::EndPlayObject();
}

void Scene::Render()
{
	RenderManager::StartRenderer();

	if (RenderManager::GetRendering())
		ActorManager::RenderActor();

	RenderManager::EndRenderer();
}

void Scene::CreateActorsArray(int nSize)
{
	if (m_arObjects)
		return;
	m_nSize = nSize;
	m_arObjects = new C_Object * [m_nSize] {};
}

void Scene::SetSceneObject(int nIndex, C_Object* pObject)
{
	if (nIndex  < 0 || m_nSize <= nIndex)
		return;
	m_arObjects[nIndex] = pObject;
}

C_Object* Scene::GetSceneObject(int nIndex)
{
	if (nIndex < 0 || m_nSize >= nIndex)
		return nullptr;
	return m_arObjects[nIndex];
}
