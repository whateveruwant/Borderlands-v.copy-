#pragma once
#include "cscene.h"
#include "cMap.h"
#include "cItem.h"
#include "cObject.h"

class cScene_1stLoading : public cScene
{
private:
	cMap*				mMap;
	Character*			mCharacter;

	vector<cObject*>	vObjects;
	vector<cObject*>	vCullingObjects;

	vector<cItem*>		vItems;
	vector<cItem*>		vCullingItems;

public:
	cScene_1stLoading(void);
	~cScene_1stLoading(void);


	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timDelta);
	virtual void Scene_Render1();
	virtual void Scene_RenderSprite();
	virtual void Scene_RenderFont();

	void Culling(float timeDelta);

	void InitParticle();
	void LoadObjects();
};