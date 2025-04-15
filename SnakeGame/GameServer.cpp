#include "GameServer.h"
#include "OWindow.h"
#include "OServer.h"


GameServer::GameServer()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	SetSceneObject(E_Server, new C_OServer{});
}

GameServer::~GameServer()
{
	int a = 0;
}

