#pragma once

#include "Actor.h"

class C_AMap;
class C_AText;

class C_AOpponent : public C_Actor
{
private:
	enum  E_Text
	{
		E_Score,
		E_Speed,
		E_Length,
		E_EnumMax
	};
public:
	C_AOpponent();
	~C_AOpponent();


	void SetState(int nSpeed, int nScore, int nLength);
	void SetMap(void* pMessage, int nMessageLength);
	void SendAll();
private:
	C_AMap* m_pOpponentMap;
	C_AText* m_pText[E_EnumMax];
};