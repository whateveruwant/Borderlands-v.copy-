#include "stdafx.h"
#include "cBandit.h"
#include "cTransform.h"

cBandit::cBandit()
{
}


cBandit::~cBandit()
{
}

void cBandit::Init(D3DXVECTOR2 xz)
{
	cEnemy::Init(xz);
	monsterHeight = 10;
	//���⼭���� �߰�
	D3DXMATRIXA16 matWolrd;
	D3DXMatrixRotationY(&matWolrd, -1 * ONE_RAD * 90.0f);
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.1f, 0.1f, 0.1f);
	D3DXMATRIXA16 result = matWolrd*matCorrection;
	mFilePath = "../Resources/Meshes/enemy/Bandit/Bandit.X";
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(mFilePath, &result);
	cBaseObject::SetMesh(pSkinned);
	cBaseObject::SetActive(true);

	//�ڽ�
	monsterbox = new cBoundBox();//���� �浹 �ڽ� 
	monsterbox->SetBound(&D3DXVECTOR3(0.0f, 8.0f, 0.0f), &D3DXVECTOR3(5.0f, 10.0f, 5.0f));

	bodybox = new cBoundBox();//���� �ڽ�
	bodybox->SetBound(&D3DXVECTOR3(0.0f, 8.0f, 0.0f), &D3DXVECTOR3(4.f, 6.f, 4.f));

	boneHead = new cTransform();//�Ӹ� �� Ʈ������
	cBaseObject::pSkinned->AddBoneTransform("Head", boneHead);
	headbox = new cBoundBox();//�Ӹ� �ڽ�
	headbox->SetBound(&D3DXVECTOR3(0.0f, 0.5f, 0.0f), &D3DXVECTOR3(1.6f, 1.6f, 1.6f));

	LboneHand = new cTransform();
	cBaseObject::pSkinned->AddBoneTransform("R_Hand", LboneHand);
	Lhandbox = new cBoundBox();
	Lhandbox->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(1.5f, 1.5f, 1.5f));

	appreciateRangeSphere = new cBoundSphere();//�÷��̾� �����ڽ�
	appreciateRangeSphere->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(100.0f, 100.0f, 100.0f));

	attackRangeSphere = new cBoundSphere();//���ݹڽ�
	attackRangeSphere->SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(70.0f, 70.0f, 70.0f));

	mCurrentState = eEnemyState_Idle;//����
	cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0.3f);//�ִϸ��̼�

	mRunSpeed = BANDIT_SPEED;				//���ǵ�
	monsterHP = BANDIT_HP;					//ü��
	monsterMaxHP = monsterHP;
	monsterDmg = BANDIT_DMG;				//������
	checkOneAttack = false;					//���� �ѹ� ���ݾ���
	banditTimeAttack = 0;					//�ѼҴ� ������
	monsterEXP = 15;						//bandit����ġ
}

void cBandit::Update(float timeDelta)
{
	cEnemy::Update(timeDelta);
	//���⼭���� �߰�
	LboneHand->LookPosition(pPlayerTransform->GetWorldPosition() + D3DXVECTOR3(-0.5f, 4.0f, 0));
	this->ActionByState(timeDelta);
	//monsterDmg = BANDIT_DMG;
}

void cBandit::Release()
{
	cEnemy::Release();
	//���⼭���� �߰�
	SAFE_DELETE(monsterbox);
	SAFE_DELETE(bodybox);
	SAFE_DELETE(headbox);
	SAFE_DELETE(Lhandbox);
}

void cBandit::Render()
{
	cEnemy::Render();
	//���⼭���� �߰�

	if (KEY_MGR->IsStayDown(VK_F1))
	{
		monsterbox->RenderGizmo(pTransform);
		bodybox->RenderGizmo(pTransform);
		headbox->RenderGizmo(boneHead);
		Lhandbox->RenderGizmo(LboneHand);
		appreciateRangeSphere->RenderGizmo(pTransform);
		attackRangeSphere->RenderGizmo(pTransform);
		pTransform->RenderGimozo();
	}
}

void cBandit::RenderSprite()
{
	cEnemy::RenderSprite();
	if (isFontSee && isLive)
	{
		int hpnum = 259;
		RECT rcHp = { 0 ,0 ,hpnum / monsterMaxHP * monsterHP, 20 };
		RECT rcBackHp = { 0, 0, hpnum, 20 };
		SPRITE_MGR->DrawTexture(monsterHPendText, &rcBackHp, WINSIZE_X / 2 - 55, WINSIZE_Y / 2 - 130, 0.5, 1.0, 0);
		SPRITE_MGR->DrawTexture(monsterHpText, &rcHp, WINSIZE_X / 2 - 55, WINSIZE_Y / 2 - 130, 0.63, 1.0, 0);
	}
}

void cBandit::RenderFont()
{
	//��Ʈ���
	if (isFontSee && isLive)
	{
		DXFONT_MGR->PrintStyleText("playerUI", "Bandit", WINSIZE_X / 2 - 30, WINSIZE_Y / 2 - 100, 0xffffffff);
		if (KEY_MGR->IsStayDown(VK_F1))
		{
			string monsterHpText = to_string(monsterHP);
			DXFONT_MGR->PrintStyleTextOutline("playerUI", monsterHpText, WINSIZE_X / 2 - 95, WINSIZE_Y / 2 - 135, 0Xffffffff);
		}
	}
}

void cBandit::ChangeState(eEnemyState currentState)
{
	if (savecurrentState != currentState)
		savecurrentState = currentState;
	else
		return;

	mCurrentState = currentState;
	switch (currentState)
	{
	case eEnemyState_Idle:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[0]->GetName(), 0.3f);
		break;
	case eEnemyState_Follow:
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[2]->GetName(), 0.3f);
		break;
	case eEnemyState_Run:
		SOUND_MGR->Play3DSound("Bandit�ν�", this->pTransform->GetWorldPosition(), 0.8f);
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[4]->GetName(), 0.3f);
		break;
	case eEnemyState_Attack:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[5]->GetName(), 0.0f);
		break;
	case eEnemyState_Hitted:
		cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[7]->GetName(), 0.0f);
		break;
	case eEnemyState_Die:
		SOUND_MGR->Play3DSound("Bandit����", this->pTransform->GetWorldPosition(), 0.5f);
		cBaseObject::GetSkinned()->PlayOneShotAfterHold(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[9]->GetName(), 1.0f);
		break;
	}
	/*
	9 Death_var3
	8 knockback_Recover(critical)
	7 HardFlinch_Cirtical(�´¸��)
	6 Melee_Weapon_Var1
	5 rifle_atk
	4 fast_run
	3 Run_F
	2 patrol_F
	1 cover_Low_Idle(�ɾ��ִ� ���̵�)
	0 idle
	*/
}

void cBandit::ActionByState(float timeDelta)
{
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
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[7]->GetName(), 0.0f);
				else
					ChangeState(eEnemyState_Hitted);
				headshotCheck = false;
				return;
			}
		}
	}

	banditTimeAttack--;

	switch (mCurrentState)
	{
	case eEnemyState_Idle:
		respawntime = 0;
		if (CheckAppreciate() || compulsion)
			ChangeState(eEnemyState_Run);
		break;
	case eEnemyState_Follow:
		cEnemy::SeePlayer();
		cEnemy::moveDirPlayer(timeDelta);
		break;
	case eEnemyState_Run:
		cEnemy::SeePlayer();
		cEnemy::moveDirPlayer(timeDelta);
		if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->attackRangeSphere))
		{
			ChangeState(eEnemyState_Attack);
		}
		else if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere) && !compulsion)
			ChangeState(eEnemyState_Idle);
		break;
	case eEnemyState_Attack:
		cEnemy::SeePlayer();
		//�ѽ�� ������
		if (banditTimeAttack < 0)
		{
			banditTimeAttack = 35;
			COLLISIONMGR->pEnemyBullets->mTransform = LboneHand;
			COLLISIONMGR->FireEnemy();
			SOUND_MGR->Play3DSound("Bandit����", this->pTransform->GetWorldPosition(), 0.5f);
		}

		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere))
				ChangeState(eEnemyState_Idle);
			else {
				if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->attackRangeSphere)) {
					cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(mFilePath)->GetAnimSet()[5]->GetName(), 0.0f);
					return;
				}
				ChangeState(eEnemyState_Run);
			}
		}
		break;
	case eEnemyState_Hitted:
		if (*cBaseObject::GetSkinned()->Getloop()) {
			if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere))
				ChangeState(eEnemyState_Idle);
			else {
				if (PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->attackRangeSphere)) {
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
		if (!activeEnd)//�߰�
		{
			pTransform->MovePositionWorld(D3DXVECTOR3(0, activeEndPosY, 0));
			if (pTransform->GetWorldPosition().y < this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z) - 10.0f)
			{
				activeEnd = true;
				activeEndPosY = 0.0f;
				break;
			}
		}
		break;
	}

	if (mCurrentState != eEnemyState_Die)
		pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z)-1, pTransform->GetWorldPosition().z);

}
