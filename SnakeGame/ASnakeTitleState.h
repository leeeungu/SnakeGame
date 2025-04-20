#pragma once

#include "Actor.h"

class C_ASnakeTitleState : public C_Actor
{
public:
	enum E_EventActor
	{
		E_Text,
		E_Spinner,
		E_EnumMax
	};
public:
	C_ASnakeTitleState();
	~C_ASnakeTitleState();

protected:
	virtual void HandleEvent() override;
private:

	bool m_bMathchingStart;
};