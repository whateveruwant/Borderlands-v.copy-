#pragma once
#include "cBossManager.h"
enum BOSSARM
{
	ARM_IDEL,
	ARM_DOWN,
	ARM_UP,
	ARM_ATTACK,
	ARM_LOOK,
	ARM_TURN,
	ARM_DIE
};


class cThresher;
class cThresherArm :public cBossManager
{
private:
	float attacktime;

	BOSSARM		armState;
	BOSSARM		saveState;
	D3DXVECTOR3 hitpos;
	D3DXVECTOR3 RandPos;

	cTransform* transbon[4];
	cBoundBox*	dmgBox;

	bool is_Arm;
	bool is_atk;
	int attacknum;

	cThresher*			pCore;

public:
	cThresherArm();
	~cThresherArm();

	void SetThresher(cThresher* pThresher) {
		pCore = pThresher;
	}

	virtual void Init(D3DXVECTOR2 xz, bool is_arm);
	virtual void LoadMesh();
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void RenderShadow();

	virtual void ActionByState(float timeDelta);
	void ChangeState();


	BOSSARM GetState() { return armState; }
	void SetState(BOSSARM test) { armState = test; };
	void SetDie() { armState = ARM_DIE; }

};