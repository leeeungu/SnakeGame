#include "DebugMessageManager.h"
#include "SDL_net.h"
#include <iostream>

void DebugMessageManager::PrintDebugMesasge_Net(const char* strMessage)
{
#ifdef _DEBUG
	printf(strMessage);
	printf(" : ");
	printf(SDLNet_GetError());
	printf("\n");
#endif 
}

void DebugMessageManager::PrintDebugMesasge(const char* strMessage)
{
#ifdef _DEBUG
	printf(strMessage);
	printf("\n");
#endif 
}

void DebugMessageManager::PrintDebugMesasge_Sprite(const char* strFile, const char* strMessage)
{
#ifdef _DEBUG
	printf(strFile);
	printf(" : ");
	printf(strMessage);
	printf("\n");
#endif 
}

void DebugMessageManager::PrintDebugMesasge_FileReader(const char* strFile, const char* strMessage)
{
#ifdef _DEBUG
	printf(strFile);
	printf(" : ");
	printf(strMessage);
	printf("\n");
#endif 
}
