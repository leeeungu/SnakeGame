#pragma once

#include "Object.h"

class C_OWindow :public C_Object
{
public:
	C_OWindow();
	~C_OWindow();
protected:
	virtual void HandleEvent() override;
};