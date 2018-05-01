#pragma once
#include "cSingletonBase.h"
#include "cTerrain.h"
#include "cParticleBullet.h"
#include "cEnemy.h"
#include "Character.h"
#include "cThresher.h"
#include "cObject.h"

class cBulletCollisionMGR : public cSingletonBase<cBulletCollisionMGR>
{
private:
	cThresher*						pBoss;
	cTerrain*						pMapInfo;
	Character*						pPlayer;
	vector<cEnemy*>					pEnemys;
	vector<cObject*>				pObjects;

public:
	cParticleBullet*				pPlayerBullets;
	cParticleBullet*				pEnemyBullets;
	cParticleBullet*				pBossBullets;


public:
	cBulletCollisionMGR();
	~cBulletCollisionMGR();

	HRESULT Init();
	void Release();
	void Update(float timeDelta);
	void Render();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SetTerrain(cTerrain* pMapInfo) { this->pMapInfo = pMapInfo; }
	void SetPlayer(Character* player) { pPlayer = player; }
	void AddEnemy(cEnemy* pEnemy) { pEnemys.push_back(pEnemy); }
	void AddObejct(cObject* pObject) { pObjects.push_back(pObject); }
	void SetBoss(cThresher*	_pBoss) { pBoss = _pBoss; }
	void ClearBoss() { SAFE_DELETE(pBoss); }
	void ClearEnemys() { pEnemys.clear(); }
	void ClearObjects() { pObjects.clear(); }

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void FirePlayer();
	void FireEnemy();
	void FireBoss();

	void CheckEnemyNearAtkAndPlayer();			//근거리 추가
	bool CheckPlayerOneBullet(int index);
	bool CheckEnemyOneBullet(int index);
	bool CheckBossOneBullet(int index);


	void WhereHit(D3DXVECTOR3 hitting, D3DXVECTOR3 hittingForward = {0,0,0});
};

#define COLLISIONMGR cBulletCollisionMGR::GetInstance()