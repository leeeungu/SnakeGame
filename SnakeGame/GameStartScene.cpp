#include "GameStartScene.h"
#include "ATexture.h"
#include "OWindow.h"

GameStartScene::GameStartScene()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});
	C_ATexture* pBackGround = new C_ATexture{};
	SetSceneObject(E_Title, pBackGround);
	pBackGround->Init(Sprite::E_SpriteType::E_Background, 1, {0,0,1700,830});
}

GameStartScene::~GameStartScene()
{
}
