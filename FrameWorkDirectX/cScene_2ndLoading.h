#pragma once
#include "cscene.h"

class cBaseObject;
class cCamera;
class cTerrain;
class cSkinnedAnimation;

class cScene_2ndLoading : public cScene
{
private:
	LPDIRECT3DTEXTURE9 tx;

public:
	cScene_2ndLoading(void);
	~cScene_2ndLoading(void);


	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timDelta);
	virtual void Scene_Render1();
	virtual void Scene_RenderSprite();
	virtual void Scene_RenderFont();

};