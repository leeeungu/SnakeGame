#pragma once

#include "Scene.h"

class GameScene : public Scene
{
	enum E_Objects
	{
		E_None,
		E_GameState,
		E_Map,
		E_Map2,
		E_Snake,
		E_AppleSpawner,
		E_UI,
		E_UIScore,
		E_UIBodySize,
		E_UILevel,
		E_Client,
		E_EnumMax
	};
public:
	GameScene();
	~GameScene();
};