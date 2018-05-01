#include "stdafx.h"
#include "cThresherTail.h"
#include "cThresher.h"

cThresherTail::cThresherTail()
{
}


cThresherTail::~cThresherTail()
{
}

void cThresherTail::Init(D3DXVECTOR2 xz)
{
	cBossManager::Init(xz);
	pTransform->SetWorldPosition(xz.x, 0, xz.y - 75);

	firetime = RandomFloatRange(10, 20);
	range = RandomFloatRange(30, 40);

	State = TAIL_IDLE;
	SaveState = State;
	bulletNum = 1;
	LoadMesh();
	trans = new cTransform;
	transbon = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("Needle", transbon);
	bulletfiretime = 0.6;
}

void cThresherTail::LoadMesh()
{
	mFilePath = "../Resources/Meshes/Boss/Thresher_arm2.X";
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -90 * ONE_RAD);
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.25f, 0.25f, 0.25f);
	D3DXMATRIXA16 result = matWolrd*matCorrection;
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);
}

void cThresherTail::Update(float timeDelta)
{
	cBossManager::Update(timeDelta);
	ActionByState(timeDelta);
}

void cThresherTail::Release()
{
	cBossManager::Release();
}

void cThresherTail::Render()
{
	cBossManager::Render();
	//GIZMO_MGR->WireSphere(transbon->GetWorldPosition(), 20);
}

void cThresherTail::RenderSprite()
{
	cBossManager::RenderSprite();
}

void cThresherTail::RenderFont()
{
	cBossManager::RenderFont();
}

void cThresherTail::RenderShadow()
{
	cBossManager::RenderShadow();
}

void cThresherTail::ActionByState(float timeDelta)
{
	Ray ray;
	ray.origin = pTransform->GetWorldPosition();
	ray.origin.y += 5;
	ray.direction = pTransform->GetForward();

	switch (State)
	{
		break;
	case TAIL_IDLE:
		pTransform->SetWorldPosition(pCore->pTransform->GetWorldPosition().x, pCore->pTransform->GetWorldPosition().y, pCore->pTransform->GetWorldPosition().z);
		pTransform->SetRotateWorld(pCore->pTransform->GetWorldRotateQuaternion());
		pTransform->MovePositionSelf(0, 0, -75);
		firetime -= timeDelta;
		if (firetime < 0)
		{
			State = TAIL_FIRESTART;
		}
		break;
	case TAIL_FIRESTART:
		bulletfiretime -= timeDelta;
		if (bulletfiretime < 0)
		{
			bulletfiretime = 0.2;
			trans->SetWorldPosition(transbon->GetWorldPosition());
			trans->LookPosition(pPlayer->tranCharacter->GetWorldPosition());
			D3DXVECTOR3 len = trans->GetWorldPosition() - pPlayer->tranCharacter->GetWorldPosition();

			COLLISIONMGR->pBossBullets->speed2 = 1;
			COLLISIONMGR->pBossBullets->randAngleX = 200;
			COLLISIONMGR->pBossBullets->randAngleY = 200;
			COLLISIONMGR->pBossBullets->range = D3DXVec3Length(&len);
			COLLISIONMGR->pBossBullets->mTransform = trans;
			trans->MovePositionWorld(0, 0, 10);
			COLLISIONMGR->FireBoss();
			SOUND_MGR->Play3DSound("보스미사일", trans->GetWorldPosition());

			COLLISIONMGR->pBossBullets->speed2 = 1;
			COLLISIONMGR->pBossBullets->randAngleX = -200;
			COLLISIONMGR->pBossBullets->randAngleY = 200;
			COLLISIONMGR->pBossBullets->range = D3DXVec3Length(&len);
			COLLISIONMGR->pBossBullets->mTransform = trans;
			COLLISIONMGR->FireBoss();
			SOUND_MGR->Play3DSound("보스미사일", trans->GetWorldPosition());
			SOUND_MGR->update();

			COLLISIONMGR->pBossBullets->speed2 = 1;
			COLLISIONMGR->pBossBullets->randAngleX = -200;
			COLLISIONMGR->pBossBullets->randAngleY = 0;
			COLLISIONMGR->pBossBullets->range = D3DXVec3Length(&len);
			COLLISIONMGR->pBossBullets->mTransform = trans;
			COLLISIONMGR->FireBoss();
			SOUND_MGR->Play3DSound("보스미사일", trans->GetWorldPosition());
			SOUND_MGR->update();

			COLLISIONMGR->pBossBullets->speed2 = 1;
			COLLISIONMGR->pBossBullets->randAngleX = 200;
			COLLISIONMGR->pBossBullets->randAngleY = 0;
			COLLISIONMGR->pBossBullets->range = D3DXVec3Length(&len);
			COLLISIONMGR->pBossBullets->mTransform = trans;
			COLLISIONMGR->FireBoss();
			SOUND_MGR->Play3DSound("보스미사일", trans->GetWorldPosition());
			SOUND_MGR->update();

			COLLISIONMGR->pBossBullets->speed2 = 1;
			COLLISIONMGR->pBossBullets->randAngleX = 0;
			COLLISIONMGR->pBossBullets->randAngleY = 200;
			COLLISIONMGR->pBossBullets->range = D3DXVec3Length(&len);
			COLLISIONMGR->pBossBullets->mTransform = trans;
			COLLISIONMGR->FireBoss();
			SOUND_MGR->Play3DSound("보스미사일", trans->GetWorldPosition());
			SOUND_MGR->update();
		}
		if (GetSkinned()->GetEnd())
		{
			bulletfiretime = 0.6;
			bulletNum--;
			if (bulletNum == 0)
			{
				State = TAIL_IDLE;
				bulletNum = RandomIntRange(1, 3);
				firetime = RandomFloatRange(5, 7);
			}
			else
			{
				State = TAIL_IDLE;
				firetime = 1.0;
			}
		}
		break;
	case TAIL_DIE:
		if (pTransform->GetWorldPosition().y > -150)
			pTransform->MovePositionWorld(0, -timeDelta * 110, 0);
		break;
	}

	ChangeState();
	//COLLISIONMGR->pEnemyBullets->mTransform;// = LboneHand;
	//COLLISIONMGR->FireEnemy();
}

void cThresherTail::ChangeState()
{
	if (SaveState == State)
	{
		return;
	}
	else
	{
		SaveState = State;
	}


	switch (State)
	{
	case TAIL_FIRE:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 1);
		break;
	case TAIL_FIRESTART:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[3]->GetName(), 1);
		break;
	case TAIL_IDLE:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0);
		break;
	case TAIL_DIE:
		break;
	}

}
