#pragma once

#include "Scene.h"

class GameScene : public Scene
{
	enum E_Objects
	{
		E_None,
		E_GameState,
		E_Map,
		E_Opponent,
		E_Snake,
		E_AppleSpawner,
		E_EnumMax
	};
public:
	GameScene();
	~GameScene();
};