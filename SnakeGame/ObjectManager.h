#pragma once

#include "Object.h"
#include <set>
#include <queue>

class ObjectManager
{
	friend C_Object;
private:
	ObjectManager() = default;
	static ObjectManager* m_pInstance;
public:
	static void CreateInstance();
	static void DestroyInstance();

protected:
	static void RegisterObject(C_Object* pObject);
	static void UnRegisterObject(C_Object* pObject);
public:
	static void BeingPlayObject();
	static void UpdateObject(Uint32 fDeltaTick);
	static void EndPlayObject();
	static void ResetObject();
		
private:
	void Loop(std::set< C_Object*> & setObject, void (C_Object::*Func)());
	void Loop(std::set< C_Object*> & setObject, void (C_Object::*Func)(Uint32), Uint32 fDeltaTick);
	void Loop(std::queue< C_Object*> & queObject, void (C_Object::*Func)());
private:
	std::set< C_Object*> m_setObject;
	std::queue< C_Object*> m_queBegin;
	std::queue< C_Object*> m_queEnd;
	std::set< C_Object*> m_setObjectLayer[Object::E_UpdateLayer::E_EnumMax];

	bool m_bBegin;
	bool m_bEnd;
};