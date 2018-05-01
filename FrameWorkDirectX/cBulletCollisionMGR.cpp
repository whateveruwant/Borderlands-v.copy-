#include "stdafx.h"
#include "cBulletCollisionMGR.h"
#include "cParticle.h"
#include "cQuadTree.h"


cBulletCollisionMGR::cBulletCollisionMGR()
{
}


cBulletCollisionMGR::~cBulletCollisionMGR()
{
}

HRESULT cBulletCollisionMGR::Init()
{
	VEC_COLOR colors;//����
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	VEC_SCALE scales;//ũ��
	scales.push_back(0.5f);
	scales.push_back(0.5f);
	scales.push_back(0.5f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/explosion.png");

	pPlayerBullets = new cParticleBullet();
	pPlayerBullets->Init(500, colors, scales, pTex);
	pEnemyBullets = new cParticleBullet();
	pEnemyBullets->Init(500, colors, scales, pTex);


	VEC_SCALE scales2;//ũ��
	scales2.push_back(7.0f);
	scales2.push_back(7.0f);
	scales2.push_back(7.0f);
	pBossBullets = new cParticleBullet();
	pBossBullets->Init(50, colors, scales2, pTex);

	return S_OK;
}

void cBulletCollisionMGR::Release()
{
	SAFE_DELETE(pPlayerBullets);
	SAFE_DELETE(pEnemyBullets);
	SAFE_DELETE(pBossBullets);
}

void cBulletCollisionMGR::Update(float timeDelta)
{
	pPlayerBullets->UpdateForPlayer(timeDelta);
	pEnemyBullets->UpdateForEnemy(timeDelta);
	pBossBullets->UpdateForBoss(timeDelta);

	CheckEnemyNearAtkAndPlayer();
}

void cBulletCollisionMGR::Render()
{
	pPlayerBullets->Render();
	pEnemyBullets->Render();
	pBossBullets->Render();
}

void cBulletCollisionMGR::FirePlayer()
{
	pPlayerBullets->StartOneParticle();
	//�ͷ��� �浹�˻�
	D3DXVECTOR3 pHits = { 0,0,0 };
	Ray tmpRay;
	tmpRay.origin = pPlayerBullets->GetPaticles().back()->originPos;
	D3DXVec3Normalize(&tmpRay.direction, &pPlayerBullets->GetPaticles().back()->GetTransform().GetForward());

	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		pPlayerBullets->GetPaticles().back()->hitTerrain = true;
		pPlayerBullets->GetPaticles().back()->hitPos = pHits;
	}
	else {
		pPlayerBullets->GetPaticles().back()->hitTerrain = false;
	}
}

void cBulletCollisionMGR::FireEnemy()
{
	pEnemyBullets->StartOneParticle();
	//�ͷ��� �浹�˻�
	D3DXVECTOR3 pHits = { 0,0,0 };
	Ray tmpRay;
	tmpRay.origin = pEnemyBullets->GetPaticles().back()->originPos;
	D3DXVec3Normalize(&tmpRay.direction, &pEnemyBullets->GetPaticles().back()->GetTransform().GetForward());
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		pEnemyBullets->GetPaticles().back()->hitTerrain = true;
		pEnemyBullets->GetPaticles().back()->hitPos = pHits;
	}
	else {
		pEnemyBullets->GetPaticles().back()->hitTerrain = false;
	}
}

void cBulletCollisionMGR::FireBoss()
{
	pBossBullets->StartOneParticle();
	//�ͷ��� �浹�˻�
	D3DXVECTOR3 pHits = { 0,0,0 };
	Ray tmpRay;
	tmpRay.origin = pBossBullets->GetPaticles().back()->originPos;
	D3DXVec3Normalize(&tmpRay.direction, &pBossBullets->GetPaticles().back()->GetTransform().GetForward());
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		pBossBullets->GetPaticles().back()->hitTerrain = true;
		pBossBullets->GetPaticles().back()->hitPos = pHits;
	}
	else {
		pBossBullets->GetPaticles().back()->hitTerrain = false;
	}
}

void cBulletCollisionMGR::CheckEnemyNearAtkAndPlayer()
{
	//�ٰŸ�	 
	for (int i = 0; i < pEnemys.size(); ++i)
	{
		if (pEnemys[i]->mCurrentState != eEnemyState_Attack || pEnemys[i]->mFilePath == "../Resources/Meshes/enemy/Bandit/Bandit.X") continue;
		if (!pEnemys[i]->isOneAttack && PHYSICS_MGR->IsOverlap(pEnemys[i]->LboneHand, pEnemys[i]->Lhandbox, pPlayer->tranCharacter, &pPlayer->tranChaBoundBox))
		{
			WhereHit(pEnemys[i]->GetTransform()->GetWorldPosition());
			if (pEnemys[i]->mFilePath == "../Resources/Meshes/enemy/Skag/Skag(final).X")//skag
			{
				pPlayer->isHittedDamage = pEnemys[i]->monsterDmg;
			}
			else if (pEnemys[i]->mFilePath == "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X")//battledroid
			{
				pPlayer->isHittedDamage = pEnemys[i]->monsterDmg;
			}
			pEnemys[i]->isOneAttack = true;
		}
	}

	if (pBoss != nullptr) {
		if (pBoss->GetState() == eBossBodyState_Attack) {
			for (int i = 0; i < pBoss->GetAttacks().size(); ++i) {
				if (!pBoss->GetOnceAttack() && PHYSICS_MGR->IsOverlap(pBoss->GetAttacks()[i].mAttackTransform, &pBoss->GetAttacks()[i].mAttackBoundBox, pPlayer->tranCharacter, &pPlayer->tranChaBoundBox)) {
					WhereHit(pBoss->GetBodyTransform()->GetWorldPosition());
					pPlayer->isHittedDamage = 20;
					LOG_MGR->AddLog("���� ���� ����");
					pBoss->SetOnceAttack(true);
				}
			}
		}
	}
}

bool cBulletCollisionMGR::CheckPlayerOneBullet(int index)
{
	if (pPlayerBullets->GetPaticles()[index]->hitTerrain) {
		D3DXVECTOR3 dir = pPlayerBullets->GetPaticles()[index]->hitPos - pPlayerBullets->GetPaticles()[index]->originPos;
		D3DXVECTOR3 dir2= pPlayerBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition() - pPlayerBullets->GetPaticles()[index]->originPos;
		float distSq = D3DXVec3Length(&dir);
		float distSq2 = D3DXVec3Length(&dir2);
		if (distSq <= distSq2) {
			pPlayerBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("�����浹");
			return true;
		}
	}
	else {
		D3DXVECTOR3 pos = pPlayerBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition();
		if (pMapInfo->GetHeight(pos.x, pos.z) != -1) {
			if (pos.y <= pMapInfo->GetHeight(pos.x, pos.z)) {
				pPlayerBullets->DeleteOneParticle(index);
				LOG_MGR->AddLog("������");
				return true;
			}
		}
	}

	//������Ʈ �浹�˻�
	for (int i = 0; i < pObjects.size(); ++i) {
		if (PHYSICS_MGR->IsOverlap(&pPlayerBullets->GetPaticles()[index]->m_Transform, pPlayerBullets->m_box, pObjects[i]->pTransform, &pObjects[i]->BoundBox))
		{
			pPlayerBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("������Ʈ�浹");
			return true;
		}
	}

	//�������� �浹�˻�
	if (pBoss != nullptr) {
		if (PHYSICS_MGR->IsOverlap(&pPlayerBullets->GetPaticles()[index]->m_Transform, pPlayerBullets->m_box, pBoss->GetHeadTransform(), &pBoss->GetHeadBoundBox()))
		{
			pBoss->isAttackted += pPlayer->Getbulletdmg() * 1.5;
			pPlayerBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("�����Ӹ����浹");
			return true;
		}
		else if (PHYSICS_MGR->IsOverlap(&pPlayerBullets->GetPaticles()[index]->m_Transform, pPlayerBullets->m_box, pBoss->GetBodyTransform(), &pBoss->GetBodyBoundBox()))
		{
			pBoss->isAttackted += pPlayer->Getbulletdmg();
			pPlayerBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("�������浹");
			return true;
		}
	}

	//������� �浹�˻�
	for (int j = 0; j < pEnemys.size(); ++j) {
		if (pEnemys[j]->isLive == false) continue;
		D3DXVECTOR3 distanceEP = pEnemys[j]->pTransform->GetWorldPosition() - pPlayerBullets->GetPaticles()[index]->GetTransform().GetWorldPosition();
		if (D3DXVec3Length(&distanceEP) >= 20.0f) continue;
		if (PHYSICS_MGR->IsOverlap(pEnemys[j]->pTransform, pEnemys[j]->monsterbox, &pPlayerBullets->GetPaticles()[index]->GetTransform(), pPlayerBullets->m_box)) {   // ����ū �ڽ� ->p_vEnemyBounds.at(i)[0] �� �˻�
			if (PHYSICS_MGR->IsOverlap(pEnemys[j]->boneHead, pEnemys[j]->headbox, &pPlayerBullets->GetPaticles()[index]->GetTransform(), pPlayerBullets->m_box)) {
				//pEnemys[j]->isAttackted = pPlayer->Getbulletdmg()*1.2;

				SOUND_MGR->Play3DSound("������弦��", pEnemys[j]->pTransform->GetWorldPosition(), 1.0f);
				SOUND_MGR->update();
				pEnemys[j]->headshotCheck = true;
				pEnemys[j]->isAttackted += pPlayer->Getbulletdmg() * pPlayer->Getcri();
				{
					sDamageText addFont;
					addFont.mColor = 0xffff0000;
					addFont.mDamage = "CRITICAL";
					addFont.mPosition = pPlayerBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition();
					addFont.mTime = 1.5f;
					addFont.slate = RandomFloatRange(0.3, 0.5);
					EFFECT_MGR->AddDamageFont(addFont);
				}
				//���� ����Ʈ ���
				if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X") {
					EFFECT_MGR->StartQuadParticleEmitter("robotbloodHeadshot", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("robotblood", 0.35f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				else if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/Skag/Skag(final).X")
				{
					EFFECT_MGR->StartQuadParticleEmitter("alienBloodHeadshot", 0.15f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("alienBloodHead", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("alienBlood2", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				else if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/Bandit/Bandit.X")
				{
					EFFECT_MGR->StartQuadParticleEmitter("bloodHeadshot", 0.15f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("bloodThird", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("bloodSecond", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				pPlayerBullets->DeleteOneParticle(index);
				LOG_MGR->AddLog("��弦");
				return true;
			}
			else if (PHYSICS_MGR->IsOverlap(pEnemys[j]->pTransform, pEnemys[j]->bodybox, &pPlayerBullets->GetPaticles()[index]->GetTransform(), pPlayerBullets->m_box)) {
				//pEnemys[j]->isAttackted = pPlayer->Getbulletdmg();
				pEnemys[j]->isAttackted += pPlayer->Getbulletdmg();
				//���� �Ѿ� ����
				{
					sDamageText addFont;
					addFont.mColor = 0xffffffff;
					addFont.mDamage = to_string(pPlayer->Getbulletdmg());
					addFont.mPosition = pPlayerBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition();
					addFont.mTime = 1.5f;
					addFont.slate = RandomFloatRange(0.3, 0.5);
					EFFECT_MGR->AddDamageFont(addFont);
				}
				//���� ����Ʈ ���
				if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X") {
					EFFECT_MGR->StartQuadParticleEmitter("robotblood", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				else if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/Skag/Skag(final).X")
				{
					EFFECT_MGR->StartQuadParticleEmitter("alienBlood1", 0.15f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("alienBlood2", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				else if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/Bandit/Bandit.X")
				{
					EFFECT_MGR->StartQuadParticleEmitter("bloodFirst", 0.15f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					EFFECT_MGR->StartQuadParticleEmitter("bloodSecond", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
				}
				pPlayerBullets->DeleteOneParticle(index);
				LOG_MGR->AddLog("����");
				return true;
			}
			else if (pEnemys[j]->mFilePath == "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X")//��Ʋ����̵� �ٸ��浹 �߰�
			{
				if ((PHYSICS_MGR->IsOverlap(pEnemys[j]->pTransform, pEnemys[j]->Llegbox, &pPlayerBullets->GetPaticles()[index]->GetTransform(), pPlayerBullets->m_box)) ||
					(PHYSICS_MGR->IsOverlap(pEnemys[j]->pTransform, pEnemys[j]->Rlegbox, &pPlayerBullets->GetPaticles()[index]->GetTransform(), pPlayerBullets->m_box)))
				{
					EFFECT_MGR->StartQuadParticleEmitter("robotblood", 0.25f, pPlayerBullets->GetPaticles()[index]->GetpTransform());
					pEnemys[j]->isAttackted += 1;
					LOG_MGR->AddLog("�ٸ���");
					return true;
				}
			}
		}
	}

	return false;
}

bool cBulletCollisionMGR::CheckEnemyOneBullet(int index)
{
	if (pEnemyBullets->GetPaticles()[index]->hitTerrain) {
		D3DXVECTOR3 dir = pEnemyBullets->GetPaticles()[index]->hitPos - pEnemyBullets->GetPaticles()[index]->originPos;
		D3DXVECTOR3 dir2 = pEnemyBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition() - pEnemyBullets->GetPaticles()[index]->originPos;
		float distSq = D3DXVec3Length(&dir);
		float distSq2 = D3DXVec3Length(&dir2);
		if (distSq <= distSq2) {
			pEnemyBullets->DeleteOneParticle(index);
			return true;
		}
	}
	else {
		D3DXVECTOR3 pos = pEnemyBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition();
		if (pMapInfo->GetHeight(pos.x, pos.z) != -1) {
			if (pos.y <= pMapInfo->GetHeight(pos.x, pos.z)) {
				pEnemyBullets->DeleteOneParticle(index);
				return true;
			}
		}
	}

	//������Ʈ �浹�˻�
	for (int i = 0; i < pObjects.size(); ++i) {
		if (PHYSICS_MGR->IsOverlap(&pEnemyBullets->GetPaticles()[index]->m_Transform, pEnemyBullets->m_box, pObjects[i]->pTransform, &pObjects[i]->BoundBox))
		{
			pEnemyBullets->DeleteOneParticle(index);
			return true;
		}
	}

	//�÷��̾���� �˻�
	if (PHYSICS_MGR->IsOverlap(&pEnemyBullets->GetPaticles()[index]->m_Transform, pEnemyBullets->m_box, pPlayer->tranCharacter, &pPlayer->tranChaBoundBox))
	{
		pPlayer->isHittedDamage = RandomIntRange(10, 15);
		WhereHit(pEnemyBullets->GetPaticles()[index]->m_Transform.GetWorldPosition(), pEnemyBullets->GetPaticles()[index]->m_Transform.GetForward());
		pEnemyBullets->DeleteOneParticle(index);
		return true;
	}

	return false;
}

bool cBulletCollisionMGR::CheckBossOneBullet(int index)
{

	if (pBossBullets->GetPaticles()[index]->hitTerrain) {
		D3DXVECTOR3 dir = pBossBullets->GetPaticles()[index]->hitPos - pBossBullets->GetPaticles()[index]->originPos;
		D3DXVECTOR3 dir2 = pBossBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition() - pBossBullets->GetPaticles()[index]->originPos;
		float distSq = D3DXVec3Length(&dir);
		float distSq2 = D3DXVec3Length(&dir2);
		if (distSq <= distSq2) {
			pBossBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("�����浹");
			return true;
		}
	}
	else {
		D3DXVECTOR3 pos = pBossBullets->GetPaticles()[index]->GetpTransform()->GetWorldPosition();
		if (pMapInfo->GetHeight(pos.x, pos.z) != -1) {
			if (pos.y <= pMapInfo->GetHeight(pos.x, pos.z)) {
				pBossBullets->DeleteOneParticle(index);
				LOG_MGR->AddLog("������");
				return true;
			}
		}
	}

	//������Ʈ �浹�˻�
	for (int i = 0; i < pObjects.size(); ++i) {
		if (PHYSICS_MGR->IsOverlap(&pBossBullets->GetPaticles()[index]->m_Transform, pBossBullets->m_box, pObjects[i]->pTransform, &pObjects[i]->BoundBox))
		{
			pBossBullets->DeleteOneParticle(index);
			LOG_MGR->AddLog("������Ʈ �浹");
			return true;
		}
	}

	//�÷��̾���� �˻�
	if (PHYSICS_MGR->IsOverlap(&pBossBullets->GetPaticles()[index]->m_Transform, pBossBullets->m_box, pPlayer->tranCharacter, &pPlayer->tranChaBoundBox))
	{
		pPlayer->isHittedDamage = RandomIntRange(10, 15);
		WhereHit(pBossBullets->GetPaticles()[index]->m_Transform.GetWorldPosition(), pBossBullets->GetPaticles()[index]->m_Transform.GetForward());
		pBossBullets->DeleteOneParticle(index);
		LOG_MGR->AddLog("�÷��̾�� ���̹�");
		return true;
	}

	return false;



}

void cBulletCollisionMGR::WhereHit(D3DXVECTOR3 hitting, D3DXVECTOR3 hittingForward)
{
	float angle;
	if (hittingForward == D3DXVECTOR3(0,0,0)) {
		D3DXVECTOR3 forward = pPlayer->GetpPlayerXZTransform()->GetForward();
		D3DXVECTOR3 dir =  hitting - pPlayer->GetpPlayerXZTransform()->GetWorldPosition();

		float cosAngle = acosf(D3DXVec3Dot(&forward, &dir) / (D3DXVec3Length(&forward) * D3DXVec3Length(&dir)));
		cosAngle = D3DXToDegree(cosAngle);
		angle = (forward.x * dir.y - forward.y * dir.x > 0.0f) ? cosAngle : -cosAngle;

		while (angle <= 0) {
			angle += 360;
		}

		if (angle <= 45 || angle >= 315) {
			//���ʿ���
			pPlayer->SetUpHit();
		}
		else if (angle >= 45 && angle <= 135) {
			//���ʿ���
			pPlayer->SetLeftHit();
		}
		else if (angle >= 135 && angle <= 225) {
			//�Ʒ��ʿ���
			pPlayer->SetDownHit();
		}
		else {
			//�����ʿ���
			pPlayer->SetRightHit();
		}
	}
	else {
		D3DXVECTOR3 forward = pPlayer->GetpPlayerXZTransform()->GetForward();
		D3DXVECTOR3 dir = hittingForward;

		float cosAngle = acosf(D3DXVec3Dot(&forward, &dir) / (D3DXVec3Length(&forward) * D3DXVec3Length(&dir)));
		cosAngle = D3DXToDegree(cosAngle);
		angle = (forward.x * dir.y - forward.y * dir.x > 0.0f) ? cosAngle : -cosAngle;

		while (angle <= 0) {
			angle += 360;
		}

		if (angle <= 45 || angle >= 315) {
			//�Ʒ��ʿ���
			pPlayer->SetDownHit();
		}
		else if (angle >= 45 && angle <= 135) {
			//���ʿ���
			pPlayer->SetLeftHit();
		}
		else if (angle >= 135 && angle <= 225) {
			//���ʿ���
			pPlayer->SetUpHit();
		}
		else {
			//�����ʿ���
			pPlayer->SetRightHit();
		}
	}
}
