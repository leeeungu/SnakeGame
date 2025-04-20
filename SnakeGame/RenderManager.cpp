#include "RenderManager.h"
#include "SDL.h"
#include "SDL_image.h"

RenderManager* RenderManager::m_pInstance = nullptr;

void RenderManager::CreateInstance()
{
	if (m_pInstance)
		return;
	m_pInstance = new RenderManager{};
	m_pInstance->m_sResetColor = { 255,255,255,255 };
	m_pInstance->m_bRendering = true;
}

void RenderManager::DestroyInstance()
{
	if (!m_pInstance)
		return;
	SDL_DestroyRenderer(m_pInstance->m_pRenderer);
	m_pInstance->m_pRenderer = nullptr;

	delete m_pInstance;
	m_pInstance = nullptr;
}

int RenderManager::RenderTexture(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_FRect* dstrect, const double angle, SDL_FPoint* center, const SDL_RendererFlip flip)
{
	return SDL_RenderCopyExF(m_pInstance->m_pRenderer, texture, srcrect, dstrect, angle, center, flip);
	//return SDL_RenderCopyF(m_pInstance->m_pRenderer, texture, srcrect, dstrect);
}

void RenderManager::StartRenderer()
{
	if (!m_pInstance->m_bRendering)
		return;
	SDL_SetRenderDrawColor(m_pInstance->m_pRenderer, m_pInstance->m_sResetColor.r, m_pInstance->m_sResetColor.g, m_pInstance->m_sResetColor.b, m_pInstance->m_sResetColor.a);
	SDL_RenderClear(m_pInstance->m_pRenderer);
}

void RenderManager::EndRenderer()
{
	if (!m_pInstance->m_bRendering)
		return;
	SDL_RenderPresent(m_pInstance->m_pRenderer);
}

void RenderManager::CreateRenderer(SDL_Window* pWindow)
{
	if (m_pInstance->m_pRenderer)
		return;
	m_pInstance->m_pRenderer = SDL_CreateRenderer(pWindow, -1, 0);
}

void RenderManager::CreateTexture(SDL_Surface* pSurface)
{
	using namespace std;
	pair<map< SDL_Surface*, SDL_Texture*>::iterator, bool> Iter = m_pInstance->m_mapTexture.insert({ pSurface, nullptr });
	if (!Iter.first->second)
	{
		Iter.first->second = SDL_CreateTextureFromSurface(m_pInstance->m_pRenderer, pSurface);
	}
}

SDL_Texture* RenderManager::GetTexture(SDL_Surface* pSurface)
{
	if (!pSurface)
		return nullptr;
	using namespace std;
	pair<map< SDL_Surface*, SDL_Texture*>::iterator, bool> Iter = m_pInstance->m_mapTexture.insert({ pSurface, nullptr });
	if (!Iter.first->second)
	{
		Iter.first->second = SDL_CreateTextureFromSurface(m_pInstance->m_pRenderer, pSurface);
	}
	return Iter.first->second;
}

void RenderManager::DeleteTexture(SDL_Surface* pSurface)
{
	using namespace std;
	pair<map< SDL_Surface*, SDL_Texture*>::iterator, bool> Iter = m_pInstance->m_mapTexture.insert({ pSurface, nullptr });
	if (Iter.first->second)
	{
		SDL_DestroyTexture(Iter.first->second);
		Iter.first->second = nullptr;
	}
}

void RenderManager::DrawDebugRect(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color)
{
	SDL_Renderer* pRenderer = m_pInstance->m_pRenderer;
	if (m_pInstance->m_bDebug == false || !pRenderer )
		return;
	SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, color.a);
	SDL_FRect temp = pRenderRect;
	temp.w *= pSize.x;
	temp.h *= pSize.y;
	SDL_RenderFillRectF(pRenderer, &temp);
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0);
}

void RenderManager::DrawDebugBorder(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color)
{
	if (m_pInstance->m_bDebug == false )
		return;
	DrawBorder(pRenderRect, pSize, color);
}

void RenderManager::DrawBorder(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color)
{
	SDL_Renderer* pRenderer = m_pInstance->m_pRenderer;
	if ( !pRenderer)
		return;
	SDL_SetRenderDrawColor(pRenderer, color.r, color.g, color.b, color.a);

	SDL_FRect temp = pRenderRect;
	temp.w *= pSize.x;
	temp.h *= pSize.y;
	SDL_RenderDrawRectF(pRenderer, &temp);
	SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0);
}
