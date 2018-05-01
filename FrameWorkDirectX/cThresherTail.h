#pragma once
#include "cBossManager.h"
class cThresher;

enum BossTaliState
{
	TAIL_FIRE,
	TAIL_FIRESTART,
	TAIL_IDLE,
	TAIL_DIE
};
class cThresherTail : public cBossManager
{
private:


	cTransform* transbon;
	cTransform* trans;
	////////////////////////////////////////////////////// << 상호참조용
	cThresher*			pCore;


	BossTaliState State;
	BossTaliState SaveState;

	float bulletfiretime;
	float firetime;
	float range;
	int bulletNum;
	D3DXVECTOR3 hitpos;


public:
	cThresherTail();
	~cThresherTail();

	void SetThresher(cThresher* pThresher) {
		pCore = pThresher;
	}

	virtual void Init(D3DXVECTOR2 xz);
	virtual void LoadMesh();
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void RenderShadow();

	virtual void ActionByState(float timeDelta);
	void ChangeState();
	void SetDie() { State = TAIL_DIE; }

};