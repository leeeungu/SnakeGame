#pragma once

#include "Actor.h"
#include <set>

class ActorManager
{
	friend C_Actor;
private:
	ActorManager() = default;

	static ActorManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();
	
	static void RenderActor();

private:
	void Loop(std::set< C_Actor*>& setActor, void (C_Actor::* Func)());
protected:
	static void RegisterActor(C_Actor* pActor);
	static void UnRegisterActor(C_Actor* pActor);

private:
	std::set< C_Actor*> m_setActorLayer[Actor::E_RenderLayer::E_EnumMax];
};