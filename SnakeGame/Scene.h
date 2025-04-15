#pragma once

#include "SDL_stdinc.h"

class C_Object;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene();

	void Reset();
	void Init();
	void HandleEvents();
	void Update(Uint32 fDeltaTick);
	void Render();

protected:
	void CreateActorsArray(int nSize);
	void SetSceneObject(int nIndex, C_Object* pObject);
	C_Object* GetSceneObject(int nIndex);
private:
	int m_nSize;
	C_Object** m_arObjects;
};