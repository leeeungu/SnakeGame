#include "AText.h"
#include "TextManager.h"
#include "ASnake.h"

C_AText::C_AText(FontData::E_FontID eFontID, int nSize, const SDL_FPoint& sRenderPosition) :
	C_Actor{}, m_sTextinfo{}, m_sRenderText{}
{
	m_sTextinfo.eFontID = eFontID;
	m_sTextinfo.nFontSize = nSize;
	m_sRenderText.reserve(100);
	m_sRenderRect.x = sRenderPosition.x;
	m_sRenderRect.y = sRenderPosition.y;
	SetRenderLayer(Actor::E_UI);
}

void C_AText::SetTextInfo(const FontData::TextInfo& sTextInfo)
{
	m_sTextinfo.eFontID = sTextInfo.eFontID;
	SetColor(sTextInfo.sFontColor);
	m_sTextinfo.nFontSize = sTextInfo.nFontSize;
}

void C_AText::SetText(const char* strText)
{
	m_sRenderText.clear();
	m_sRenderText = strText;

	RenderManager::DeleteTexture(m_pSDLSurface);
	ReleaseSurface();
	m_pSDLSurface = TextManager::CreateFontSurface(&m_sTextinfo, m_sRenderText);
	if (m_pSDLSurface)
	{
		m_sSourceRect = { 0,0, m_pSDLSurface->w ,m_pSDLSurface->h };
		m_sRenderRect.w = (float)m_pSDLSurface->w;
		m_sRenderRect.h = (float)m_pSDLSurface->h;
	}
	RenderManager::CreateTexture(m_pSDLSurface);
}

void C_AText::SetColor(const SDL_Color& sColor)
{
	m_sTextinfo.sFontColor = sColor;
}
