#include "stdafx.h"
#include "cBossManager.h"
#include "cTransform.h"
#include "cBoundBox.h"
#include "cBoundSphere.h"
#include "cSkinnedAnimation.h"

cBossManager::cBossManager()
{
}

cBossManager::~cBossManager()
{
}

void cBossManager::Init(D3DXVECTOR2 xz)
{
}

void cBossManager::Update(float timeDelta)
{
	cBaseObject::Update(timeDelta);
}

void cBossManager::Release()
{
	SAFE_DELETE(attackRangeSphere);
}

void cBossManager::Render()
{
	cBaseObject::Render();
}

void cBossManager::RenderSprite()
{
}

void cBossManager::RenderFont()
{
}

void cBossManager::RenderShadow()
{
	cBaseObject::RenderShadow();
}

void cBossManager::SeePlayer()
{
	D3DXVECTOR3 tmp = { pPlayer->GetpPlayerXZTransform()->GetWorldPosition().x, this->pTransform->GetWorldPosition().y, pPlayer->GetpPlayerXZTransform()->GetWorldPosition().z };
	pTransform->LookPosition(tmp, 5.0f*ONE_RAD);
}

void cBossManager::moveDirPlayer(float timeDelta)
{
	////거리를 구한다.
	D3DXVECTOR3 dirToTarget = pPlayer->GetpPlayerXZTransform()->GetWorldPosition() - this->pTransform->GetWorldPosition();
	float dist = D3DXVec3Length(&dirToTarget);

	//이동량
	float deltaMove = speed * timeDelta;
	float t = Clamp01(deltaMove / dist);

	D3DXVECTOR3 pos = VecLerp(this->pTransform->GetWorldPosition(), pPlayer->GetpPlayerXZTransform()->GetWorldPosition(), t);

	//높이 얻는다.
	pos.y = this->pMap->GetpTerrain()->GetHeight(pos.x, pos.z);
	this->pTransform->SetWorldPosition(pos);

	//중력처리
	D3DXVECTOR3 sliding;
	pMap->GetpTerrain()->GetSlant(&sliding, 9.8f, pos.x, pos.z);

	this->pTransform->MovePositionWorld(sliding * timeDelta);

}
