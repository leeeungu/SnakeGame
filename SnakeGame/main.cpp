#include "stdafx.h"

SDL_Window* g_window;
SDL_Renderer* g_renderer;
bool g_flag_running;
bool reset;
Uint32 g_last_time_ms;
int width;
int height;

SDL_Point operator+ (const SDL_Point& a, SDL_Point& b)
{
	SDL_Point result = a;
	result.x += b.x;
	result.y += b.y;
	return result;
}

void CreateInstances();
void DestroyInstance();

int main(int argc, char* argv[]) {
	std::srand((unsigned int)std::time(nullptr));
	reset = true;
	SDL_Init(SDL_INIT_EVERYTHING);

	width = 800;
	height = 830;
	CreateInstances();
	g_window = SDL_CreateWindow("Snake Game 2D", 200, 100, width, height, 0);
	g_renderer = SDL_CreateRenderer(g_window, -1, 0); // -1 은 글카 번호
	
	GameScene* game = new GameScene();

	Uint32 Updatetick = 33;

	g_last_time_ms = SDL_GetTicks();
	g_flag_running = true;
	while (g_flag_running) {

		Uint32 cur_time_ms = SDL_GetTicks();
		if (reset == true)
		{
			game->InitGame();
			reset = false;
		}

		if (cur_time_ms - g_last_time_ms < Updatetick)
			continue;

		//Updatetick--;
		game->HandleEvents();
		if (g_flag_running == false)
			break;

		game->Update();
	
		game->Render();
		g_last_time_ms = cur_time_ms;
	}

	DestroyInstance();
	
	delete game;
	game = nullptr;
	SDL_DestroyRenderer(g_renderer);
	g_renderer = nullptr;
	SDL_DestroyWindow(g_window);
	g_window = nullptr;
	SDL_Quit();


	
	//_CrtDumpMemoryLeaks();
	return 0;
}

void CreateInstances()
{
	SpriteManager::CreateInstance();
	TextManager::CreateInstance();
	SoundManager::CreateInstance();
}

void DestroyInstance()
{
	SoundManager::DestroyInstance();
	TextManager::DestroyInstance();
	SpriteManager::DestroyInstance();
}
