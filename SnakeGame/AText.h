#pragma once

#include "Actor.h"
#include "TextManager.h"
#include <string>

class C_AText: public C_Actor
{
private:
	FontData::TextInfo m_sTextinfo;
	std::string m_sRenderText;
public:
	C_AText() = delete;
	C_AText(FontData::E_FontID eFontID = FontData::E_FontID::E_cutetat, int nSize = 30.f, const SDL_FPoint& sRenderPosition = {0,0});

	const FontData::TextInfo* GetTextInfo() { return &m_sTextinfo; }
	void SetTextInfo(const FontData::TextInfo& sTextInfo);
	void SetFontSize(int nFontSize) { m_sTextinfo.nFontSize = nFontSize;  }
	void SetText(const char* strText);
	void SetColor(const SDL_Color& sColor);
};