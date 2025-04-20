#include "GameStartScene.h"
#include "ATexture.h"
#include "OWindow.h"
#include "SoundManager.h"
#include "AClient.h"
#include "ASnakeTitleState.h"
#include "GameInstance.h"

GameStartScene::GameStartScene()
{
	CreateActorsArray(E_EnumMax);
	SetSceneObject(E_None, new C_OWindow{});

	if (!GameInstance::GetObject(GameInstance::E_Client))
	{
		C_AClient* pClient = new C_AClient{};
		GameInstance::AddObject(GameInstance::E_Client, pClient);
	}

	C_ASnakeTitleState* pState = new C_ASnakeTitleState{};
	SetSceneObject(E_TitleState, pState);

	C_ATexture* pTexture = new C_ATexture{};
	SetSceneObject(E_BackGround, pTexture);
	pTexture->Init(Sprite::E_SpriteType::E_Background, 1, {0,0,1700,830});
	pTexture->SetRenderLayer(Actor::E_Map);

	pTexture = new C_ATexture{};
	SetSceneObject(E_Title, pTexture);
	pTexture->Init(Sprite::E_SpriteType::E_Title, 1, {450, 50, 800, 300 });
	pTexture->SetRenderLayer(Actor::E_Actor);

	pTexture = new C_ATexture{};
	SetSceneObject(E_Spinner, pTexture);
	pTexture->Init(Sprite::E_SpriteType::E_Spinner, 1, { 800,400, 100, 100 });
	pTexture->SetRenderLayer(Actor::E_Actor);
	pTexture->SetRender(false);
	pState->SetEventActor(pTexture, C_ASnakeTitleState::E_Spinner);

	pTexture = new C_ATexture{};
	SetSceneObject(E_Text, pTexture);
	pTexture->Init(Sprite::E_SpriteType::E_PressEnter, 1, { 650,390, 400, 150 });
	pTexture->SetRenderLayer(Actor::E_Actor);
	pState->SetEventActor(pTexture, C_ASnakeTitleState::E_Text);

	SoundManager::FadeInMusic(Sound::Music::E_Title);
}

GameStartScene::~GameStartScene()
{
}
