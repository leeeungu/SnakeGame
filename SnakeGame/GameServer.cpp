#include "GameServer.h"
#include "OWindow.h"
#include "GameInstance.h"
#include "AServer.h"


GameServer::GameServer()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	GameInstance::AddObject(GameInstance::E_Server, new C_AServer{});
}

GameServer::~GameServer()
{
}

