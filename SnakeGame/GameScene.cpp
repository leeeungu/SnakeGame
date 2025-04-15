#include "stdafx.h"
#include "GameScene.h"

GameScene::GameScene()
{
	{  // text
		TextManager::GetSingleton()->LoadFont("cutetat", "../Resources/cutetat.ttf");
		score_textinfo = new TextInfo("cutetat");
		score_textinfo->target_pos = { 200,0 };
		score_textinfo->font_color = { 0,0,0 };
		score_textinfo->font_size = 30;

		ui_textinfo = new TextInfo("cutetat");
		ui_textinfo->target_pos = { 250,400 };
		ui_textinfo->font_color = { 255,0,0 };
		ui_textinfo->font_size = 100;
	}

	{  // music
		GameOverMusic_Id = "GameOver";
		SoundManager::GetSingleton()->LoadMusic(GameOverMusic_Id, "../Resources/GameOver.mp3");

		BackgroundMusic_Id = "BackgroundMusic";
		SoundManager::GetSingleton()->LoadMusic(BackgroundMusic_Id, "../Resources/BackgroundMusic.mp3");

		RedAppleChunk_Id = "RedApple";
		SoundManager::GetSingleton()->LoadChunk(RedAppleChunk_Id, "../Resources/RedApple.wav");
		SoundManager::GetSingleton()->SetVolumeChunk(RedAppleChunk_Id, 100);
		
		GoldAppleChunk_Id = "GoldApple";
		SoundManager::GetSingleton()->LoadChunk(GoldAppleChunk_Id, "../Resources/GoldApple.wav");
		SoundManager::GetSingleton()->SetVolumeChunk(GoldAppleChunk_Id, 100);
		
		BlueAppleChunk_Id = "BlueApple";
		SoundManager::GetSingleton()->LoadChunk(BlueAppleChunk_Id, "../Resources/BlueApple.wav");
		SoundManager::GetSingleton()->SetVolumeChunk(BlueAppleChunk_Id, 100);

		SoundManager::GetSingleton()->SetVolumeMusic(80);
	}

	{ // sprite
		SpriteManager* manager = SpriteManager::GetSingleton();
		if (manager->HasSpriteRects("CurveRightDown") == false) {
			Snake_File_ID = "Snake";
			Snake_Sprite_ID = { "CurveRightDown" ,"Row", "CurveLeftDown", "UpHead","RightHead",
							"CurveRightUp", "Col","LeftHead","DownHead", "CurveLeftUp","UpTial",
							"RightTial", "LeftTial","DownTial", "RedApple", "GoldApple","BlueApple" };

			manager->LoadTexture(Snake_File_ID, "../Resources/Snake.png");

			manager->AddSpriteRect(Snake_Sprite_ID[0], new SDL_Rect{ 0, 0, 64, 64 }); // 1,1
			manager->AddSpriteRect(Snake_Sprite_ID[1], new SDL_Rect{ 64, 0, 64, 64 });//2,1
			manager->AddSpriteRect(Snake_Sprite_ID[2], new SDL_Rect{ 128, 0, 64, 64 });//3,1
			manager->AddSpriteRect(Snake_Sprite_ID[3], new SDL_Rect{ 192 , 0, 64, 64 });//4,1
			manager->AddSpriteRect(Snake_Sprite_ID[4], new SDL_Rect{ 256, 0, 64, 64 });//5,1
			manager->AddSpriteRect(Snake_Sprite_ID[5], new SDL_Rect{ 0, 64, 64, 64 });//1,2
			manager->AddSpriteRect(Snake_Sprite_ID[6], new SDL_Rect{ 128, 64, 64, 64 });//3,2
			manager->AddSpriteRect(Snake_Sprite_ID[7], new SDL_Rect{ 192, 64, 64, 64 });//4,2
			manager->AddSpriteRect(Snake_Sprite_ID[8], new SDL_Rect{ 256, 64, 64, 64 });//5,2
			manager->AddSpriteRect(Snake_Sprite_ID[9], new SDL_Rect{ 128, 128, 64, 64 }); //3,3
			manager->AddSpriteRect(Snake_Sprite_ID[10], new SDL_Rect{ 192, 128, 64, 64 });//4,3
			manager->AddSpriteRect(Snake_Sprite_ID[11], new SDL_Rect{ 256, 128, 64, 64 });//5,3
			manager->AddSpriteRect(Snake_Sprite_ID[12], new SDL_Rect{ 192, 192, 64, 64 });//4,4
			manager->AddSpriteRect(Snake_Sprite_ID[13], new SDL_Rect{ 256, 192, 64, 64 });//5,4
			manager->AddSpriteRect(Snake_Sprite_ID[14], new SDL_Rect{ 0, 192, 64, 64 });//1,4
			manager->AddSpriteRect(Snake_Sprite_ID[15], new SDL_Rect{ 64, 192, 64, 64 });//2,4
			manager->AddSpriteRect(Snake_Sprite_ID[16], new SDL_Rect{ 0, 192, 64, 64 });//1,4
		}
		manager = nullptr;
	}

	 // map
	maps.resize(MAP_SIZE, std::vector<int>(MAP_SIZE, 0));
	InitGame();
}

GameScene::~GameScene()
{
	delete score_textinfo;
	score_textinfo = nullptr;
	delete ui_textinfo;
	ui_textinfo = nullptr;
}

void 
GameScene::InitGame()
{
	// music
	SoundManager::GetSingleton()->FadeInMusic(BackgroundMusic_Id);

	{ // level
		level = MaxLevel;
		score = 0;
		g_input = 0;
		ui_text = "Ready";
		bReady = true;
		gameover = true;
	}

	{ // snake reset
		Head = SDL_Point{ 5,5 };
		head_index = SnakeState::RightHead;

		sprite.file_id = Snake_File_ID;
		sprite.sprite_id = Snake_Sprite_ID[head_index];

		snakeBody.clear();
		snakeBody_index.clear();

		dir = { 0, 0 };
		snakeUpdate = 0;
		snakeBodyCound = 0;
	}
	
	{ // map
		for (int i = 0; i < MAP_SIZE; i++) {
			for (int j = 0; j < MAP_SIZE; j++)
				maps[i][j] = -1;
		}
		maps[Head.y][Head.x] = head_index;
	}

	SpawnApple();
}

void 
GameScene::HandleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			g_flag_running = false;
			return;
		}
		else if (event.type == SDL_KEYDOWN) { // 키 입력 마우스 왼클릭
			if (event.key.keysym.sym == SDLK_UP && snakeBodyCound == 0) {
				g_input = 1;
			}
			if (event.key.keysym.sym == SDLK_DOWN && snakeBodyCound == 0) {
				g_input = 2;
			}
			if (event.key.keysym.sym == SDLK_LEFT && snakeBodyCound == 0) {
				g_input = 3;
			}
			if (event.key.keysym.sym == SDLK_RIGHT && snakeBodyCound == 0) {
				g_input = 4;
			}
			if (g_input != 0 && bReady == true) {
				gameover = false;
				bReady = false;
			}
			return;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT && gameover == true && bReady == false)
				InitGame();
			return;
		}
	}
}

void 
GameScene::Update()
{
	if (gameover == true)
		return;

	snakeUpdate++;
	if (snakeUpdate % level == 0) {
		if (snakeBodyCound > 0)	{
			SDL_Point temp = snakeBody.back();
			maps[temp.x][temp.y] = -1;
			snakeBody.pop_back();
			snakeBody_index.pop_back();
			snakeBodyCound--;
			return;
		}

		// 이동할 곳 탐색
		int nextheadindex = head_index;
		if (g_input == 1 && dir.y != 1)	{
			dir = { 0,-1 };
			nextheadindex = SnakeState::UpHead;
		}
		else if (g_input == 2 && dir.y != -1) {
			dir = { 0,1 };
			nextheadindex = SnakeState::DownHead;
		}
		else if (g_input == 3 && dir.x != 1)  {
			dir = { -1,0 };
			nextheadindex = SnakeState::LeftHead;
		}
		else if (g_input == 4 && dir.x != -1) {
			dir = { 1,0 };
			nextheadindex = SnakeState::RightHead;
		}
		
		if (Head.y + dir.y >= 0 && Head.x + dir.x >= 0 &&
			Head.y + dir.y < MAP_SIZE && Head.x + dir.x < MAP_SIZE) {

			int nextx = Head.x + dir.x, nexty = Head.y + dir.y;
			if (maps[nextx][nexty] >= 0 && maps[nextx][nexty] < 14)
				gameover = true;
			else if (maps[nextx][nexty] == 14)
			{
				bAddTail = true;
				SoundManager::GetSingleton()->PlayChannel(RedAppleChunk_Id);
			}
			else if (maps[nextx][nexty] == 15)
			{
				maps[nextx][nexty] = -1;
				snakeBodyCound = ((int)snakeBody.size() + 1) / 2;
				SoundManager::GetSingleton()->PlayChannel(GoldAppleChunk_Id);
				return;
			}
			else if (maps[nextx][nexty] == 16 && level < MaxLevel)
			{
				level++;
				SoundManager::GetSingleton()->PlayChannel(BlueAppleChunk_Id);
			}
		}
		else
			gameover = true;

		if (gameover == true)
		{
			ui_text = "GameOver";
			SoundManager::GetSingleton()->FadeOutMusic(0);
			SoundManager::GetSingleton()->FadeInMusic(GameOverMusic_Id,1);
			return;
		}

		// 이동 및 그림 지정
		SetSprite();
		snakeBody.push_front(Head);
		if (snakeBody.empty() == false) {
			if (bAddTail == false) {
				SDL_Point temp = snakeBody.back();
				maps[temp.x][temp.y] = -1;
				snakeBody.pop_back();
				snakeBody_index.pop_back();

				// 꼬리
				if (snakeBody_index.size() > 1)
				{
					snakeBody_index.pop_back();// 꼬리
					SDL_Point pre = snakeBody.back(); // 꼬리
					snakeBody.pop_back();
					int index = snakeBody_index.back();// 꼬리의 앞부분
					SDL_Point prepre = snakeBody.back(); // 꼬리의 앞부분

					if (prepre.x == pre.x)
					{
						if (prepre.y > temp.y)
							index = SnakeState::DownTial;
						else
							index = SnakeState::UpTial;
					}
					else if (prepre.x > pre.x)
						index = SnakeState::RightTial;
					else
						index = SnakeState::LeftTial;

					snakeBody.push_back(pre);
					snakeBody_index.push_back(index);
				}
			}
			else {
				bAddTail = false;
				score += 10;
				if (score % 100 == 0 && level > 1)
					level--;
			}

			if (snakeBody_index.size() == 1) // 꼬리
			{
				snakeBody_index.pop_back();
				int index = nextheadindex;
				if (index == SnakeState::UpHead)
					index = SnakeState::UpTial;
				else if (index == SnakeState::DownHead)
					index = SnakeState::DownTial;
				else if (index == SnakeState::RightHead)
					index = SnakeState::RightTial;
				else if (index == SnakeState::LeftHead)
					index = SnakeState::LeftTial;
				snakeBody_index.push_back(index);
			}
			if (snakeBody_index.size() >= 1) // 맵에 저장
			{
				SDL_Point temp = snakeBody.front();
				maps[temp.x][temp.y] = snakeBody_index.front();
				temp = snakeBody.back();
				maps[temp.x][temp.y] = snakeBody_index.back();
			}
		}
		
		Head.x += dir.x;
		Head.y += dir.y;
		maps[Head.x][Head.y] = head_index = nextheadindex;
	}
	if (snakeUpdate % 40 == 0)
		SpawnApple();

	if (snakeUpdate > 100000)
		snakeUpdate = 1;
}

void 
GameScene::Render()
{
	SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
	SDL_RenderClear(g_renderer);

	SpriteManager* manager = SpriteManager::GetSingleton();
	SDL_FRect temp;
	{   // Map Border Render snake render
		for (int i = 0; i < MAP_SIZE; i++) {
			for (int j = 0; j < MAP_SIZE; j++) {
				temp = { i * RECT_SIZE,30.f+ j * RECT_SIZE, RECT_SIZE, RECT_SIZE };
				sprite.color = { 255,255,255 };

				manager->bDebug = true;
				manager->DrawDebugBorder(&temp, 1.0f, 1.0f, SDL_Color({ 0,0,0 }));
				manager->bDebug = false;

				if (maps[i][j] >= 0)
				{
					sprite.sprite_id = Snake_Sprite_ID[maps[i][j]];
					if (maps[i][j] >= 16) 
						sprite.color = { 0,0, 255 };

					manager->DrawSprite(&temp, &sprite);
				}
			}
		}
	}
	sprite.color = { 255,255,255 };

	{ // text
		if (gameover == true)
			TextManager::GetSingleton()->RenderFont(ui_textinfo, ui_text);

		score_textinfo->target_pos = { 100,0 };
		TextManager::GetSingleton()->RenderFont(score_textinfo, "Score : " + std::to_string(score));

		score_textinfo->target_pos = { 300,0 };
		TextManager::GetSingleton()->RenderFont(score_textinfo, "Length : " + std::to_string(snakeBody.size() + 1));

		score_textinfo->target_pos = { 500,0 };
		TextManager::GetSingleton()->RenderFont(score_textinfo, "Speed : " + std::to_string(MaxLevel - level + 1));
	}
	
	SDL_RenderPresent(g_renderer);
}

void 
GameScene::ClearGame()
{
	SoundManager::GetSingleton()->FadeOutMusic(0);
}

void 
GameScene::SetSprite( )
{
	if (snakeBody.empty() == false) {	// 머리 바로 다음의 몸통
		SDL_Point next = Head + dir; // 갈 곳
		SDL_Point pre = snakeBody.front(); // 추가 전
		int index = SnakeState::CurveRightDown;

		if (pre.y == Head.y) { // 가로로 
			if (Head.y == next.y)
				index = SnakeState::Row;
			else if (next.y > Head.y ) { // 꼬리가 왼족 방향	ㄱ	or 반전 ㄱ ?
				if (pre.x < next.x) // ㄱ
					index = SnakeState::CurveLeftDown;
				else // 반전 ㄱ
					index = SnakeState::CurveRightDown;
			}
			else {  // ㄴ or 반전 ㄴ
				if (next.x > pre.x) // 반전 ㄴ
					index = SnakeState::CurveLeftUp;
				else // ㄴ
					index = SnakeState::CurveRightUp;
			}
		}
		else {  // 세로
			if (Head.x == next.x)
				index = SnakeState::Col;
			else if (next.x < Head.x) {// 꼬리가 위 ㄱ or 반전 ㄴ
				if (pre.y < next.y) // ㄱ
					index = SnakeState::CurveLeftUp;
 				else // 반전 ㄴ
					index = SnakeState::CurveLeftDown;
			}
			else { // ㄴ or 반전 ㄱ 
				if (pre.y > next.y)  // 반전 ㄱ
					index = SnakeState::CurveRightDown;
				else // ㄴ
					index = SnakeState::CurveRightUp;
			}
		}
		snakeBody_index.push_front(index);
	}
	else 
		snakeBody_index.push_back(head_index);
}

void 
GameScene::SpawnApple()
{
	int count = 0, indexy = 0, indexx =0;
	int AppleKind  = 0;
	while (count++ < 5) {
		indexy = rand() % MAP_SIZE;
		indexx = rand() % MAP_SIZE;
		AppleKind = rand() % 100;
		if (maps[indexy][indexx] == -1)
		{
			if (AppleKind < 3)
				maps[indexy][indexx] = 16;
			else if (AppleKind < 6)
				maps[indexy][indexx] = 15;
			else
				maps[indexy][indexx] = 14;
			break;
		}
	}
}