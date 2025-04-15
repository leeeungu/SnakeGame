#pragma once

#include "Scene.h"

class GameStartScene : public Scene
{
	enum E_Objects
	{
		E_None,
		E_Title,
		E_Start,
		E_EnumMax
	};
public:
	GameStartScene();
	~GameStartScene();
};