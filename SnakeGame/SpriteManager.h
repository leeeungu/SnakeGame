#pragma once

#include <string>
#include <vector>
#include "SDL.h"

namespace Sprite
{
	enum E_SpriteType
	{
		E_None,
		E_Block,
		E_Background,
		E_EnumMax
	};
	namespace Background
	{
		enum E_SpriteID
		{
			E_None,
			E_Title,
			E_EnumMax
		};
	}

	namespace Block
	{
		enum E_SpriteID
		{
			E_None,
			E_CurveRightDown,
			E_Row,
			E_CurveLeftDown,
			E_UpHead,
			E_RightHead,
			E_CurveRightUp,
			E_Col,
			E_LeftHead,
			E_DownHead,
			E_CurveLeftUp,
			E_UpTial,
			E_RightTial,
			E_LeftTial,
			E_DownTial,
			E_RedApple,
			E_GoldApple,
			E_BlueApple,
			E_EnumMax
		};
	}
}

class SpriteManager
{
private:
	SpriteManager() = default;
	static SpriteManager* m_pInstance;

public:

	// Singleton
	static void CreateInstance();
	static void DestroyInstance();

	static SDL_Surface* LoadImageSurface(Sprite::E_SpriteType eSpriteType);
	static bool RegisterSpriteRect(Sprite::E_SpriteType eSpriteType, unsigned int nIndex,const SDL_Rect& rect);
	static bool GetSpriteSourceRect(Sprite::E_SpriteType eSpriteType, unsigned int nIndex, SDL_Rect& sRect);
	static std::string GetFilePath();
	static std::string GetImageFile(Sprite::E_SpriteType eSpriteType);
	static SDL_Surface* GetImageSurface(Sprite::E_SpriteType eSpriteType);

private:
	void GetSourceRect(Sprite::E_SpriteType eSpriteType, std::vector< SDL_Rect>& arResult);
private:
	unsigned int m_arSpriteIndex[Sprite::E_SpriteType::E_EnumMax];
	SDL_Rect* m_arSprite[Sprite::E_SpriteType::E_EnumMax];
	SDL_Surface* m_arSpriteSurface[Sprite::E_SpriteType::E_EnumMax];
	SDL_Rect m_arSnake[(int)Sprite::Block::E_SpriteID::E_EnumMax];
	SDL_Rect m_arTexture[(int)Sprite::Background::E_SpriteID::E_EnumMax];
};
