#pragma once

#include "SDL.h"
#include <map>

class RenderManager
{
	enum E_Renderer
	{
		E_RenderSize = 5,
	};
private:
	RenderManager() = default;
	static RenderManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

public:
	static int RenderTexture(SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_FRect* dstrect, const double angle = 0.0, SDL_FPoint* center = nullptr, const SDL_RendererFlip flip = SDL_RendererFlip::SDL_FLIP_NONE);
	static void SetRendering(bool bValue) { m_pInstance->m_bRendering = bValue; }
	static bool GetRendering() { return m_pInstance->m_bRendering; }
	static void StartRenderer();
	static void EndRenderer();

	static void CreateRenderer(SDL_Window* pWindow);
	//void DestroyRenderer(SDL_Window* pWindow);

	static void CreateTexture(SDL_Surface* pSurface);
	static void DeleteTexture(SDL_Surface* pSurface);

	static SDL_Texture* GetTexture(SDL_Surface* pSurface);

	static void DrawDebugRect(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color);
	static void DrawDebugBorder(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color);
	static void DrawBorder(const SDL_FRect& pRenderRect, const SDL_FPoint& pSize, const SDL_Color& color);

	static void SetRsetColor(const SDL_Color& sResetColor) { m_pInstance->m_sResetColor = sResetColor; }
private:
	std::map< SDL_Surface*, SDL_Texture*> m_mapTexture;
	SDL_Renderer* m_pRenderer;
	SDL_Color m_sResetColor;
	bool m_bDebug;
	bool m_bRendering;
};