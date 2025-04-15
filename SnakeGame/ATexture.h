#pragma once

#include "Actor.h"
#include "SpriteManager.h"

class C_ATexture : public C_Actor
{
private:
	std::string m_sRenderText;
public:
	C_ATexture();

	void Init(Sprite::E_SpriteType eSpriteType, int nIndex, SDL_FRect sRenderPosition);
};