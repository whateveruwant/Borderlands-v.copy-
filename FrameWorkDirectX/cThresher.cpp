#include "stdafx.h"
#include "cThresher.h"
#include "cTransform.h"

cThresher::cThresher()
{
}

cThresher::~cThresher()
{
}

void cThresher::Init(D3DXVECTOR2 xz)
{
	hpTex= RESOURCE_TEXTURE->GetResource("../Resources/ui/bosshp.png");
	hp2Tex =RESOURCE_TEXTURE->GetResource("../Resources/ui/bosshp2.png");
	bossStartimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/bossStart.png");
	cBossManager::Init(xz);
	LoadMesh();
	InitForConstant();
	InitForTransAndBound();
	D3DXVECTOR3 heightCorrect = D3DXVECTOR3(xz.x, pMap->GetpTerrain()->GetHeight(xz.x, xz.y) - 10, xz.y);
	pTransform->SetWorldPosition(heightCorrect);
	pTransform->RotateWorld(0, 45 * ONE_RAD, 0);
	mArm1 = new cThresherArm;
	mArm1->SetpMap(pMap);
	mArm1->SetpPlayer(pPlayer);
	mArm1->Init({ xz }, true);
	mArm1->SetThresher(this);
	mArm2 = new cThresherArm;
	mArm2->SetpMap(pMap);
	mArm2->SetpPlayer(pPlayer);
	mArm2->Init({ xz }, false);
	mArm2->SetThresher(this);

	cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[2]->GetName(), 0.0f);
}

void cThresher::InitForTransAndBound()
{
	mBodyTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("Spine03", mBodyTransform);
	mBodyBoundBox.SetBound(&D3DXVECTOR3(0, -10, 0), &D3DXVECTOR3(10, 40, 10));
	mHeadTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("Head", mHeadTransform);
	mHeadBoundBox.SetBound(&D3DXVECTOR3(10, 0, 0), &D3DXVECTOR3(30, 10, 15));

	sThresherAttack tmpAttack;
	tmpAttack.mAttackTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("L_Front_Tentacle07", tmpAttack.mAttackTransform);
	tmpAttack.mAttackBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(15, 7, 7));
	mThresherAttacks.push_back(tmpAttack);
	sThresherAttack tmpAttack1;
	tmpAttack1.mAttackTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("R_Front_Tentacle07", tmpAttack1.mAttackTransform);
	tmpAttack1.mAttackBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(15, 7, 7));
	mThresherAttacks.push_back(tmpAttack1);
	sThresherAttack tmpAttack2;
	tmpAttack2.mAttackTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("L_Front_Tentacle05", tmpAttack2.mAttackTransform);
	tmpAttack2.mAttackBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(15, 7, 7));
	mThresherAttacks.push_back(tmpAttack2);
	sThresherAttack tmpAttack3;
	tmpAttack3.mAttackTransform = new cTransform;
	cBaseObject::pSkinned->AddBoneTransform("R_Front_Tentacle05", tmpAttack3.mAttackTransform);
	tmpAttack3.mAttackBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(15, 7, 7));
	mThresherAttacks.push_back(tmpAttack3);

	attackRangeSphere = new cBoundSphere;
	attackRangeSphere->SetBound(&pTransform->GetWorldPosition(), &D3DXVECTOR3(60, 60, 60));
}

void cThresher::InitForConstant()
{
	mTrigger = false;
	isLive = true;
	isOnceAttack = false;
	speed = 20.0f;
	HP = 1000.0f;
	MaxHP = HP;
	mState = eBossBodyState_Idle;
	isAttackted = 0;						//���� ����
	count = 0;
}

void cThresher::LoadMesh()
{
	mFilePath = "../Resources/Meshes/Boss/Thresher_main.X";
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -90.0f*ONE_RAD );
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.25f, 0.25f, 0.25f);
	D3DXMATRIXA16 result = matWolrd*matCorrection;
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);
}

void cThresher::Update(float timeDelta)
{
	cBossManager::Update(timeDelta);
	if (mTrigger&&count < 100)
	{
		if (count == 0)
		{
			SOUND_MGR->Allstop();//
			SOUND_MGR->play("��������2");
			SOUND_MGR->update();
		}
		if (count == 99)
		{
			SOUND_MGR->Allstop();//
			SOUND_MGR->bgmPlay("�������");
			SOUND_MGR->update();
		}
		count++;
		return;
	}
	D3DXVECTOR3 heightCorrect = D3DXVECTOR3(pTransform->GetWorldPosition().x,pMap->GetpTerrain()->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z) - 10, pTransform->GetWorldPosition().z);
	pTransform->SetWorldPosition(heightCorrect);
	ActionByState(timeDelta);

	if (KEY_MGR->IsOnceDown('P')) {
		HP = 300;
	}

	if (HP <= 500) {
		mArm1->Update(timeDelta);
		mArm2->Update(timeDelta);
	}
	if (mState != eBossBodyState_Idle) {
		mTail->Update(timeDelta);
	}
}

void cThresher::Release()
{
	cBossManager::Release();
	SAFE_DELETE(mBodyTransform);
	SAFE_DELETE(mHeadTransform);

	SAFE_DELETE(mArm1);
	SAFE_DELETE(mArm2);
	SAFE_DELETE(mTail);
}

void cThresher::Render()
{
	cBossManager::Render();
	//mBodyBoundBox.RenderGizmo(mBodyTransform);
	//mHeadBoundBox.RenderGizmo(mHeadTransform);
	//attackRangeSphere->RenderGizmo(pTransform);
	//for (int i = 0; i < mThresherAttacks.size(); ++i) {
	//	mThresherAttacks[i].mAttackBoundBox.RenderGizmo(mThresherAttacks[i].mAttackTransform);
	//}
	mArm1->Render();
	mArm2->Render();
	if (mState != eBossBodyState_Idle) {
		mTail->Render();
	}
}

void cThresher::RenderSprite()
{
	cBossManager::RenderSprite();

	
	if (mTrigger)
	{
		if (count < 100)
		{
			RECT rcimg = { 0,0,2048,950 };
			SPRITE_MGR->DrawTexture(bossStartimg, &rcimg, RandomIntRange(-10, 1), RandomIntRange(-10, 1), 0.63, 0.85, 0);
			return;
		}
		RECT rc = { 2,0,253,15 };
		RECT rc2 = { 2,0,2 + ((((float)251) / MaxHP)*HP),15 };

		SPRITE_MGR->DrawTexture(hp2Tex, &rc, 250, 60, 3, 1, 0);
		SPRITE_MGR->DrawTexture(hpTex, &rc2, 250, 60, 3, 1, 0);

	}


}

void cThresher::RenderFont()
{
	cBossManager::RenderFont();
	if (mTrigger&&count < 100)
	{
		return;
	}
	if(mTrigger)
		DXFONT_MGR->PrintStyleTextOutline("playerUI5", "Thresher", 260,70 , 0XFFFFFFFF);
}

void cThresher::RenderShadow()
{
	cBossManager::RenderShadow();
	mArm1->RenderShadow();
	mArm2->RenderShadow();
	if (mState != eBossBodyState_Idle) {
		mTail->RenderShadow();
	}
}

void cThresher::ActionByState(float timeDelta)
{
	if (isAttackted != 0 && isLive) {
		HP -= isAttackted;
		LOG_MGR->AddLog("%d", HP);
		isAttackted = 0;
		if (HP <= 0) {
			SOUND_MGR->Allstop();
			SOUND_MGR->play("����Ŭ����");
			SOUND_MGR->update();
			HP = 0;
			pPlayer->SetExd(monsterEXP);
			mArm1->SetDie();
			mArm2->SetDie();
			mTail->SetDie();
			ChangeState(eBossBodyState_Die);
			isLive = false;
			return;
		}
		else if (HP <= 500) {
			ChangeState(eBossBodyState_Angry);
			return;
		}
		else {
			if (headshotCheck && RandomIntRange(0,10) <= 3) {	//��Ʈ�� ���Դ��� �ٽ� Ȯ���˻縦 �Ͽ� ��Ʈ��� �ߵ�
				if (mState == eBossBodyState_Hitted)
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[10]->GetName(), 0.0f);
				else
					ChangeState(eBossBodyState_Hitted);
				headshotCheck = false;
				return;
			}
		}
	}

	switch (mState)
	{
	case eBossBodyState_spawn:
		break;
	case eBossBodyState_Idle:
		if (mTrigger) {
			ChangeState(eBossBodyState_Run);
			mTail = new cThresherTail;
			mTail->SetpMap(pMap);
			mTail->SetpPlayer(pPlayer);
			mTail->Init({ pTransform->GetWorldPosition().x,pTransform->GetWorldPosition().z });
			mTail->SetThresher(this);
		}
		break;
	case eBossBodyState_Run:
		SeePlayer();
		moveDirPlayer(timeDelta);
		if (PHYSICS_MGR->IsOverlap(pPlayer->GetpPlayerTransform(), pPlayer->GetpPlayerSphere(), pTransform, attackRangeSphere)) {
			ChangeState(eBossBodyState_Attack);
		}
		break;
	case eBossBodyState_Attack:
		SeePlayer();
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(pPlayer->GetpPlayerTransform(), pPlayer->GetpPlayerSphere(), pTransform, attackRangeSphere)) {
				ChangeState(eBossBodyState_Run);
			}
			else {
				isOnceAttack = false;
				cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[RandomIntRange(7, 9)]->GetName(), 0.0f);
				return;
			}
		}
		break;
	case eBossBodyState_Hitted:
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (PHYSICS_MGR->IsOverlap(pPlayer->GetpPlayerTransform(), pPlayer->GetpPlayerSphere(), pTransform, attackRangeSphere)) {
				ChangeState(eBossBodyState_Attack);
				return;
			}
			ChangeState(eBossBodyState_Run);
		}
		break;
	case eBossBodyState_Angry:
		if (*cBaseObject::GetSkinned()->Getloop()) {
			speed *= 2;
			if (PHYSICS_MGR->IsOverlap(pPlayer->GetpPlayerTransform(), pPlayer->GetpPlayerSphere(), pTransform, attackRangeSphere)) {
				ChangeState(eBossBodyState_Attack);
				return;
			}
			ChangeState(eBossBodyState_Run);
		}
		break;
	case eBossBodyState_Die:
		break;
	}
}

void cThresher::ChangeState(eBossBodyState currentState)
{
	if (mState != currentState)
		mState = currentState;
	else
		return;

	//[�ִϸ��̼�]
	//	0 �� Idle
	//	1 �� ArmIdle1	- ������(�˼����ö�)
	//	2 �� ArmIdle2	- �г��Ҷ� ����� ����(�˼����ö�)
	//	3 �� Move1		- �� �Ʒ��� ���� ������
	//	4 �� Move2_1	- �� �Ʒ��� ����(SET)
	//	5 �� Move2_2	- �� ���� �ö����(SET)
	//	6 �� Move3		- �޷��ö�
	//	7 �� Attack1	- ��� ġ��
	//	8 �� Attack2	- ���ʷ� ��� �Ʒ��� ġ��
	//	9 �� Attack3	- �������� �ķ�ġ��
	//	10�� Hited		- ������
	//	11�� Death
	/*
	SOUND_MGR->add3DSound("��������", "../Resources/Sounds/BossSound/��������.mp3", 0, 500);
	SOUND_MGR->add3DSound("����������", "../Resources/Sounds/BossSound/����������.mp3", 0, 500);

	SOUND_MGR->add3DSound("�˼������.", "../Resources/Sounds/BossSound/�˼������.mp3", 0, 500);
	SOUND_MGR->add3DSound("�˼����ö�", "../Resources/Sounds/BossSound/�˼����ö�.mp3", 0, 500);
	SOUND_MGR->add3DSound("�˼�������", "../Resources/Sounds/BossSound/�˼�������.mp3", 0, 500);
	SOUND_MGR->add3DSound("�����̻���", "../Resources/Sounds/BossSound/�����̻���.mp3", 0, 500);*/
	switch (mState)
	{
	case eBossBodyState_spawn:
		break;
	case eBossBodyState_Idle:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0.0f);
		break;
	case eBossBodyState_Run:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[6]->GetName(), 0.0f);
		break;
	case eBossBodyState_Attack:
		SOUND_MGR->Play3DSound("�˼�Ÿ��", pTransform->GetWorldPosition());
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[RandomIntRange(7,9)]->GetName(), 0.0f);
		break;
	case eBossBodyState_Hitted:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[10]->GetName(), 0.0f);
		break;
	case eBossBodyState_Angry:
		SOUND_MGR->Play3DSound("�˼������", pTransform->GetWorldPosition());
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[2]->GetName(), 0.0f);
		break;
	case eBossBodyState_Die:
		SOUND_MGR->Play3DSound("����������",pTransform->GetWorldPosition());
		SOUND_MGR->update();
		cBaseObject::GetSkinned()->PlayOneShotAfterHold(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[11]->GetName(), 0.0f);
		break;
	}
}