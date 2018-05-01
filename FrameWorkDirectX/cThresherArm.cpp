#include "stdafx.h"
#include "cThresherArm.h"
#include "cThresher.h"

cThresherArm::cThresherArm()
{
}


cThresherArm::~cThresherArm()
{
}

void cThresherArm::Init(D3DXVECTOR2 xz, bool is_arm)
{
	cBossManager::Init(xz);
	LoadMesh();
	if (is_arm)
		pTransform->SetWorldPosition(xz.x + 50, -150, xz.y + 50);
	else
		pTransform->SetWorldPosition(xz.x - 50, -150, xz.y + 50);

	is_Arm = is_arm;
	is_atk = true;
	armState = ARM_DOWN;
	saveState = armState;
	attacknum = 0;

	RandPos = D3DXVECTOR3(0, 0, 0);
	attacktime = RandomFloatRange(2, 4);


	/////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 4; i++)
	{
		transbon[i] = new cTransform();
	}
	cBaseObject::pSkinned->AddBoneTransform("Tentacle_End", transbon[0]);
	cBaseObject::pSkinned->AddBoneTransform("ArmBone_12", transbon[1]);
	cBaseObject::pSkinned->AddBoneTransform("ArmBone_9", transbon[2]);
	cBaseObject::pSkinned->AddBoneTransform("ArmBone_5", transbon[3]);

	dmgBox = new cBoundBox;
	dmgBox->SetBound(&D3DXVECTOR3(0, 15, 0), &D3DXVECTOR3(5, 30, 5));
	/////////////////////////////////////////////////////////////////////////////
	cBaseObject::GetSkinned()->SetPlaySpeed(3);
	cBaseObject::GetSkinned()->Play("Death_Var1", 1);
}

void cThresherArm::LoadMesh()
{
	//mFilePath = "../Resources/Meshes/Boss/Thresher_arm1.X";

	mFilePath = "../Resources/Meshes/Boss/bossarm.X";
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -90 * ONE_RAD);
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.25f, 0.25f, 0.25f);
	D3DXMATRIXA16 result = matWolrd*matCorrection;
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);
}

void cThresherArm::Update(float timeDelta)
{
	cBossManager::Update(timeDelta);
	ActionByState(timeDelta);

}

void cThresherArm::Release()
{
	cBossManager::Release();
}

void cThresherArm::Render()
{
	cBossManager::Render();
	//for (int i = 0; i < 4; i++)
	//{
	//	dmgBox->RenderGizmo(transbon[i]);
	//}

}

void cThresherArm::RenderSprite()
{
	cBossManager::RenderSprite();
}

void cThresherArm::RenderFont()
{
	cBossManager::RenderFont();
}

void cThresherArm::RenderShadow()
{
	cBossManager::RenderShadow();
}

void cThresherArm::ActionByState(float timeDelta)
{
	Ray ray;
	ray.origin = pTransform->GetWorldPosition();
	ray.origin.y += 5;
	ray.direction = pTransform->GetForward();

	switch (armState)
	{
	case ARM_IDEL:
		if (GetSkinned()->GetEnd())
			armState = ARM_DOWN;
		break;
	case ARM_DOWN:
		if (attacktime <0)
		{
			attacktime = RandomFloatRange(3, 6);
			armState = ARM_UP;
			pTransform->SetWorldPosition(pCore->pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().y, pCore->pTransform->GetWorldPosition().z);
			pTransform->SetRotateWorld(pCore->pTransform->GetWorldRotateQuaternion());

			D3DXVECTOR3 len = pTransform->GetWorldPosition() - pPlayer->tranCharacter->GetWorldPosition();
			if (is_Arm)
				pTransform->MovePositionSelf(RandomFloatRange(20, 40), 0, 0);
			else
				pTransform->MovePositionSelf(RandomFloatRange(-20, -40), 0, 0);


			float length = D3DXVec3Length(&len);


			if (length > 30)
			{
				pTransform->MovePositionSelf(0, 0, RandomFloatRange(length / 3, length / 2));
			}
			if (pMap->GetpTerrain()->GetHeight(RandPos.x, RandPos.z) <= pTransform->GetWorldPosition().y)
			{
				pTransform->SetWorldPosition(RandPos);
			}
		}
		else
		{
			attacktime -= timeDelta;
		}
		if (pTransform->GetWorldPosition().y > -170)
			pTransform->MovePositionWorld(0, -timeDelta * 110, 0);
		else if (pTransform->GetWorldPosition().y <= -170)
			pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, -170, pTransform->GetWorldPosition().z);

		break;
	case ARM_UP:
		attacknum = RandomIntRange(0, 3);
		if (pTransform->GetWorldPosition().y < pMap->GetpTerrain()->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z) - 7)
		{
			pTransform->MovePositionWorld(0, timeDelta * 150, 0);
		}
		else
		{
			if (PHYSICS_MGR->IsRayHitBound(&ray, &pPlayer->tranChaBoundBox, pPlayer->tranCharacter, &hitpos, NULL))
			{
				armState = ARM_ATTACK;
			}
			else
			{
				armState = ARM_TURN;
			}
		}
		break;
	case ARM_ATTACK:

		if (is_atk)
		{
			for (int i = 0; i < 4; i++)
			{
				if (PHYSICS_MGR->IsOverlap(pPlayer->tranCharacter, &pPlayer->tranChaBoundBox, this->transbon[i], this->dmgBox))
				{
					SOUND_MGR->Play3DSound("촉수타격", pTransform->GetWorldPosition());
					SOUND_MGR->update();
					pPlayer->hitCharacter(50);
					is_atk = false;
					break;
				}
			}
		}

		if (GetSkinned()->GetEnd())
		{
			armState = ARM_DOWN;
			is_atk = true;
		}
		break;
	case ARM_LOOK:
		if (GetSkinned()->GetEnd())
			armState = ARM_TURN;
		break;
	case ARM_TURN:
		pTransform->LookPosition(pPlayer->tranCharacter->GetWorldPosition(), 5 * ONE_RAD);
		pTransform->CorrectionUp(D3DXVECTOR3(0, 1, 0), 5 * ONE_RAD);
		pTransform->MovePositionWorld(0, 0, 10 * timeDelta);
		if (PHYSICS_MGR->IsRayHitBound(&ray, &pPlayer->tranChaBoundBox, pPlayer->tranCharacter, &hitpos, NULL))
		{
			armState = ARM_ATTACK;
		}
		break;
	case ARM_DIE:
		if (pTransform->GetWorldPosition().y > -170)
			pTransform->MovePositionWorld(0, -timeDelta * 110, 0);
		break;
	}
	ChangeState();
}

void cThresherArm::ChangeState()
{
	if (saveState != armState)
		saveState = armState;
	else
		return;

	/*
	SOUND_MGR->add3DSound("보스출현", "../Resources/Sounds/BossSound/보스출현.mp3", 0, 500);
	SOUND_MGR->add3DSound("보스죽을때", "../Resources/Sounds/BossSound/보스죽을때.mp3", 0, 500);

	SOUND_MGR->add3DSound("촉수경고음.", "../Resources/Sounds/BossSound/촉수경고음.mp3", 0, 500);
	SOUND_MGR->add3DSound("촉수나올때", "../Resources/Sounds/BossSound/촉수나올때.mp3", 0, 500);
	SOUND_MGR->add3DSound("촉수죽을때", "../Resources/Sounds/BossSound/촉수죽을때.mp3", 0, 500);
	SOUND_MGR->add3DSound("보스미사일", "../Resources/Sounds/BossSound/보스미사일.mp3", 0, 500);*/


	float CrossFade = 0.3;
	//cBaseObject::GetSkinned()->SetPlaySpeed(1);
	switch (armState)
	{
	case ARM_IDEL:
		cBaseObject::GetSkinned()->PlayOneShot("Idle1", CrossFade);
		break;
	case ARM_DOWN:
		cBaseObject::GetSkinned()->PlayOneShot("Burrow", CrossFade);
		break;
	case ARM_UP:
		SOUND_MGR->Play3DSound("촉수나올때", pTransform->GetWorldPosition());
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->Play("Exit", CrossFade);
		break;
	case ARM_ATTACK:
	{
		//int num =RandomIntRange(0,2);
		switch (attacknum)
		{
		case 0:
			pTransform->RotateWorld(25 * ONE_RAD, 0, 0);
			cBaseObject::GetSkinned()->PlayOneShot("attack1", CrossFade);
			break;
		case 1:
			pTransform->RotateWorld(25 * ONE_RAD, 0, 0);
			cBaseObject::GetSkinned()->PlayOneShot("attack2", CrossFade);
			break;
		case 2:
			cBaseObject::GetSkinned()->PlayOneShot("attack4", CrossFade);
			break;
		case 3:
			cBaseObject::GetSkinned()->PlayOneShot("attack3", CrossFade);
			break;
		default:
			break;
		}
	}break;
	case ARM_LOOK:
		cBaseObject::GetSkinned()->PlayOneShot("Idle2", CrossFade);
		break;
	case ARM_TURN:
		cBaseObject::GetSkinned()->Play("Idle1", CrossFade);
		break;
	case ARM_DIE:
		cBaseObject::GetSkinned()->Play("Burrow", CrossFade);
		break;
	}
}
