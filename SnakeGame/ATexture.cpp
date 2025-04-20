#include "ATexture.h"
#include "SpriteManager.h"

C_ATexture::C_ATexture()
{
	SetRegisterRender(true);
	SetUpdate(false);
}

void C_ATexture::SetTexture(Sprite::E_SpriteType eSpriteType, int nIndex)
{
	m_pSDLSurface = SpriteManager::LoadImageSurface(eSpriteType);
	SpriteManager::GetSpriteSourceRect(eSpriteType, nIndex, m_sSourceRect);
}

void C_ATexture::Init(Sprite::E_SpriteType eSpriteType, int nIndex, SDL_FRect sRenderPosition)
{
	SetTexture(eSpriteType, nIndex);
	m_sRenderRect = sRenderPosition;
}

void C_ATexture::Update(Uint32 fDeltaTick)
{
	m_dAngle += fDeltaTick;
	if (m_dAngle > 360)
		m_dAngle = 0;
}
