#pragma once

#define MAP_SIZE 32
#define RECT_SIZE 25.0f
#define MaxLevel 6

enum  SnakeState
{
	CurveRightDown = 0,
	Row,
	CurveLeftDown,
	UpHead,
	RightHead,
	CurveRightUp,
	Col,
	LeftHead,
	DownHead,
	CurveLeftUp,
	UpTial,
	RightTial,
	LeftTial,
	DownTial,
	RedApple,
	GoldApple,
	BlueApple
};

class GameScene
{
public:
	GameScene();
	~GameScene();
	// Scene을(를) 통해 상속됨
	void InitGame() ;
	void HandleEvents() ;
	void Update() ;
	void Render() ;
	void ClearGame() ;

private:
	void SetSprite();
	void SpawnApple();

private:
	// game event
	int g_input;
	int score;
	int level;
	bool gameover;
	bool bReady;

	// music
	std::string BackgroundMusic_Id;
	std::string GameOverMusic_Id;

	std::string GoldAppleChunk_Id;
	std::string RedAppleChunk_Id;
	std::string BlueAppleChunk_Id;

	// text
	struct TextInfo* score_textinfo;
	struct TextInfo* ui_textinfo;
	std::string ui_text;

	// resource
	struct SpriteInfo sprite;
	std::string Snake_File_ID;
	std::vector<std::string> Snake_Sprite_ID;

	// snake head
	SDL_Point Head;
	int head_index ;
	SDL_Point dir;

	// snake body
	std::list<SDL_Point> snakeBody;
	std::list<int> snakeBody_index;

	// snake update data
	bool bAddTail;
	int snakeUpdate;
	int snakeBodyCound;

	// map
	std::vector<std::vector<int>> maps;
};

//
	//{
	//	// score text
	//	score_text->font_color = { 255, 0, 0, 0 };
	//	score_text->font_size = 60;
	//	score_text->target_pos = { 100,(float)height - 100 };
	//	score_text->fun = [](std::wstring score) {
	//		std::wstring temp = L"00000";
	//		temp += score;
	//		std::reverse(temp.begin(), temp.end());
	//		temp.resize(5);
	//		std::reverse(temp.begin(), temp.end()); 
	//		return L"score : " + temp; };
	//}
