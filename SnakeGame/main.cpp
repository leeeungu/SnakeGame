#include <iostream>
#include "FrameWork.h"

SDL_Point operator+ (const SDL_Point& a, SDL_Point& b)
{
	SDL_Point result = a;
	result.x += b.x;
	result.y += b.y;
	return result;
}

int main(int argc, char* argv[]) {
	std::srand((unsigned int)std::time(nullptr));
	SDL_Init(SDL_INIT_EVERYTHING);

	//_CrtSetBreakAlloc(299);

	FrameWork::CreateInstance();

	while (FrameWork::GetRunning()) {

		if (FrameWork::GetRset())
		{
			FrameWork::Init();
			FrameWork::SetRset(false);
		}


		FrameWork::HandleEvents();
		if (FrameWork::TickUpdate())
		{
			if (FrameWork::GetRunning())
			{
				FrameWork::Update();
				FrameWork::Render();
			}
		}
	}

	FrameWork::DestroyInstance();
	_CrtDumpMemoryLeaks();
	SDL_Quit();
	return 0;
}
