#pragma once


struct TextInfo
{
public:
	TextInfo(std::string font_id)
		:font_id(font_id)
	{
		font_size = 11;
		font_color = { 255,255,255,255 };
		target_pos = { 0,0 };
		isKorea = false;
		//fun = [](std::string input) {return input; };
		//str = L"";
	}
	~TextInfo() {};
	std::string font_id;
	int font_size;
	bool isKorea;
	
	std::function<std::string(std::string)> fun; 
	SDL_Color font_color;
	SDL_FPoint target_pos;
};

class TextManager
{
private:
	TextManager() {};
	static TextManager* instance_;

public:

	// Singleton
	static void CreateInstance();
	static void DestroyInstance();
	static TextManager* GetSingleton();

	// Load Font
	void LoadFont(std::string font_id, std::string font_files);

	// Draw Font
	void RenderFont(TextInfo* info, std::string str);

private:
	std::map<std::string, std::string> files_map_;
};

