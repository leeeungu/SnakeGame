#pragma once

#include "Scene.h"

class GameStartScene : public Scene
{
	enum E_Objects
	{
		E_None,
		E_BackGround,
		E_Title,
		E_Spinner,
		E_Text,
		E_Client,
		E_TitleState,
		E_EnumMax
	};
public:
	GameStartScene();
	~GameStartScene();
};