#pragma once
#include "cEnemy.h"

class cBandit : public cEnemy
{
private:
	int banditTimeAttack;//총알 나갈때 제어
public:
	cBandit();
	~cBandit();

	virtual void Init(D3DXVECTOR2 xz);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void ChangeState(eEnemyState currentState);
	virtual void ActionByState(float timeDelta);
};

