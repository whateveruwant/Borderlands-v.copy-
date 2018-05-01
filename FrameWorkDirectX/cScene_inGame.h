#pragma once
#include "cScene.h"
#include "Character.h"
#include "cMap.h"
#include "cObject.h"
#include "cEnemy.h"
#include "cItem.h"
#include "cThresher.h"
class cScene_inGame :public cScene
{
private:
	Character*			mCharacter;

	vector<cEnemy*>		vEnemies;
	vector<cEnemy*>		vCullingEnemies;

	vector<cObject*>	vObjects;
	vector<cObject*>	vCullingObjects;

	vector<cItem*>		vItems;
	vector<cItem*>		vCullingItems;

	cMap*				mMap;

	cThresher*			mBoss;
	cTransform*			mBossTriggerTransform;
	cBoundBox			mBossTriggerBoundBox;

public:
	cScene_inGame();
	~cScene_inGame();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render1();
	virtual void Scene_RenderSprite();
	virtual void Scene_RenderFont();
	void RenderShadow();

public:  // For Boss
	void InitForBoss();

public:
	void Culling(float timeDelta);
	void MoveMap(bool isTown);
	void MoveMapForDie(bool isTown);


public:
	void InitParticle();
	void SetupSounds();

	void LoadObjects(string fileName);
	void LoadEnemys(string fileName);
};