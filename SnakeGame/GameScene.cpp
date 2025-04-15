#include "GameScene.h"
#include "AText.h"
#include "SoundManager.h"
#include "AMap.h"
#include "ASnake.h"
#include "ASnakeGameState.h"
#include "AAppleSpawner.h"
#include "OWindow.h"
#include "TCPManager.h"
#include "OClient.h"
#include "AOpponent.h"

GameScene::GameScene()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	C_OClient* pClient = new C_OClient{};
	SetSceneObject(E_Client, pClient);
	C_ASnakeGameState* pGame = new C_ASnakeGameState{};
	{
		SetSceneObject(E_GameState, pGame);
	}
	{  // text
		using namespace FontData;
		C_AText* pText{};
		pText = new C_AText{ E_FontID::E_cutetat , 100, {250,400 } };
		SetSceneObject(E_UI, pText);
		pText->SetColor({ 255,0,0 });
		pGame->SetEventActor(pText, C_ASnakeGameState::E_UI);

		pText = new C_AText{ E_FontID::E_cutetat , 30, {100,0} };
		SetSceneObject(E_UIScore, pText);
		pGame->SetEventActor(pText, C_ASnakeGameState::E_UIScore);

		pText = new C_AText{ E_FontID::E_cutetat , 30, {500,0} };
		SetSceneObject(E_UIBodySize, pText);
		pGame->SetEventActor(pText, C_ASnakeGameState::E_UILevel);

		pText = new C_AText{ E_FontID::E_cutetat , 30, {300,0} };
		SetSceneObject(E_UILevel, pText);
		pGame->SetEventActor(pText, C_ASnakeGameState::E_UIBodySize);
	}
	{ 
		C_AOpponent* pOpponent = new C_AOpponent{};
		SetSceneObject(E_Map2, pOpponent);
		pClient->SetMap(pOpponent);

		C_AMap* pMap = new C_AMap{};
		SetSceneObject(E_Map, pMap);
		pMap->SetRegisterRender(true);
		C_AAppleSpawner* pSpawner = new C_AAppleSpawner{};
		SetSceneObject(E_AppleSpawner, pSpawner);
		pSpawner->SetEventActor(pMap, C_AAppleSpawner::E_EventActor::E_Map);
		//pGame->SetEventActor(pSpawner, C_ASnakeGameState::E_EventActor::E_Spawner);

		C_ASnake* pSnake = new C_ASnake{};
		SetSceneObject(E_Snake, pSnake);
		pSnake->SetEventActor(pMap, C_ASnake::E_Map);
		pSnake->SetEventActor(pGame, C_ASnake::E_GameState);
		pGame->SetEventActor(pSnake, C_ASnakeGameState::E_Snake);
	}
}

GameScene::~GameScene()
{
	SoundManager::FadeOutMusic(0);
}
