#pragma once

#include "Object.h"

class GameInstance
{
public:
	enum E_InstanceType
	{
		E_Client,
		E_Server,
		E_EnumMax
	};

private:
	GameInstance() = default;
	static GameInstance* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

	static bool AddObject(E_InstanceType eType, C_Object*pObject);
	static C_Object* GetObject(E_InstanceType eType);
private:
	void DeleteObject(C_Object*& pObject);
private:
	C_Object* m_arObjects[E_EnumMax];
};