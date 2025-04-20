#pragma once


#include "Actor.h"

class C_AMap;


class C_AAppleSpawner : public C_Actor
{
public:
	enum E_EventActor
	{
		E_Map,
		E_EnumMax
	};
public:
	C_AAppleSpawner();

protected:
	virtual void DelegateEventActor(int nIndex) override;
	virtual void Update(Uint32 fDeltaTick) override;
	virtual bool OverlapEvent(C_Actor* pActor) override;
private:
	void SpawnApple();

private:
	C_AMap* m_pMap;
	Uint32 m_fCurrentTime;
	Uint32 m_fSpawnTime;
};