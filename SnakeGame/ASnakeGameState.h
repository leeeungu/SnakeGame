#pragma once

#include "SDL.h"
#include "Actor.h"

namespace GameState
{
	enum E_Level
	{
		E_None,
		E_Level1,
		E_Level2,
		E_Level3,
		E_Level4,
		E_Level5,
		E_Level6,
		E_Level7,
		E_EnumMax
	};
}

class C_ASnakeGameState : public C_Actor
{
public:
	enum E_EventActor
	{
		E_UI,
		E_UIScore,
		E_UIBodySize,
		E_UILevel,
		E_Snake,
		E_EnumMax
	};

public:
	C_ASnakeGameState();
	~C_ASnakeGameState();

	GameState::E_Level GetLevel() { return m_nSpeed; }

	void SetScore(int nValue);
	int GetScore() { return m_nScore; }

protected:
	void SendState();
	void SetLevelCount(int nCount);
	void SetDead();
	void SetUIBodyCount(int nCount);
	virtual void HandleEvent() override;
	virtual void Reset() override;
	virtual bool OverlapEvent(C_Actor* pActor) override;

private:
	GameState::E_Level m_nSpeed;
	Uint32 m_arTicks[GameState::E_Level::E_EnumMax];
	int m_nScore;
	int m_nLevelCount;
	int m_nLength;
	bool m_bCanReset;
};