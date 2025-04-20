#pragma once

#include "Actor.h"

namespace Block
{
	namespace Apple
	{
		enum E_TypeID
		{
			E_None,
			E_Red,
			E_Gold,
			E_Blue,
			E_EnumMax
		};
	}
}

class C_ABlock : public C_Actor
{
	friend class C_AMap;
public:
	C_ABlock();

	void SetSpriteIndex(unsigned int nIndex);
	void SetSpriteIndex_Map(unsigned int nIndex);
	unsigned int GetSpriteIndex() { return m_nSpriteIndex; }
	void SetCoord(const SDL_Point& nCoord) { m_nCoord = nCoord; }
	SDL_Point GetCoord() { return m_nCoord;}
	Block::Apple::E_TypeID GetAppleType() { return m_eApple; }
	
	void SendMessage();

protected:
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength) override;
	virtual void Render() override;
	virtual bool OverlapEvent(C_Actor* pActor) override;
	virtual void Reset() override;

private:
	unsigned int m_nSpriteIndex;
	Block::Apple::E_TypeID m_eApple;
	SDL_Color m_arColorMode[Block::Apple::E_TypeID::E_EnumMax];
	SDL_Point m_nCoord;
};