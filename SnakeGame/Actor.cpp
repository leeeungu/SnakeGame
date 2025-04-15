#include "Actor.h"
#include "ActorManager.h"

C_Actor::C_Actor() :
	C_Object{}, m_eRenderLayer{}, m_pSDLSurface {}, m_sSourceRect{}, m_sRenderRect{}
{
	m_bActive = true;
	m_bResigerRender = true;
	m_bRender = true;
	ActorManager::RegisterActor(this);
}

C_Actor::~C_Actor()
{
	ActorManager::UnRegisterActor(this);
	DestoryEventActorArray();
}

void C_Actor::RenderActor()
{
	if (!m_bRender)
		return;
	BeginRender();
	Render();
	EndRender();
}

C_Actor** C_Actor::GetEventActor_Internal(int nIndex)
{
	C_Actor** ppActor{};
	if (nIndex >= 0 && nIndex < m_nEventActorSize && m_arEventActor)
	{
		ppActor = &m_arEventActor[nIndex];
	}
	return ppActor;
}

bool C_Actor::OverlapEventActor(C_Actor* pActor)
{
	return OverlapEvent(pActor);
}

void C_Actor::SetRenderPosition(const SDL_FPoint& sValue)
{
	m_sRenderRect.x = sValue.x;
	m_sRenderRect.y = sValue.y;
}

void C_Actor::SetRenderRector(const SDL_FRect& sPositionRect)
{
	m_sRenderRect = sPositionRect;
}

void C_Actor::SetRenderSize(const SDL_FPoint& sValue)
{
	m_sRenderRect.w = sValue.x;
	m_sRenderRect.h = sValue.y;
}

void C_Actor::SetSourcePosition(const SDL_Rect& sPositionRect)
{
	m_sSourceRect = sPositionRect;
}

void C_Actor::SetRenderLayer(Actor::E_RenderLayer bValue)
{
	if (m_eRenderLayer == bValue)
		return;	
	ActorManager::UnRegisterActor(this);
	m_eRenderLayer = bValue;
	ActorManager::RegisterActor(this);
}

void C_Actor::SetRegisterRender(bool bValue)
{
	if (m_bResigerRender == bValue)
		return;
	ActorManager::UnRegisterActor(this);
	m_bResigerRender = bValue;
	ActorManager::RegisterActor(this);
}

void C_Actor::SetEventActor(C_Actor* pActor, int nIndex)
{
	C_Actor** ppActor = GetEventActor_Internal(nIndex);
	if (ppActor)
		*ppActor = pActor;
}

C_Actor* C_Actor::GetEventActor(int nIndex)
{
	C_Actor** ppActor = GetEventActor_Internal(nIndex);
	C_Actor* pActor{};
	if (ppActor)
		pActor = *ppActor;
	return pActor;
}

bool C_Actor::RunEvent(int nIndex)
{
	C_Actor* pActor = GetEventActor(nIndex);
	if (pActor)
		pActor->OverlapEventActor(this);
	return pActor != nullptr;
}

void C_Actor::CreateEventActorArray(int nEnumMax)
{
	if (m_nEventActorSize != 0 || m_arEventActor || nEnumMax <= 0)
		return;
	m_nEventActorSize =  nEnumMax;
	m_arEventActor = new C_Actor * [m_nEventActorSize] {};
}

void C_Actor::DestoryEventActorArray()
{
	if (!m_arEventActor)
		return;
	delete[] m_arEventActor;
	m_arEventActor = nullptr;
}

void C_Actor::ReleaseSurface()
{
	if (m_pSDLSurface)
		SDL_FreeSurface(m_pSDLSurface);
	m_pSDLSurface = nullptr;
}

void C_Actor::InitObject()
{
	Init();
}

void C_Actor::Render()
{
	SDL_Texture* pTexture = RenderManager::GetTexture(m_pSDLSurface);
	if (pTexture )
		RenderManager::RenderTexture(pTexture, &m_sSourceRect, &m_sRenderRect);
}