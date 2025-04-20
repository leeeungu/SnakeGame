#include "GameScene.h"
#include "OWindow.h"
#include "ASnakeGameState.h"
#include "AMap.h"
#include "AAppleSpawner.h"
#include "ASnake.h"
#include "AClient.h"
#include "AOpponent.h"
#include "GameInstance.h"
#include "ATexture.h"

GameScene::GameScene()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	C_ASnakeGameState* pGame = new C_ASnakeGameState{};
	SetSceneObject(E_GameState, pGame);
	{ 
		C_AOpponent* pOpponent = new C_AOpponent{};
		SetSceneObject(E_Opponent, pOpponent);
		C_AClient* pClient = (C_AClient*)GameInstance::GetObject(GameInstance::E_Client);
		if (pClient)
			pClient->SetEventActor(pOpponent, C_AClient::E_EventActor::E_AOpponent);
		if (pClient)
			pClient->SetEventActor(pGame, C_AClient::E_EventActor::E_AGameState);
		C_AMap* pMap = new C_AMap{};
		SetSceneObject(E_Map, pMap);
		pMap->SetRegisterRender(true);
		pGame->SetEventActor(pMap, C_ASnakeGameState::E_EventActor::E_Map);

		C_AAppleSpawner* pSpawner = new C_AAppleSpawner{};
		SetSceneObject(E_AppleSpawner, pSpawner);
		pSpawner->SetEventActor(pMap, C_AAppleSpawner::E_EventActor::E_Map);
		pGame->SetEventActor(pSpawner, C_ASnakeGameState::E_EventActor::E_Spawner);

		C_ASnake* pSnake = new C_ASnake{};
		SetSceneObject(E_Snake, pSnake);
		pSnake->SetEventActor(pMap, C_ASnake::E_Map);
		pSnake->SetEventActor(pGame, C_ASnake::E_GameState);
		pGame->SetEventActor(pSnake, C_ASnakeGameState::E_Snake);
	}
}

GameScene::~GameScene()
{
}
