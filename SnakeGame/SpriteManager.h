#pragma once


struct SpriteInfo
{
public:
	SpriteInfo(std::string file_id) {
		this->file_id = file_id;
		sizeX = 1.0f;
		sizeY = 1.0f;
		flipsMode = SDL_FLIP_NONE;
		currentIndex = 0;
		color = { 255,255,255,0 };
		brender = true;
		spriteSpeed = -1;
		currenttime = 0;
	}
	SpriteInfo()
	{
		this->file_id = "";
		sizeX = 1.0f;
		sizeY = 1.0f;
		flipsMode = SDL_FLIP_NONE;
		currentIndex = 0;
		color = { 255,255,255,0 };
		brender = true;
		spriteSpeed = -1;
		currenttime = 0;
		bloop = true;
		bend = false;
	}
	~SpriteInfo() {
	};

	// drawRect & size
	float sizeX;
	float sizeY;

	// draw image
	std::string file_id; // file id
	std::string sprite_id; //sprite id (연속해서 그릴 그림의 id)
	int currentIndex; 

	SDL_RendererFlip flipsMode;  // flips
	SDL_Color color;

	float spriteSpeed; // randering speed
	Uint32 currenttime;

	bool brender; // draw or not
	bool bloop;
	bool bend;
};

class SpriteManager
{
private:
	SpriteManager() {};
	static SpriteManager* instance_;

public:

	// Singleton
	static void CreateInstance();
	static void DestroyInstance();
	static SpriteManager* GetSingleton();

	// Load Texture 
	static bool LoadTexture(std::string file_id, std::string file_name);
	static bool LoadTexture(std::string file_id, std::string file_name, unsigned char color_key_r, unsigned char color_key_g, unsigned char color_key_b);
	 
	static void AddSpriteRect(std::string sprite_id, SDL_Rect* rect);

	// Draw
	static void DrawSprite(SDL_FRect* drawRect, SpriteInfo* info);

	// setting
	static size_t GetSpriteIndexSize(std::string sprite_id) { return instance_->sprite_rects_[sprite_id]->size();  }

	static bool HasSpriteRects(std::string sprite_id) { return instance_->sprite_rects_[sprite_id] != nullptr ? true : false; }
	static SDL_Rect* GetSpriteRect(std::string sprite_id) { return &(*instance_->sprite_rects_[sprite_id])[0]; }

	//debug
	static void DrawDebugRect(SDL_FRect* info, float sizeX =1, float sizeY =1, SDL_Color color = { 255,0,0 });
	void DrawDebugRect(SDL_FRect* info, SDL_Color color);
	static void DrawDebugBorder(SDL_FRect* info, float sizeX = 1, float sizeY = 1, SDL_Color color = { 255,0,0 });

	void DrawDebugBorder(SDL_FRect* info, SDL_Color color);

	bool bDebug;
private:
	std::map<std::string, SDL_Texture*> texture_map_;
	std::map<std::string, std::vector<SDL_Rect>*> sprite_rects_;
};
