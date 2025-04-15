#pragma once

#include <iostream>
#include <windows.h>
#include <string>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <list>
#include <deque>

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "SDL_image.h"

extern SDL_Renderer* g_renderer;
extern int width;
extern int height;

extern bool g_flag_running;
extern bool reset;

#include "SpriteManager.h"
#include "TextManager.h"
#include "SoundManager.h"

#include "GameScene.h"


SDL_Point operator+ (const SDL_Point& a, SDL_Point& b);