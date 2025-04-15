#include "TextManager.h"

TextManager* TextManager::m_pInstance = nullptr;
void TextManager::CreateInstance() 
{
	if (m_pInstance != 0)
		return;
	TTF_Init();
	m_pInstance = new TextManager();
	using namespace FontData;

	for (int i = 0; i < E_FontID::E_EnmMax; i++)
	{
		m_pInstance->LoadFont((E_FontID)i);
	}
}

void TextManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	using namespace std;
	using namespace FontData;

	for (int i = 0; i < E_FontID::E_EnmMax; i++)
	{
		map<int, TTF_Font*>& mapTTP = m_pInstance->m_mapOpendFont[i];
		map<int, TTF_Font*>::iterator pIter = mapTTP.begin();
		while (pIter != mapTTP.end())
		{
			TTF_CloseFont(pIter->second);
			pIter->second = nullptr;
			pIter++;
		}
	}
	map<int, TTF_Font*> m_mapOpendFont[E_FontID::E_EnmMax];

	delete(m_pInstance);
	m_pInstance = nullptr;
	TTF_Quit();
}

std::string TextManager::GetFontPath()
{
	return "../Resources/Font/";
}

std::string TextManager::GetFontName(FontData::E_FontID eFontID)
{
	std::string arData[FontData::E_FontID::E_EnmMax]{};
	arData[FontData::E_FontID::E_cutetat] = "cutetat.ttf";
	return arData[eFontID];
}

TTF_Font* TextManager::GetTTFFont(FontData::E_FontID eFontID, int nFontSize)
{
	if (eFontID >= FontData::E_FontID::E_EnmMax || eFontID < 0)
		eFontID = FontData::E_FontID::E_cutetat;
	std::pair<std::map<int, TTF_Font*>::iterator, bool> FontIter = m_mapOpendFont[eFontID].insert({ nFontSize, nullptr });
	TTF_Font*& pTTFFont = FontIter.first->second;
	if (!pTTFFont)
	{
		pTTFFont = TTF_OpenFont(m_arFontPath[eFontID].c_str(), nFontSize);
	}
	return pTTFFont;
}

void TextManager::LoadFont(FontData::E_FontID eFonID)
{
	if (m_pInstance->m_arFontPath[eFonID].empty())
		m_pInstance->m_arFontPath[eFonID] = GetFontPath() + GetFontName(eFonID);
}

SDL_Surface* TextManager::CreateFontSurface(FontData::TextInfo* info, std::string& str)
{
	FontData::E_FontID eFontID = info->eFontID;
	std::string& strPath = m_pInstance->m_arFontPath[eFontID];
	TTF_Font* pTTFFont = m_pInstance->GetTTFFont(eFontID, info->nFontSize);
	if (!pTTFFont)
		return nullptr;
	return  TTF_RenderUTF8_Blended(pTTFFont, str.c_str(), info->sFontColor);
}


