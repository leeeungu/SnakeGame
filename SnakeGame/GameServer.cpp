#include "GameServer.h"
#include "OWindow.h"
#include "GameInstance.h"
#include "AServer.h"
#include "AThreadServer.h"


GameServer::GameServer()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	GameInstance::AddObject(GameInstance::E_Server, new C_AThreadServer{});
}

GameServer::~GameServer()
{
}

