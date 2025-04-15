#include "ATexture.h"
#include "SpriteManager.h"

C_ATexture::C_ATexture()
{
}

void C_ATexture::Init(Sprite::E_SpriteType eSpriteType, int nIndex, SDL_FRect sRenderPosition)
{
	m_pSDLSurface = SpriteManager::LoadImageSurface(eSpriteType);
	SpriteManager::GetSpriteSourceRect(eSpriteType, nIndex, m_sSourceRect);
	m_sRenderRect = sRenderPosition;
}
