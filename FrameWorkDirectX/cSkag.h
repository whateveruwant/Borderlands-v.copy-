#pragma once
#include "cEnemy.h"
class cSkag : public cEnemy
{
private:

public:
	cSkag();
	~cSkag();

	virtual void Init(D3DXVECTOR2 xz);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void ChangeState(eEnemyState currentState);
	virtual void ActionByState(float timeDelta);
};

