#pragma once

#include "Actor.h"

class C_ATestPlayer : public C_Actor
{
	enum  E_PlayerState
	{
		E_Title, 
		E_Game,
		E_EnumMax
	};

	enum  E_PlayerDirection
	{
		E_Right,
		E_Down,
		E_Left,
		E_Up,
		E_Max
	};
public:
	C_ATestPlayer();

	void ReturnTitle();
protected:
	virtual void Update(Uint32 fDeltaTick) override ;
	
	void Update_Title(Uint32 fDeltaTick);
	void Update_Game(Uint32 fDeltaTick);

protected:
	void (C_ATestPlayer::* pFunc[E_EnumMax])(Uint32 fDeltaTick);
	E_PlayerState m_eState;
	Uint32 m_fTime;
	bool m_bMatching;
	E_PlayerDirection m_eDirection;
	SDL_KeyCode m_arDir[E_Max];
};