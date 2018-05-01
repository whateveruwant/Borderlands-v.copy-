#include "stdafx.h"
#include "cSkag.h"
#include "cTransform.h"



cSkag::cSkag()
{
}


cSkag::~cSkag()
{
}

void cSkag::Init(D3DXVECTOR2 xz)
{
	cEnemy::Init(xz);
	monsterHeight = 5;
	//여기서부터 추가
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -1 * ONE_RAD * 90.0f);
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.1f, 0.1f, 0.1f);
	D3DXMATRIXA16 result = matWolrd * matCorrection;

	mFilePath = "../Resources/Meshes/enemy/Skag/Skag(final).X";
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);

	//박스
	monsterbox = new cBoundBox();
	monsterbox->SetBound(&D3DXVECTOR3(0.0f, 5.0f, 0.0f), &D3DXVECTOR3(6.0f, 6.0f, 9.0f));

	bodybox = new cBoundBox();
	bodybox->SetBound(&D3DXVECTOR3(0.0f, 5.0f, 0.0f), &D3DXVECTOR3(5.0f, 5.0f, 5.0f));

	boneHead = new cTransform();
	cBaseObject::pSkinned->AddBoneTransform("Head", boneHead);
	headbox = new cBoundBox();
	headbox->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(2.0f, 2.0f, 2.0f));

	LboneHand = new cTransform();
	cBaseObject::pSkinned->AddBoneTransform("LHand", LboneHand);
	Lhandbox = new cBoundBox();
	Lhandbox->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(3.0f, 3.0f, 3.0f));

	//스페어 박스
	appreciateRangeSphere = new cBoundSphere();
	appreciateRangeSphere->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(50.0f, 50.0f, 50.0f));

	attackRangeSphere = new cBoundSphere();
	attackRangeSphere->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(10.0f, 10.0f, 10.0f));

	//상태
	mCurrentState = eEnemyState_Idle;
	savecurrentState = mCurrentState;
	cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0.3f);

	//
	mRunSpeed = SKAG_SPEED;				//스피드
	monsterHP = SKAG_HP;				//체력
	monsterMaxHP = monsterHP;
	monsterDmg = SKAG_DMG;				//데미지
	checkOneAttack = false;				//아직 한번 공격안함
	monsterEXP = 10;					//skag경험치
}

void cSkag::Update(float timeDelta)
{
	cEnemy::Update(timeDelta);
	this->ActionByState(timeDelta);
	monsterDmg = SKAG_DMG;
}

void cSkag::Release()
{
	cEnemy::Release();
	//여기서부터 추가
	SAFE_DELETE(monsterbox);
	SAFE_DELETE(bodybox);
	SAFE_DELETE(headbox);
	SAFE_DELETE(Lhandbox);
}

void cSkag::Render()
{
	cEnemy::Render();
	pTransform->RenderGimozo();
	//여기서부터 추가

	if (KEY_MGR->IsStayDown(VK_F1))
	{
		//박스 렌더
		monsterbox->RenderGizmo(pTransform);
		bodybox->RenderGizmo(pTransform);
		headbox->RenderGizmo(boneHead);
		appreciateRangeSphere->RenderGizmo(pTransform);
		attackRangeSphere->RenderGizmo(pTransform);
		Lhandbox->RenderGizmo(LboneHand);//손
		pTransform->RenderGimozo();
	}
}

void cSkag::RenderSprite()
{
	cEnemy::RenderSprite();
	if (isFontSee && isLive)
	{
		int hpnum = 259;
		RECT rcHp = { 0 ,0 ,hpnum / monsterMaxHP * monsterHP, 20 };
		RECT rcBackHp = { 0, 0, hpnum, 20 };
		SPRITE_MGR->DrawTexture(monsterHPendText, &rcBackHp, WINSIZE_X / 2 - 60, WINSIZE_Y / 2 - 130, 0.5, 1.0, 0);
		SPRITE_MGR->DrawTexture(monsterHpText, &rcHp, WINSIZE_X / 2 - 60, WINSIZE_Y / 2 - 130, 0.623, 1.0, 0);
	}
}

void cSkag::RenderFont()
{
	//폰트출력
	if (isFontSee && isLive)
	{
		DXFONT_MGR->PrintStyleText("playerUI", "Skag", WINSIZE_X / 2 - 30, WINSIZE_Y / 2 - 100, 0xffffffff);
		if (KEY_MGR->IsStayDown(VK_F1))
		{
			string monsterHpText = to_string(monsterHP);
			DXFONT_MGR->PrintStyleTextOutline("playerUI", monsterHpText, WINSIZE_X / 2 - 100, WINSIZE_Y / 2 - 135, 0Xffffffff);
		}
	}
}

void cSkag::ChangeState(eEnemyState currentState)
{
	if (mCurrentState != currentState)
		mCurrentState = currentState;
	else
		return;

	switch (currentState)
	{
	case eEnemyState_Idle:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0.3f);
		break;
	case eEnemyState_Follow:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[1]->GetName(), 0.3f);
		break;
	case eEnemyState_Run:
		SOUND_MGR->Play3DSound("Skag인식", this->pTransform->GetWorldPosition(), 0.3f);
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[2]->GetName(), 0.3f);
		break;
	case eEnemyState_Attack:
		isOneAttack = false;
		SOUND_MGR->Play3DSound("Skag공격", this->pTransform->GetWorldPosition(), 0.3f);
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[3]->GetName(), 0.0f);
		break;
	case eEnemyState_Hitted:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[6]->GetName(), 0.0f);
		break;
	case eEnemyState_Die:
		SOUND_MGR->Play3DSound("Skag죽음", this->pTransform->GetWorldPosition(), 0.3f);
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->PlayOneShotAfterHold(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[9]->GetName(), 0.0f);
		break;
	}

	/*
	9 죽는거 Death_Corrosive_var1
	8 리스폰 spawn_01
	7 처맞는거 Knockback_Recover
	6 Hardfinch_head
	5 물뜯는 Attack_Bite_2
	4 Attack_Bite_1
	3 손공격 Attack_Claw_L
	2 뛰기 run_F_Big1
	1 걷기 Patrol_Walk_F
	0 idle
	*/
}

void cSkag::ActionByState(float timeDelta)
{
	//playerEnemyCalPos = pPlayerTransform->GetWorldPosition()- pTransform->GetWorldPosition();
	//resultLength = D3DXVec3Length(&playerEnemyCalPos);

	if (isAttackted != 0 && isLive) {
		monsterHP -= isAttackted;
		isAttackted = 0;
		if (monsterHP <= 0) {
			monsterHP = 0;
			pPlayer->SetExd(monsterEXP);
			ChangeState(eEnemyState_Die);
			isLive = false;
			return;
		}
		else if (mCurrentState == eEnemyState_Idle) {
			compulsion = true;
			ChangeState(eEnemyState_Run);
			return;
		}
		else {
			if (headshotCheck) {
				if (mCurrentState == eEnemyState_Hitted)
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[6]->GetName(), 0.0f);
				else
					ChangeState(eEnemyState_Hitted);
				headshotCheck = false;
				return;
			}
		}
	}

	switch (mCurrentState)
	{
	case eEnemyState_Idle:
		respawntime = 0;//리스폰타임 초기화
		pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z), pTransform->GetWorldPosition().z);
		if (CheckAppreciate() || compulsion)
			ChangeState(eEnemyState_Run);
		break;
	case eEnemyState_Follow:
		cEnemy::SeePlayer();
		cEnemy::moveDirPlayer(timeDelta);
		break;
	case eEnemyState_Run:
		pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z), pTransform->GetWorldPosition().z);
		cEnemy::SeePlayer();
		cEnemy::moveDirPlayer(timeDelta);
		if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerBoundBox, this->pTransform, this->monsterbox))
			ChangeState(eEnemyState_Attack);
		else if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere) && !compulsion)
			ChangeState(eEnemyState_Idle);
		break;
	case eEnemyState_Attack:
		pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z), pTransform->GetWorldPosition().z);
		cEnemy::SeePlayer();
		//this->monsterLAttack = PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerBoundBox, this->pTransform, this->Lhandbox);//왼손공격 충돌 
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere))
				ChangeState(eEnemyState_Idle);
			else {
				if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerBoundBox, this->pTransform, this->monsterbox)) {
					isOneAttack = false;
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[3]->GetName(), 0.0f);
					return;
				}
				ChangeState(eEnemyState_Run);
			}
		}
		break;
	case eEnemyState_Hitted:
		pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z), pTransform->GetWorldPosition().z);
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere))
				ChangeState(eEnemyState_Idle);
			else {
				if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerBoundBox, this->pTransform, this->monsterbox)) {
					ChangeState(eEnemyState_Attack);
					return;
				}
				ChangeState(eEnemyState_Run);
			}
		}
		break;

	case eEnemyState_Die:
		_cMap->SetAstarTile(StartX, StartZ, false);
		_cMap->SetAstarTile(StartX - 1, StartZ - 1, false);
		_cMap->SetAstarTile(StartX - 1, StartZ, false);
		_cMap->SetAstarTile(StartX - 1, StartZ + 1, false);
		_cMap->SetAstarTile(StartX, StartZ - 1, false);
		_cMap->SetAstarTile(StartX, StartZ + 1, false);
		_cMap->SetAstarTile(StartX + 1, StartZ - 1, false);
		_cMap->SetAstarTile(StartX + 1, StartZ, false);
		_cMap->SetAstarTile(StartX + 1, StartZ + 1, false);

		if (cBaseObject::GetSkinned()->Gethold()) {
			activeEnd = false;
		}
		if (!activeEnd)//추가
		{
			pTransform->MovePositionWorld(D3DXVECTOR3(0, activeEndPosY, 0));
			if (pTransform->GetWorldPosition().y <  this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z) - 7.0f)
			{
				activeEnd = true;
				activeEndPosY = 0.0f;
				break;
			}
		}
		break;
	}
}




