#pragma once

#include "Scene.h"

class GameServer : public Scene
{
	enum E_Actors
	{
		E_None,
		E_EnumMax
	};
public:
	GameServer();
	~GameServer();
};