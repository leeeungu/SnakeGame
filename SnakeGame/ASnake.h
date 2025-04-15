#pragma once

#include <list>
#include "Actor.h"

class C_AMap;
class C_ABlock;

class C_ASnake : public C_Actor
{
public:
	enum E_EventActor
	{
		E_Map,
		E_GameState,
		E_EnumMax
	};

private:

	enum E_BodySize
	{
		E_Size = 400
	};
	struct S_Body
	{
		SDL_Point sCoord;
		C_ABlock* pBlock;
		S_Body* pNext;
		S_Body* pPre;
	};
	S_Body arBody[E_BodySize::E_Size];
	int m_nBodyIndex;
	int m_nBodyCount;
	S_Body* m_pHead;
	S_Body* m_pTail;
	S_Body* m_pPreTail;
	SDL_Point m_sCurPos;

	int m_nBodySize;
	SDL_Point m_sDir;
	SDL_Point m_sPreDir;
	C_AMap* m_pMap;

	int m_eApplType;
	bool m_bDead;
	bool m_bDecreases;
	int m_nDecreaseSize; 
public:
	C_ASnake();
	~C_ASnake();

	bool GetDead() { return m_bDead; }
	int GetAppleType();
	int GetBodyCount() { return m_nBodyCount; }
protected:
	void SetDead();
	virtual void Update(Uint32 fDeltaTick) override;
	virtual void HandleEvent() override;
	virtual bool OverlapEvent(C_Actor* pActor) override;
	virtual void Reset() override;
private:
	S_Body* GetBody();
	void PushBody(S_Body* pBody);
	void PopBody(S_Body* pBody);
	void Up();
	void Down();
	void Left();
	void Right();
	void SetSprite(S_Body* pBody);
};