#pragma once

#include "Actor.h"
#include "SpriteManager.h"

class C_ATexture : public C_Actor
{
private:
	std::string m_sRenderText;
public:
	C_ATexture();

	void SetTexture(Sprite::E_SpriteType eSpriteType, int nIndex = 1);
	void Init(Sprite::E_SpriteType eSpriteType, int nIndex = 1, SDL_FRect sRenderPosition = {0,0,0,0});

protected:
	virtual void Update(Uint32 fDeltaTick) override;
private:
};