#include "stdafx.h"
#include "cBattleDroid.h"
#include "cTransform.h"

cBattleDroid::cBattleDroid()
{
}


cBattleDroid::~cBattleDroid()
{
}

void cBattleDroid::Init(D3DXVECTOR2 xz)
{
	cEnemy::Init(xz);
	monsterHeight = 15;
	//여기서부터 추가
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -1 * ONE_RAD * 90.0f);
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.1f, 0.1f, 0.1f);
	D3DXMATRIXA16 result = matWolrd*matCorrection;
	mFilePath = "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X";
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);

	//박스
	monsterbox = new cBoundBox();
	monsterbox->SetBound(&D3DXVECTOR3(0.0f, 10.0f, 0.0f), &D3DXVECTOR3(6.0f, 11.0f, 6.0f));

	bodybox = new cBoundBox();
	bodybox->SetBound(&D3DXVECTOR3(0.0f, 15.0f, 0.0f), &D3DXVECTOR3(5.0f, 4.0f, 3.0f));

	boneHead = new cTransform();
	cBaseObject::pSkinned->AddBoneTransform("Eye", boneHead);
	headbox = new cBoundBox();
	headbox->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(1.5f, 1.5f, 1.5f));

	LboneHand = new cTransform();
	cBaseObject::pSkinned->AddBoneTransform("L_Hand", LboneHand);
	Lhandbox = new cBoundBox();
	Lhandbox->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(2.0f, 2.0f, 2.0f));

	//발
	Llegbox = new cBoundBox();
	Llegbox->SetBound(&D3DXVECTOR3(3.5f, 6.0f, 0.0f), &D3DXVECTOR3(1.5f, 5.0f, 1.5f));
	Rlegbox = new cBoundBox();
	Rlegbox->SetBound(&D3DXVECTOR3(-3.5f, 6.0f, 0.0f), &D3DXVECTOR3(1.5f, 5.0f, 1.5f));

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
	mRunSpeed = BATTLEDROID_SPEED;				//스피드
	monsterHP = BATTLEDROID_HP;					//체력
	monsterMaxHP = monsterHP;
	monsterDmg = BATTLEDROID_DMG;				//데미지
	checkOneAttack = false;						//아직 한번 공격안함
	monsterEXP = 20;							//battledroid경험치
}

void cBattleDroid::Update(float timeDelta)
{
	cEnemy::Update(timeDelta);
	this->ActionByState(timeDelta);
	monsterDmg = BATTLEDROID_DMG;
}

void cBattleDroid::Release()
{
	cEnemy::Release();
	//여기서부터 추가
	SAFE_DELETE(monsterbox);
	SAFE_DELETE(bodybox);
	SAFE_DELETE(headbox);
	SAFE_DELETE(Lhandbox);
	SAFE_DELETE(Llegbox);
	SAFE_DELETE(Rlegbox);
}

void cBattleDroid::Render()
{
	cEnemy::Render();
	//여기서부터 추가

	if (KEY_MGR->IsStayDown(VK_F1))
	{
		monsterbox->RenderGizmo(pTransform);
		bodybox->RenderGizmo(pTransform);
		headbox->RenderGizmo(boneHead);
		appreciateRangeSphere->RenderGizmo(pTransform);
		attackRangeSphere->RenderGizmo(pTransform);
		Lhandbox->RenderGizmo(LboneHand);//왼손
		Llegbox->RenderGizmo(pTransform);//왼다리
		Rlegbox->RenderGizmo(pTransform);//오른다리
		pTransform->RenderGimozo();
	}
}

void cBattleDroid::RenderSprite()
{
	cEnemy::RenderSprite();
	if (isFontSee && isLive)
	{
		int hpnum = 259;
		RECT rcHp = { 0 ,0 ,hpnum / monsterMaxHP * monsterHP, 20 };
		RECT rcBackHp = { 0, 0, hpnum, 20 };
		SPRITE_MGR->DrawTexture(monsterHPendText, &rcBackHp, WINSIZE_X / 2 - 25, WINSIZE_Y / 2 - 130, 0.5, 1.0, 0);
		SPRITE_MGR->DrawTexture(monsterHpText, &rcHp, WINSIZE_X / 2 - 25, WINSIZE_Y / 2 - 130, 0.63, 1.0, 0);
	}
}

void cBattleDroid::RenderFont()
{
	//폰트출력
	if (isFontSee && isLive)
	{
		DXFONT_MGR->PrintStyleText("playerUI", "BattleDroid", WINSIZE_X / 2 - 30, WINSIZE_Y / 2 - 100, 0xffffffff);
		if (KEY_MGR->IsStayDown(VK_F1))
		{
			string monsterHpText = to_string(monsterHP);
			DXFONT_MGR->PrintStyleTextOutline("playerUI", monsterHpText, WINSIZE_X / 2 - 65, WINSIZE_Y / 2 - 135, 0Xffffffff);
		}
	}
}

void cBattleDroid::ChangeState(eEnemyState currentState)
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
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[2]->GetName(), 0.3f);
		break;
	case eEnemyState_Run:
		SOUND_MGR->Play3DSound("Droid인식", this->pTransform->GetWorldPosition(), 0.8f);
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[3]->GetName(), 0.3f);
		break;
	case eEnemyState_Attack:
		isOneAttack = false;
		SOUND_MGR->Play3DSound("Droid공격", this->pTransform->GetWorldPosition(), 0.5f);
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[4]->GetName(), 0.0f);
		break;
	case eEnemyState_Hitted:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[8]->GetName(), 0.0f);
		break;
	case eEnemyState_Die:
		SOUND_MGR->Play3DSound("Droid죽음", this->pTransform->GetWorldPosition(), 0.5f);
		cBaseObject::GetSkinned()->PlayOneShotAfterHold(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[10]->GetName(), 1.0f);
		break;
	}
	/*
	10 Death_Var2
	9 knockback_Recover
	8 Hardflinch_B_var1
	7 cirnge_suicide(자폭)
	6 ciritical_eye(총공격)
	5 melee
	4 melee_L(왼손공격)
	3 sprint_F
	2 patrol_walk_F
	1 spawn_unbox
	0 idle
	*/
}

void cBattleDroid::ActionByState(float timeDelta)
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
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[8]->GetName(), 0.0f);
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
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere))
				ChangeState(eEnemyState_Idle);
			else {
				if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerBoundBox, this->pTransform, this->monsterbox)) {
					isOneAttack = false;
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[4]->GetName(), 0.0f);
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

		if (!activeEnd)//추가
		{
			pTransform->MovePositionWorld(D3DXVECTOR3(0, activeEndPosY, 0));
			if (pTransform->GetWorldPosition().y <  this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z) - 13.0f)
			{
				activeEnd = true;
				activeEndPosY = 0.0f;
				break;
			}
		}
		break;
	}
}
