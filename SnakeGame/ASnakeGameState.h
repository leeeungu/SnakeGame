#pragma once

#include "SDL.h"
#include "Actor.h"
#include "SpriteManager.h"

class C_AMap;
class C_AText;
class C_ATexture;
class C_ASnake;
class C_AAppleSpawner;

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

	namespace SnakeUI
	{
		enum  E_Text
		{
			E_Score,
			E_Speed,
			E_Length,
			E_EnumMax
		};
	}
}

class C_ASnakeGameState : public C_Actor
{
public:
	enum E_EventActor
	{
		E_Map,
		E_Snake,
		E_Spawner,
		E_EnumMax
	};

public:
	C_ASnakeGameState();
	~C_ASnakeGameState();

	GameState::E_Level GetLevel() { return m_nSpeed; }

	void SetScore(int nValue);
	int GetScore() { return m_nScore; }

	void GameEnd();
protected:
	virtual void DelegateEventActor(int nIndex) override;
	virtual void Update(Uint32 fDeltaTick) override;
	void SendState();
	void SetLevelCount(int nCount);
	void SetDead();
	void SetUIBodyCount(int nCount);
	virtual void HandleEvent() override;
	virtual void Reset() override;
	virtual bool OverlapEvent(C_Actor* pActor) override;
private:
	Uint32 m_nCount;
	int m_nIndex;
	Sprite::E_SpriteType m_eTexture[4];
	GameState::E_Level m_nSpeed;
	Uint32 m_arTicks[GameState::E_Level::E_EnumMax];

	C_AMap* m_pMap;
	C_AText* m_pText[GameState::SnakeUI::E_EnumMax];
	C_ATexture* m_pGameUI;
	C_ASnake* m_pSnake;
	C_AAppleSpawner* m_pSpawner;
	int m_nScore;
	int m_nLevelCount;
	int m_nLength;
	bool m_bDead;
};