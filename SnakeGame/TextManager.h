#pragma once

#include <string>
#include <map>
#include "SDL.h"
#include "SDL_ttf.h"


namespace FontData
{
	enum E_FontID
	{
		E_cutetat,
		E_EnmMax
	};
	
	struct TextInfo
	{
	public:
		TextInfo() = default;
		TextInfo(E_FontID eInputFontID)
			: eFontID(eInputFontID), sFontColor{}, nFontSize{}, bIsKorea{}
		{
		}
		~TextInfo() = default;
		E_FontID eFontID;
		SDL_Color sFontColor = { 0,0,0,0 };
		int nFontSize = 11;
		bool bIsKorea = true;
	};
}

// Singleton
class TextManager
{
private:
	TextManager()  = default;
	static TextManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();
private:
	std::string GetFontPath();
	std::string GetFontName(FontData::E_FontID eFontID);
	TTF_Font* GetTTFFont(FontData::E_FontID eFontID, int nFontSize);
	// Load Font
	void LoadFont(FontData::E_FontID eFontID);
public:
	// Draw Font
	static SDL_Surface* CreateFontSurface(FontData::TextInfo* info, std::string& str);
	
private:
	std::string m_arFontPath[FontData::E_FontID::E_EnmMax];
	std::map<int, TTF_Font*> m_mapOpendFont[FontData::E_FontID::E_EnmMax];
};

