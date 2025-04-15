#pragma once

#include "SDL.h"

class Scene;

namespace Framework
{
	namespace Scene
	{
		enum  E_Type
		{
			E_NONE,
			E_Title,
			E_SnakeGame,
			E_SnakeServer,
			E_EnumMax
		};
	}
}

class FrameWork
{
private:
	FrameWork() = default;
	static FrameWork* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static void CreateWindow(const char* strWindwoName, const SDL_Point& sSize, const SDL_Point& sPosition = { 200,100 });
	static void DestroyWindow();

	static void Init();
	static void HandleEvents();
	static void Update();
	static void Render();

	static bool TickUpdate();
	static void SetRunning(bool bValue) { m_pInstance->m_bRunning = bValue; }
	static bool GetRunning() { return			m_pInstance->m_bRunning; }
	static void SetRset(bool bValue) { m_pInstance->m_bReset = bValue; }
	static bool GetRset() { return			m_pInstance->m_bReset; }
	static void SetUpdate(bool bValue) { m_pInstance->m_bUpdate = bValue; }
	static bool GetUpdate() { return			m_pInstance->m_bUpdate; }
	static void SetUpdatetick(Uint32 nTick) { m_pInstance->m_nUpdatetick = nTick; }
	static Uint32 GetUpdatetick() { return	m_pInstance->m_nUpdatetick; }
	static Uint32 GetDeltaTime() { return	m_pInstance->m_nDeltaTime; }

public:
	void ChangeScene(Framework::Scene::E_Type eSceneType);


private:
	int m_nWindowIndex;
	bool m_bRunning;
	bool m_bReset;
	bool m_bUpdate;
	Framework::Scene::E_Type m_eSceneType;
	Scene* m_pMainScene;
	Uint32 m_nUpdatetick = 33;
	Uint32 m_nLastUpdate;
	Uint32 m_nDeltaTime;
};