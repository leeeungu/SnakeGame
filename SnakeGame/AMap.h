#pragma once

#include "Actor.h"
#include "ABlock.h"

class C_OClient;

namespace Map
{
	enum E_MapData
	{
		E_MapSize = 32,
		E_RectSize = 25
	};
};

class C_AMap : public C_Actor
{
private:
	C_ABlock* m_arBlockActor[Map::E_MapData::E_MapSize][Map::E_MapData::E_MapSize];
public:
	C_AMap();
	~C_AMap() ;

	C_ABlock* GetBlock(const SDL_Point& sPoint);
	void  SetMapPosition(const SDL_FPoint& sPoint);

	virtual void SetRegisterRender(bool bValue) override;
	void RecvUDPMapMessage(void* pMessage, int nMessageLength);
	void SendAll();
protected:
	virtual bool RecvUDPMessage(void* pMessage, int nMessageLength) override;
	virtual void Reset() override;
};