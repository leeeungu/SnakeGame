#include "FrameWork.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "TextManager.h"
#include "SpriteManager.h"
#include "ObjectManager.h"
#include "ActorManager.h"
#include "InputHandleManager.h"
#include "WindowManager.h"
#include "UDPManager.h"
#include "TCPManager.h"
#include "MultiThreadManager.h"
#include "DebugMessageManager.h"
#include "FileReader.h"
#include <iostream>
#include "GameServer.h"
#include "GameScene.h"
#include "GameStartScene.h"
#include "GameInstance.h"

FrameWork* FrameWork::m_pInstance = nullptr;

void FrameWork::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new FrameWork{};
	SDLNet_Init();

	m_pInstance->m_bReset = true;
	m_pInstance->m_bRunning = true;
	m_pInstance->m_bUpdate = true;
	m_pInstance->m_nLastUpdate = SDL_GetTicks();
	WindowManager::CreateInstance();
	RenderManager::CreateInstance();
	InputHandleManager::CreateInstance();
	SpriteManager::CreateInstance();
	ObjectManager::CreateInstance();
	ActorManager::CreateInstance();
	TextManager::CreateInstance();
	SoundManager::CreateInstance();
	TCPManager::CreateInstance();
	UDPManager::CreateInstance();
	MultiThreadManager::CreateInstance(10);
	GameInstance::CreateInstance();

	std::vector<std::string> arData{};
	arData.resize(2);
	if (FileReader::ReadFile("../Resources/ServerIP/ServerIP.txt", arData))
	{
		TCPManager::SetServerIP(arData[0]);
		UDPManager::SetServerIP(arData[0]);
		TCPManager::SetTCPPort(std::stoi(arData[1]));
		UDPManager::SetUDPPort(std::stoi(arData[1]));
	}

	bool bServer{};
	bServer = true;
	bServer = false;
	if (bServer)
	{
		CreateWindow("Snake Server", { 300,300});
		m_pInstance->ChangeScene(Framework::Scene::E_SnakeServer);
		m_pInstance->SetUpdatetick(1);
		RenderManager::SetRendering(false);
	}
	else
	{
		CreateWindow("Snake Game 2D", { 1700,830 });
		m_pInstance->ChangeScene(Framework::Scene::E_Title);
		m_pInstance->SetUpdatetick(33);
	}
}

void FrameWork::DestroyInstance()
{
	if (!m_pInstance)
		return;

	if (m_pInstance->m_pMainScene)
	{
		delete m_pInstance->m_pMainScene;
		m_pInstance->m_pMainScene = nullptr;
	}
	GameInstance::DestroyInstance();
	MultiThreadManager::DestroyInstance();
	UDPManager::DestroyInstance();
	TCPManager::DestroyInstance();
	SoundManager::DestroyInstance();
	TextManager::DestroyInstance();
	ActorManager::DestroyInstance();
	ObjectManager::DestroyInstance();
	SpriteManager::DestroyInstance();
	InputHandleManager::DestroyInstance();
	RenderManager::DestroyInstance();
	WindowManager::DestroyInstance();

	SDLNet_Quit();
	delete m_pInstance;
	m_pInstance = nullptr;
}

void FrameWork::CreateWindow(const char* strWindwoName, const SDL_Point& sSize, const SDL_Point& sPosition)
{
	if (WindowManager::CreateWindow(strWindwoName, sSize, m_pInstance->m_nWindowIndex, sPosition))
		RenderManager::CreateRenderer(WindowManager::GetWindow(m_pInstance->m_nWindowIndex));
}

void FrameWork::DestroyWindow()
{
	WindowManager::DestoryWindow(m_pInstance->m_nWindowIndex);
}

void FrameWork::Init()
{
	if (m_pInstance->m_pMainScene)
	{
		m_pInstance->m_pMainScene->Init();
	}
}

void FrameWork::HandleEvents()
{
	if (m_pInstance->m_pMainScene)
	{
		m_pInstance->m_pMainScene->HandleEvents();
	}
}

void FrameWork::Update()
{
	if (m_pInstance->m_pMainScene && m_pInstance->GetUpdate())
	{
		m_pInstance->m_pMainScene->Update(m_pInstance->m_nDeltaTime);
	}
}

void FrameWork::Render()
{
	if (m_pInstance->m_pMainScene && RenderManager::GetRendering())
	{
		m_pInstance->m_pMainScene->Render();
	}
}

void FrameWork::ChangeScene(Framework::Scene::E_Type eSceneType)
{
	if (m_pInstance->m_eSceneType == eSceneType)
		return;
	Scene* pScene{};
	m_pInstance->m_eSceneType = eSceneType;

	switch (eSceneType)
	{
	case Framework::Scene::E_SnakeGame:
		pScene = new GameScene{};
			break;
	case Framework::Scene::E_SnakeServer:
		pScene = new GameServer{};
		break;
	case Framework::Scene::E_Title:
		pScene = new GameStartScene{};
		break;
	default:
		break;
	}
	if (!pScene)
	{
		SetRunning(false);
		return;
	}

	if (m_pInstance->m_pMainScene)
	{
		delete m_pInstance->m_pMainScene;
		m_pInstance->m_pMainScene = nullptr;
	}
	m_pInstance->m_pMainScene = pScene;
	FrameWork::SetRset(true);
}

bool FrameWork::TickUpdate()
{
	Uint32 cur_time_ms = SDL_GetTicks();
	m_pInstance->m_nDeltaTime = cur_time_ms - m_pInstance->m_nLastUpdate;
	bool bReulst = m_pInstance->m_nDeltaTime >= m_pInstance->m_nUpdatetick;
	if (bReulst)
		m_pInstance->m_nLastUpdate = cur_time_ms;
	return bReulst;
}
