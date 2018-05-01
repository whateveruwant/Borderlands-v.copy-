#pragma once
#include "cBossManager.h"
#include "cThresherArm.h"
#include "cThresherTail.h"
struct sThresherAttack {
	cTransform*			mAttackTransform;
	cBoundBox			mAttackBoundBox;
};
class cThresher : public cBossManager
{
private:
	LPDIRECT3DTEXTURE9 hpTex;
	LPDIRECT3DTEXTURE9 hp2Tex;
	LPDIRECT3DTEXTURE9 bossStartimg;


	eBossBodyState		mState;

	cTransform*			mBodyTransform;
	cTransform*			mHeadTransform;
	cBoundBox			mBodyBoundBox;
	cBoundBox			mHeadBoundBox;

	vector<sThresherAttack>	mThresherAttacks;

	cThresherArm*		mArm1;
	cThresherArm*		mArm2;
	cThresherTail*		mTail;

	int count;

	bool				mTrigger;
	bool				isOnceAttack;

public:
	cThresher();
	~cThresher();

	cTransform* GetBodyTransform() { return mBodyTransform; }
	cTransform* GetHeadTransform() { return mHeadTransform; }
	cBoundBox	GetBodyBoundBox() { return mBodyBoundBox; }
	cBoundBox	GetHeadBoundBox() { return mHeadBoundBox; }

	vector<sThresherAttack> GetAttacks() { return mThresherAttacks; }
	bool GetTrigger() { return mTrigger; }
	bool GetOnceAttack() { return isOnceAttack; }
	void SetOnceAttack(bool tf) { isOnceAttack = tf; }
	void SetTrigger(bool _tf) { mTrigger = _tf; }
	eBossBodyState GetState() { return mState; }

	virtual void Init(D3DXVECTOR2 xz);
	void InitForTransAndBound();
	void InitForConstant();
	virtual void LoadMesh();
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void RenderShadow();

	virtual void ActionByState(float timeDelta);
	void ChangeState(eBossBodyState currentState);
};