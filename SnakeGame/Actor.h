#pragma once

#include "Object.h"
#include "SDL.h"
#include "RenderManager.h"

class ActorManager;

__interface I_Actor
{
	void BeginRender();
	void Render();
	void EndRender();
	bool OverlapEvent(class C_Actor* pActor);
};

namespace Actor
{
	enum E_RenderLayer
	{
		E_Actor,
		E_Map,
		E_UI,
		E_EnumMax
	};
}
class C_Actor abstract : public C_Object, public I_Actor
{
	friend ActorManager;
private:
	bool m_bResigerRender;
	int m_nEventActorSize;
protected:
	Actor::E_RenderLayer m_eRenderLayer;
	SDL_Surface* m_pSDLSurface;
	SDL_Rect m_sSourceRect;
	SDL_FRect m_sRenderRect;
	C_Actor** m_arEventActor;
	bool m_bRender;

public:
	C_Actor();
	~C_Actor();



	bool OverlapEventActor(C_Actor* pActor);


	void SetRenderPosition(const SDL_FPoint& sValue);
	void SetRenderRector(const SDL_FRect& sPositionRect);
	void SetRenderSize(const SDL_FPoint& sValue);
	void SetSourcePosition(const SDL_Rect& sPositionRect);



	//Rendering
	Actor::E_RenderLayer GetRenderLayer() { return m_eRenderLayer; }
	void SetRenderLayer(Actor::E_RenderLayer bValue);
	void SetRender(bool bValue) { m_bRender = bValue; }
	bool GetRender() { return m_bRender; }
	bool GetRegisterRender() { return m_bResigerRender; }
	virtual void SetRegisterRender(bool bValue);

	void SetEventActor(C_Actor* pActor, int nIndex);
protected:
	//Rendering
	void RenderActor();
private:
	C_Actor** GetEventActor_Internal(int nIndex);
protected:
	C_Actor* GetEventActor(int nIndex);
	bool RunEvent(int nIndex);
	void CreateEventActorArray(int nEnumMax);
	void DestoryEventActorArray();
	void ReleaseSurface();

	virtual void InitObject() final;
	// I_Object을(를) 통해 상속됨
	virtual void BeginPlay() override {};
	virtual void Update(Uint32 fDeltaTick) override {};
	virtual void EndPlay() override {};
	virtual void Init() override {};

	// I_Actor을(를) 통해 상속됨
	virtual void BeginRender() override {};
	virtual void Render() override;
	virtual void EndRender() override {};
	virtual bool OverlapEvent(C_Actor* pActor) override { return false; };
};