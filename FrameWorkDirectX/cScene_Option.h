#pragma once
#include "cScene.h"
class cTerrain;
class cScene_Option : public cScene
{
private:
	cTerrain*		m_pTerrain;

public:
	cScene_Option();
	~cScene_Option();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timDelta);
	virtual void Scene_Render1();
};

