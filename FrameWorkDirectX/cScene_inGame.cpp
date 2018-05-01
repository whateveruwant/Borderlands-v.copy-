#include "stdafx.h"
#include "cScene_inGame.h"
#include "cLight_Direction.h"
#include "cTransform.h"
#include "cCamera.h"
#include "cSkag.h"
#include "cBandit.h"
#include "cBattleDroid.h"
#include "cSkelObject.h"
#include "cStaticObject.h"
#include "cVendingObject.h"
#include "cTerrain.h"
#include "time.h"

cScene_inGame::cScene_inGame()
{
}


cScene_inGame::~cScene_inGame()
{
}

HRESULT cScene_inGame::Scene_Init()
{
	srand(time(NULL));
	SOUND_MGR->Allstop();//�޴� bgm��ž

	SCENE_MGR->fProgress = 0.0f;
	SCENE_MGR->fString = "���� ���� ��";
	mMap = new cMap();
	mMap->Init();


	COLLISIONMGR->Init();
	COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
	

	SCENE_MGR->fProgress = 20.0f;
	SCENE_MGR->fString = "ĳ���� ���� ��";
	mCharacter = new Character();
	mCharacter->SetCamera(pMainCamera);
	mCharacter->Init();
	mCharacter->SetGameKey(&gGameKey);
	mCharacter->SetTerrain(mMap->GetpTerrain());
	mCharacter->SetMap(mMap);
	COLLISIONMGR->SetPlayer(mCharacter);

	SCENE_MGR->fProgress = 40.0f;
	SCENE_MGR->fString = "������Ʈ,�� ���� ��";
	LoadObjects("townFile/Object.save");

	SCENE_MGR->fProgress = 60.0f;
	SCENE_MGR->fString = "����Ʈ ���� ��";
	EFFECT_MGR->Init();
	EFFECT_MGR->SetCamera(pMainCamera);

	SCENE_MGR->fProgress = 80.0f;
	SCENE_MGR->fString = "��ƼŬ ���� ��";
	InitParticle();

	this->SetEnvironment("../Resources/TextureCUBE/CubeMap_BlackNight.dds");
	//this->SetEnvironment("../Resources/TextureCUBE/cubetest.dds");
	this->pSceneBaseDirectionLight->pTransform->RotateWorld(
		90.0f * ONE_RAD, 0, 0);

	SCENE_MGR->fProgress = 100.0f;
	SCENE_MGR->fString = "�ε� ��";
	SetupSounds();
	return S_OK;
}

void cScene_inGame::Scene_Release()
{
	mCharacter->Release();
	SAFE_DELETE(mCharacter);
	this->vCullingEnemies.clear();
	this->vCullingObjects.clear();
	this->vCullingItems.clear();
	for (int i = 0; i < vEnemies.size(); ++i) {
		SAFE_DELETE(vEnemies[i]);
	}
	for (int i = 0; i < vObjects.size(); ++i) {
		SAFE_DELETE(vObjects[i]);
	}
	for (int i = 0; i < vItems.size(); ++i) {
		SAFE_DELETE(vItems[i]);
	}
	SAFE_DELETE(mBoss);

	//�Ŵ��� ����
	COLLISIONMGR->Release();
	cBulletCollisionMGR::ReleaseInstance();
	EFFECT_MGR->Release();
	cEffectMGR::ReleaseInstance();
}

void cScene_inGame::Scene_Update(float timeDelta)
{
	if (mCharacter->GetPlayerDie()) {
		MoveMapForDie(!mCharacter->LoadPlayer());
	}

	SOUND_MGR->Update_Listener(pMainCamera->GetWorldPosition());
	float timedel = timeDelta;
	if (mCharacter->playerDie())
		timedel /= 4;
	if (mCharacter->GetInven()->Update(timedel))return;

	ShowCursor(FALSE);

	mMap->Update(timedel);
	mCharacter->Update(timedel);

	if (!mMap->GetIsTown()) {
		mBoss->Update(timedel);
		if (PHYSICS_MGR->IsOverlap(mBossTriggerTransform, &mBossTriggerBoundBox, mCharacter->GetpPlayerTransform(), mCharacter->GetpPlayerBoundBox())) {
			mBoss->SetTrigger(true);
		}
		if (mBoss->GetTrigger())
		{
			for (int j = 0; j < vObjects.size(); ++j) {
				if (vObjects[j]->GetFileName() == "../Resources/Meshes/Objects/Rock2.X") {
					if (vObjects[j]->pTransform->GetWorldPosition().y <0) {
						vObjects[j]->pTransform->MovePositionWorld(0, 25 * timeDelta, 0);
					}
				}
			}
		}
	}
	Culling(timedel);
	
	COLLISIONMGR->Update(timedel);
	EFFECT_MGR->Update(timedel);

	this->pSceneBaseDirectionLight->pTransform->DefaultControl2(timedel);
	RenderShadow();
	if (KEY_MGR->IsOnceDown('O')) {
		LOG_MGR->AddLog("x:%d,y:%d", (int)mCharacter->GetpPlayerTransform()->GetWorldPosition().x, (int)mCharacter->GetpPlayerTransform()->GetWorldPosition().z);
	}
}

void cScene_inGame::Scene_Render1()
{
	{
		cXMesh_Skinned::SetBaseLight(this->pSceneBaseDirectionLight);
		cXMesh_Skinned::SetLighting(&pSceneDarkLight);
		cXMesh_Skinned::SetCamera(this->pMainCamera);
		cXMesh_Skinned::SetTechniqueName("ReciveShadow");
		cXMesh_Static::SetBaseLight(this->pSceneBaseDirectionLight);
		cXMesh_Static::SetLighting(&pSceneDarkLight);
		cXMesh_Static::SetCamera(this->pMainCamera);
		cXMesh_Static::SetTechniqueName("ReciveShadow");
	}

	if (!mMap->GetIsTown()) {
		mBoss->Render();
	}
	mMap->Render(pMainCamera, pSceneBaseDirectionLight, pDirectionLightCamera);
	for (int i = 0; i < vCullingEnemies.size(); ++i) {
		vCullingEnemies[i]->Render();
	}
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->Render();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->Render();
	}
	mCharacter->Render();
	COLLISIONMGR->Render();
	if (mBossTriggerTransform != nullptr) {
		//mBossTriggerBoundBox.RenderGizmo(mBossTriggerTransform);
	}
}

void cScene_inGame::Scene_RenderSprite()
{
	for (int i = 0; i < vCullingEnemies.size(); ++i) {
		vCullingEnemies[i]->RenderSprite();
	}
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->RenderSprite();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->RenderSprite();
	}
	if (!mMap->GetIsTown()) {
		mBoss->RenderSprite();
	}
	mCharacter->Scene_RenderSprite();
}

void cScene_inGame::Scene_RenderFont()
{
	if (mCharacter->GetInven()->Scene_RenderFont())return;
	for (int i = 0; i < vCullingEnemies.size(); ++i) {
		vCullingEnemies[i]->RenderFont();
	}
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->RenderFont();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->RenderFont();
	}
	mCharacter->Scene_RenderFont();
	if (!mMap->GetIsTown()) {
		mBoss->RenderFont();
	}
	EFFECT_MGR->RenderFont();
}

void cScene_inGame::Culling(float timeDelta)
{
	this->pMainCamera->UpdateFrustum();
	this->vCullingEnemies.clear();
	this->vCullingObjects.clear();
	this->vCullingItems.clear();

	//enemy culling
	float nearDs = 1000;
	int nearIdx = NULL;
	


	if (mCharacter->CameraMove() )
	{
		
		//
		for (int i = 0; i < this->vEnemies.size(); i++) {
			if (!mMap->GetIsTown() && mBoss->GetTrigger())break;
			vEnemies[i]->Update(timeDelta);
			if (this->pMainCamera->Frustum.IsInFrustum(this->vEnemies[i]))
				this->vCullingEnemies.push_back(this->vEnemies[i]);
			if (!vEnemies[i]->isLive) {//������
				if (!vEnemies[i]->dropItem) {
					cItem* tmp = new cItem();
					tmp->Init(mCharacter->GetPlayerInfo(), (eItemKind)RandomIntRange(0, 5), vEnemies[i]->GetTransform()->GetWorldPosition());
					vItems.push_back(tmp);
					//EFFECT_MGR->StartParticleEmitter("moneyItemParticle", 5.0f, vItems.back()->pTransform);
					vEnemies[i]->dropItem = true;
				}
				if (vEnemies[i]->respawnmaxtime < vEnemies[i]->respawntime)//(KEY_MGR->IsOnceDown(VK_F2)) 
				{
					vEnemies[i]->RespawnEnemy();
				}
			}
			//�� ��Ʈ����
			D3DXVECTOR3 dir = *mCharacter->GetpPlayerPosition() - vEnemies[i]->pTransform->GetWorldPosition();
			float distiSq = D3DXVec3Length(&dir);
			if (distiSq > nearDs) continue;
			D3DXVECTOR3 pHit;
			D3DXVECTOR3 pHitMap;
			D3DXVECTOR3 pHitNormal;
			Ray ray;
			D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
			this->pMainCamera->ComputeRay(&ray, &center);
			if (PHYSICS_MGR->IsRayHitBound(&ray, vEnemies[i]->monsterbox, vEnemies[i]->pTransform, &pHit, &pHitNormal)) {
				D3DXVECTOR3 dir2;
				float distaceMap;
				if (mMap->GetpTerrain()->IsIntersectRay(&pHitMap, &ray)) {
					dir2 = *mCharacter->GetpPlayerPosition() - pHitMap;
					distaceMap = D3DXVec3Length(&dir2);
					if (distiSq < distaceMap) {
						if (nearIdx != NULL) vEnemies[nearIdx]->isFontSee = false;
						vEnemies[i]->isFontSee = true;
						nearIdx = i;
					}
				}
			}
			//��Ʈ���� ��
		}
	}

	//object culling
	bool chageMap = false;
	for (int i = 0; i < vObjects.size(); ++i) {
		vObjects[i]->Update(timeDelta);
		if (this->pMainCamera->Frustum.IsInFrustum(this->vObjects[i]))
			this->vCullingObjects.push_back(this->vObjects[i]);
		switch (vObjects[i]->mObjKind)
		{
		case eObjectKind_skel:
		{
			D3DXVECTOR3 dir = *mCharacter->GetpPlayerPosition() - vObjects[i]->pTransform->GetWorldPosition();
			float distSq = D3DXVec3Length(&dir);
			if (distSq >= 40.0f) {
				continue;
			}
			D3DXVECTOR3 pHit;
			D3DXVECTOR3 pHitNormal;
			Ray ray;
			D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
			this->pMainCamera->ComputeRay(&ray, &center);
			if (PHYSICS_MGR->IsRayHitBound(&ray, &vObjects[i]->BoundBox, vObjects[i]->pTransform, &pHit, &pHitNormal)) {
				vObjects[i]->isSee = true;
			}
		} break;
		case eObjectKind_vending: {D3DXVECTOR3 dir = *mCharacter->GetpPlayerPosition() - vObjects[i]->pTransform->GetWorldPosition();
			float distSq = D3DXVec3Length(&dir);
			if (distSq >= 40.0f) {
				continue;
			}
			D3DXVECTOR3 pHit;
			D3DXVECTOR3 pHitNormal;
			Ray ray;
			D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
			this->pMainCamera->ComputeRay(&ray, &center);
			if (PHYSICS_MGR->IsRayHitBound(&ray, &vObjects[i]->BoundBox, vObjects[i]->pTransform, &pHit, &pHitNormal)) {
				vObjects[i]->isSee = true;
			}
		} break;
		case eObjectKind_static: {
			if (vObjects[i]->GetFileName() == "../Resources/Meshes/Objects/Potal.X") {
				if (vObjects[i]->GetPlayAnim()) {
					//pSceneBaseDirectionLight->Color.a -= 0.1f;
					//pSceneBaseDirectionLight->Color.r -= 0.1f;
					//pSceneBaseDirectionLight->Color.g -= 0.1f;
					//pSceneBaseDirectionLight->Color.b -= 0.1f;
					//for (int k = 0; k < pSceneDarkLight.size(); ++k) {
					//	pSceneDarkLight[k]->Intensity += 0.1f;
					//}
				}

				if ((cStaticObject*)vObjects[i]->GetChangeMap()) {
					MoveMap(mMap->GetIsTown());
					chageMap = true;
					break;
					//pSceneBaseDirectionLight->Color = D3DXCOLOR(1, 1, 1, 1);
					//for (int k = 0; k < 4; ++k) {
					//	pSceneDarkLight[k]->Intensity = 0.35f;
					//}
				}
				D3DXVECTOR3 dir = *mCharacter->GetpPlayerPosition() - vObjects[i]->pTransform->GetWorldPosition();
				float distSq = D3DXVec3Length(&dir);
				if (distSq >= 40.0f) {
					continue;
				}
				D3DXVECTOR3 pHit;
				D3DXVECTOR3 pHitNormal;
				Ray ray;
				D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
				this->pMainCamera->ComputeRay(&ray, &center);
				if (PHYSICS_MGR->IsRayHitBound(&ray, &vObjects[i]->BoundBox, vObjects[i]->pTransform, &pHit, &pHitNormal)) {
					vObjects[i]->isSee = true;
				}
			}
			else if (vObjects[i]->GetFileName() == "../Resources/Meshes/Objects/Toilet.X") {
				if (vObjects[i]->GetPlayAnim() && mCharacter->moveEndCamera()) {
					for (int j = 0; j < vObjects.size(); ++j) {
						if (vObjects[j]->GetFileName() == "../Resources/Meshes/Objects/Rock2.X") {
							if (vObjects[j]->pTransform->GetWorldPosition().y >= -100) {
								vObjects[j]->pTransform->MovePositionWorld(0, -25 * timeDelta, 0);
							}
						}
					}
				}
				if ((cStaticObject*)vObjects[i]->GetChangeMap()) {
					mCharacter->PlayCameraMove();
					vObjects[i]->SetChangeMap(false);
					break;
				}
				D3DXVECTOR3 dir = *mCharacter->GetpPlayerPosition() - vObjects[i]->pTransform->GetWorldPosition();
				float distSq = D3DXVec3Length(&dir);
				if (distSq >= 40.0f) {
					continue;
				}
				D3DXVECTOR3 pHit;
				D3DXVECTOR3 pHitNormal;
				Ray ray;
				D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
				this->pMainCamera->ComputeRay(&ray, &center);
				if (PHYSICS_MGR->IsRayHitBound(&ray, &vObjects[i]->BoundBox, vObjects[i]->pTransform, &pHit, &pHitNormal)) {
					vObjects[i]->isSee = true;
				}
			}
		} break;
		}
		if (chageMap) break;
	}

	//item culling
	for (int i = 0; i < vItems.size(); ++i) {
		vItems[i]->Update(timeDelta);
		if (!vItems[i]->IsActive()) {
			vItems.erase(vItems.begin() + i);
			--i;
			continue;
		}
		if (this->pMainCamera->Frustum.IsInFrustum(this->vItems[i]))
			this->vCullingItems.push_back(this->vItems[i]);
		D3DXVECTOR3 pHit;
		D3DXVECTOR3 pHitNormal;
		Ray ray;
		D3DXVECTOR2 center = { WINSIZE_X / 2, WINSIZE_Y / 2 };
		this->pMainCamera->ComputeRay(&ray, &center);
		if (PHYSICS_MGR->IsRayHitStaticMeshObject(&ray, vItems[i], &pHit, &pHitNormal)) {
			vItems[i]->showInfo = true;
		}
	}
}

void cScene_inGame::SetupSounds()
{
	//�޴�
	//SOUND_MGR->addSound("Ÿ��Ʋ�޴�", "../Resources/Sounds/MenuSound.mp3", true, true);
	SOUND_MGR->addSound("�������", "../Resources/Sounds/bgm3.mp3", true, true);
	SOUND_MGR->addSound("����͹��", "../Resources/Sounds/����͹����.mp3", true, true);
	SOUND_MGR->addSound("�������", "../Resources/Sounds/BossSound/BossBgm1.mp3", true, true);
	SOUND_MGR->addSound("����Ŭ����", "../Resources/Sounds/GameClear.mp3", true, true);

	//SOUND_MGR->addSound("�޴���ư", "../Resources/Sounds/MenuButtonClick.mp3", false, false);
	SOUND_MGR->addSound("��ưŬ��", "../Resources/Sounds/ButtonClick2.mp3", false, false);

	SOUND_MGR->addSound("��������2", "../Resources/Sounds/BossSound/��������2.mp3", false, false);

	//ĳ����
	SOUND_MGR->add3DSound("�������ѼҸ�", "../Resources/Sounds/CharacterSound/Rifle.mp3", 0, 1000);
	//SOUND_MGR->addSound("�������ѼҸ�", "../Resources/Sounds/CharacterSound/Rifle.mp3", false, false);
	SOUND_MGR->add3DSound("���������ѼҸ�", "../Resources/Sounds/CharacterSound/Sniper.mp3", 0, 1000);
	//SOUND_MGR->addSound("���������ѼҸ�", "../Resources/Sounds/CharacterSound/Sniper.mp3", false, false);
	SOUND_MGR->add3DSound("�����ѼҸ�", "../Resources/Sounds/CharacterSound/Shotgun.mp3", 0, 1000);
	//SOUND_MGR->addSound("�����ѼҸ�", "../Resources/Sounds/CharacterSound/Shotgun.mp3", false, false);

	SOUND_MGR->add3DSound("����������", "../Resources/Sounds/CharacterSound/����������.mp3", 0, 1000);
	SOUND_MGR->add3DSound("��������", "../Resources/Sounds/CharacterSound/��������.mp3", 0, 1000);
	SOUND_MGR->add3DSound("��������", "../Resources/Sounds/CharacterSound/��������.mp3", 0, 1000);

	SOUND_MGR->add3DSound("�ѹٲٴ¼Ҹ�", "../Resources/Sounds/CharacterSound/�ѹٲٴ¼Ҹ�.mp3", 0, 1000);

	SOUND_MGR->addSound("ĳ���Ͱȱ�", "../Resources/Sounds/CharacterSound/.mp3", false, false);
	SOUND_MGR->addSound("ĳ���Ͷٱ�", "../Resources/Sounds/CharacterSound/�޸����׽�Ʈ2.mp3", false, false);//�޸����׽�Ʈ2d
	SOUND_MGR->add3DSound("ĳ���Ͷٱ�", "../Resources/Sounds/CharacterSound/�޸����׽�Ʈ2.mp3", 0, 1000);//�޸����׽�Ʈ3d
	SOUND_MGR->addSound("ĳ��������", "../Resources/Sounds/CharacterSound/�÷��̾�_����.mp3", false, false);
	SOUND_MGR->addSound("�����ǹ߾�", "../Resources/Sounds/CharacterSound/�����ǹ߾�.mp3", true, true);//�����ʿ�

	SOUND_MGR->addSound("������", "../Resources/Sounds/CharacterSound/������.mp3", false, false);
	SOUND_MGR->add3DSound("ĳ����Ÿ����", "../Resources/Sounds/CharacterSound/ĳ����Ÿ����.mp3", 0, 1000);
	SOUND_MGR->add3DSound("����Ÿ����", "../Resources/Sounds/CharacterSound/����Ÿ��.mp3", 0, 1000);

	SOUND_MGR->addSound("�κ��丮����", "../Resources/Sounds/CharacterSound/�κ��丮.mp3", false, false);
	SOUND_MGR->addSound("�̴ϸ�", "../Resources/Sounds/CharacterSound/�̴ϸ�.mp3", false, false);

	//����
	SOUND_MGR->add3DSound("������弦��", "../Resources/Sounds/MonsterSound/MonsterHeadshot.mp3", 0, 500);//��弦

	SOUND_MGR->add3DSound("Bandit����", "../Resources/Sounds/MonsterSound/Bandit����.mp3", 0, 500);//����
	SOUND_MGR->add3DSound("Skag����", "../Resources/Sounds/MonsterSound/Skag����.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid����", "../Resources/Sounds/MonsterSound/Droid����.mp3", 0, 500);

	SOUND_MGR->add3DSound("Bandit�ν�", "../Resources/Sounds/MonsterSound/Bandit�ν�.mp3", 0, 500);//�ν�
	SOUND_MGR->add3DSound("Skag�ν�", "../Resources/Sounds/MonsterSound/Skag�ν�.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid�ν�", "../Resources/Sounds/MonsterSound/Droid�ν�.mp3", 0, 500);

	SOUND_MGR->add3DSound("Bandit����", "../Resources/Sounds/MonsterSound/Bandit����.mp3", 0, 500);//����
	SOUND_MGR->add3DSound("Skag����", "../Resources/Sounds/MonsterSound/Skag����.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid����", "../Resources/Sounds/MonsterSound/Droid����.mp3", 0, 500);

	//����
	SOUND_MGR->add3DSound("BossDie", "../Resources/Sounds/BossSound/Boss����.mp3", 0, 2000);

	//������
	SOUND_MGR->addSound("����", "../Resources/Sounds/ObjectsSound/����.mp3", false, false);
	SOUND_MGR->add3DSound("���ڿ�����", "../Resources/Sounds/ObjectsSound/���ڿ�����.mp3", 0, 500);
	SOUND_MGR->add3DSound("�����۸�����", "../Resources/Sounds/ObjectsSound/�����۸�����.mp3", 0, 500);
	SOUND_MGR->add3DSound("��������", "../Resources/Sounds/ObjectsSound/��������.mp3", 0, 500);
	SOUND_MGR->add3DSound("�����������", "../Resources/Sounds/ObjectsSound/�����������.mp3", 0, 500);
	SOUND_MGR->addSound("�����̵�", "../Resources/Sounds/����.mp3", false, false);
	SOUND_MGR->addSound("��ø�", "../Resources/Sounds/��ø�.mp3", false, false);
	SOUND_MGR->addSound("�����", "../Resources/Sounds/�����.mp3", false, false);
	SOUND_MGR->addSound("����������", "../Resources/Sounds/����������.mp3", false, false);


	//����
	SOUND_MGR->add3DSound("��������", "../Resources/Sounds/BossSound/��������.mp3", 0, 500);
	SOUND_MGR->add3DSound("����������", "../Resources/Sounds/BossSound/����������.mp3", 0, 500);

	SOUND_MGR->add3DSound("�˼������.", "../Resources/Sounds/BossSound/�˼������.mp3", 0, 500);
	SOUND_MGR->add3DSound("�˼����ö�", "../Resources/Sounds/BossSound/�˼����ö�.mp3", 0, 500);
	SOUND_MGR->add3DSound("�˼�������", "../Resources/Sounds/BossSound/�˼�������.mp3", 0, 500);
	SOUND_MGR->add3DSound("�����̻���", "../Resources/Sounds/BossSound/�����̻���.mp3", 0, 500);
	SOUND_MGR->add3DSound("�˼�Ÿ��", "../Resources/Sounds/BossSound/����Ÿ����.mp3", 0, 500);

	//������� ����
	SOUND_MGR->bgmPlay("�������");
}

void cScene_inGame::InitForBoss()
{
	mBoss = new cThresher;
	mBoss->SetpMap(mMap);
	mBoss->SetpPlayer(mCharacter);
	mBoss->Init({ -350, -350 });
	COLLISIONMGR->SetBoss(mBoss);
}

void cScene_inGame::InitParticle()
{
{//��� ��1
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(3.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bl1.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.1f,
		0.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0),//ȸ��
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodFirst", q_particle);
}
{//��� ��2
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bl5.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodSecond", q_particle);
}
{//��� ��3
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(4.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);
	scales.push_back(1.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bloodSecond.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodThird", q_particle);
}
{//��� ��弦 ���� ��
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bl3.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodHeadshot", q_particle);
}
{//������ ��1-----------------------------------------------------------------------
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.8f, 0.0f, 0.8f));
	colors.push_back(D3DXCOLOR(0.0f, 0.6f, 0.0f, 0.6f));
	colors.push_back(D3DXCOLOR(0.0f, 0.4f, 0.0f, 0.4f));
	colors.push_back(D3DXCOLOR(0.0f, 0.2f, 0.0f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(3.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/alienBlood1.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.1f,
		0.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0),//ȸ��
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBlood1", q_particle);
}
{//������ ��2
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.8f, 0.0f, 0.8f));
	colors.push_back(D3DXCOLOR(0.0f, 0.6f, 0.0f, 0.6f));
	colors.push_back(D3DXCOLOR(0.0f, 0.4f, 0.0f, 0.4f));
	colors.push_back(D3DXCOLOR(0.0f, 0.2f, 0.0f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/alienBlood2.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBlood2", q_particle);
}
{//������ ��弦 ��
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.8f, 0.0f, 0.8f));
	colors.push_back(D3DXCOLOR(0.0f, 0.6f, 0.0f, 0.6f));
	colors.push_back(D3DXCOLOR(0.0f, 0.4f, 0.0f, 0.4f));
	colors.push_back(D3DXCOLOR(0.0f, 0.2f, 0.0f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(4.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);
	scales.push_back(1.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/alienBloodHeadshot.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBloodHeadshot", q_particle);
}
{//������ ��弦 ���� ��
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.8f, 0.0f, 0.8f));
	colors.push_back(D3DXCOLOR(0.0f, 0.6f, 0.0f, 0.6f));
	colors.push_back(D3DXCOLOR(0.0f, 0.4f, 0.0f, 0.4f));
	colors.push_back(D3DXCOLOR(0.0f, 0.2f, 0.0f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/alienBloodHead.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBloodHead", q_particle);
}
{//�κ���--------------------------------------------------------------------------------
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.6f, 0.6f, 0.5f, 1.0f));
	colors.push_back(D3DXCOLOR(0.5f, 0.5f, 0.4f, 0.8f));
	colors.push_back(D3DXCOLOR(0.4f, 0.4f, 0.3f, 0.6f));
	colors.push_back(D3DXCOLOR(0.3f, 0.3f, 0.2f, 0.4f));
	colors.push_back(D3DXCOLOR(0.2f, 0.2f, 0.1f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(7.0f);
	scales.push_back(5.0f);
	scales.push_back(4.0f);
	scales.push_back(3.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/rblood.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("robotblood", q_particle);
}
{//�κ���弦
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.6f, 0.6f, 0.5f, 1.0f));
	colors.push_back(D3DXCOLOR(0.5f, 0.5f, 0.4f, 0.8f));
	colors.push_back(D3DXCOLOR(0.4f, 0.4f, 0.3f, 0.6f));
	colors.push_back(D3DXCOLOR(0.3f, 0.3f, 0.2f, 0.4f));
	colors.push_back(D3DXCOLOR(0.2f, 0.2f, 0.1f, 0.2f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(7.0f);
	scales.push_back(6.0f);
	scales.push_back(5.0f);
	scales.push_back(4.0f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/rblood2.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, NULL, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		0.5f,
		1.5f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("robotbloodHeadshot", q_particle);
}

//�� ����Ʈ --------------------------------------------------------------------------------
{//�Ѿ� ������ ��ƼŬ
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(2.0f);
	scales.push_back(1.5f);
	scales.push_back(1.0f);
	scales.push_back(0.2f);


	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/endBullet.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		1.0f,
		3.0f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		0.5f,
		0.5f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("endBullet", q_particle);
}

//��� ������ΰ�------------------------------------------------------------------------------
{//����
	cQuadParticleEmitter* q_particle = new cQuadParticleEmitter();
	q_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.8f, 0.8f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.6f, 0.6f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.4f, 0.4f));
	colors.push_back(D3DXCOLOR(1.0f, 0.2f, 0.2f, 0.2f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));

	VEC_SCALE scales;
	scales.push_back(0.4f);
	scales.push_back(0.8f);
	scales.push_back(1.5f);
	scales.push_back(1.8f);

	LPDIRECT3DTEXTURE9   pNewTex = NULL;
	D3DXCreateTextureFromFileEx(Device, "../Resources/particle/smoke.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
		D3DX_DEFAULT, 0xff000000, 0, NULL, NULL, &pNewTex);
	q_particle->Init(
		10,
		10.0f,
		1.0f,
		3.0f,
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		D3DXVECTOR3(0, 1, 0),
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //�ʱ���۽� ȸ��Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //�ʱ���۽� ȸ��Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȸ�� �� Min
		D3DXVECTOR3(0, 0, 1),            //�ʴ� ȸ���� ȭ�� �� Max
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Min
		D3DXVECTOR3(0, 1, 0),            //�ʴ� ȸ�� ���� Max
		colors,
		scales,
		1.0f,
		1.5f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("smoke", q_particle);
}

//muzzle ����Ʈ
{//������ ����Ʈ
	cParticleMuzzlingEitter* e_particle = new cParticleMuzzlingEitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(9.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(8.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(7.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(6.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

	VEC_SCALE scales;
	scales.push_back(2.8f);
	scales.push_back(2.5f);
	scales.push_back(2.0f);
	scales.push_back(1.5f);
	scales.push_back(1.1f);

	scales.push_back(2.7f);
	scales.push_back(2.6f);
	scales.push_back(2.4f);
	scales.push_back(2.3f);
	scales.push_back(2.2f);

	scales.push_back(2.1f);
	scales.push_back(1.9f);
	scales.push_back(1.8f);
	scales.push_back(1.7f);
	scales.push_back(1.3f);

	scales.push_back(1.2f);
	scales.push_back(1.0f);
	scales.push_back(0.9f);
	scales.push_back(0.8f);
	scales.push_back(0.7f);

	scales.push_back(0.6f);
	scales.push_back(0.5f);
	scales.push_back(0.4f);
	scales.push_back(0.3f);
	scales.push_back(0.2f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/endBullet.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		3000,            //�ִ� ��ƼŬ ��
		500.0f,            //�ʴ� ��ƼŬ �߻� ��
		0.001,               //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.001,               //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0), //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,            //�÷� �迭
		scales,            //������ �迭
		0.1f,            //����ũ�� �ּҰ�
		0.3f,            //�ִ밪
		pTex,            //�ؽ���
		false);
	e_particle->ParticleBurst(5, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f);
	EFFECT_MGR->AddParticleMuzzleEmitter("rifleBurst", e_particle);
}
{//�������� ����Ʈ
	cParticleMuzzlingEitter* e_particle = new cParticleMuzzlingEitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 0.9f, 0.7f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.6f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.7f, 0.5f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.4f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.5f, 0.3f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.2f, 1.0f));

	VEC_SCALE scales;
	scales.push_back(3.5f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);
	scales.push_back(0.5f);
	scales.push_back(0.1f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/explosion.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		1000,            //�ִ� ��ƼŬ ��
		200.0f,            //�ʴ� ��ƼŬ �߻� ��
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(1, 1, 1),   //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(-1, -1, -1), //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,            //�÷� �迭
		scales,            //������ �迭
		0.1f,            //����ũ�� �ּҰ�
		0.3f,            //�ִ밪
		pTex,            //�ؽ���
		false);


	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("sniperBurst", e_particle);
}
{//���� ����Ʈ �׽�Ʈ
	cParticleMuzzlingEitter* e_particle = new cParticleMuzzlingEitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 5.0f, 4.0f, 1.0f));
	colors.push_back(D3DXCOLOR(9.0f, 4.0f, 3.0f, 1.0f));
	colors.push_back(D3DXCOLOR(8.0f, 3.0f, 2.0f, 1.0f));
	colors.push_back(D3DXCOLOR(7.0f, 2.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(6.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));

	VEC_SCALE scales;
	scales.push_back(3.5f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);
	scales.push_back(0.5f);
	scales.push_back(0.1f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/explosion.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		1000,            //�ִ� ��ƼŬ ��
		200.0f,            //�ʴ� ��ƼŬ �߻� ��
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(1, 1, 1),   //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(-1, -1, -1), //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,            //�÷� �迭
		scales,            //������ �迭
		0.1f,            //����ũ�� �ּҰ�
		0.3f,            //�ִ밪
		pTex,            //�ؽ���
		false);

	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("shotgunBurst", e_particle);
}
{//����ȿ�� ����Ʈ
	cParticleMuzzlingEitter* e_particle = new cParticleMuzzlingEitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 0.9f, 0.7f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.8f, 0.6f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.7f, 0.5f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.6f, 0.4f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.5f, 0.3f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.4f, 0.2f, 1.0f));

	VEC_SCALE scales;
	scales.push_back(3.5f);
	scales.push_back(3.0f);
	scales.push_back(2.0f);
	scales.push_back(0.5f);
	scales.push_back(0.1f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/explosion.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		1000,            //�ִ� ��ƼŬ ��
		200.0f,            //�ʴ� ��ƼŬ �߻� ��
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.1,               //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0, 0),   //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(1, 1, 1),   //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(-1, -1, -1), //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,            //�÷� �迭
		scales,            //������ �迭
		0.1f,            //����ũ�� �ּҰ�
		0.3f,            //�ִ밪
		pTex,            //�ؽ���
		false);


	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("����ȿ��", e_particle);
}


//������
{//��
	cPartcleEmitter* e_particle = new cPartcleEmitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));


	VEC_SCALE scales;
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/fire.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		3000,                        //�ִ� ��ƼŬ ��
		1000.0f,                     //�ʴ� ��ƼŬ �߻� ��
		0.3f,                        //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.5f,                        //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,                        //�÷� �迭
		scales,                        //������ �迭
		0.2f,                        //����ũ�� �ּҰ�
		0.5f,                        //�ִ밪
		pTex                        //�ؽ���
	);
	EFFECT_MGR->AddParticleEmitter("moneyItemParticle", e_particle);
}
{//����
	cPartcleEmitter* e_particle = new cPartcleEmitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));


	VEC_SCALE scales;
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/fire.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		3000,                        //�ִ� ��ƼŬ ��
		1000.0f,                     //�ʴ� ��ƼŬ �߻� ��
		0.3f,                        //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.5f,                        //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,                        //�÷� �迭
		scales,                        //������ �迭
		0.5f,                        //����ũ�� �ּҰ�
		0.5f,                        //�ִ밪
		pTex                        //�ؽ���
	);
	EFFECT_MGR->AddParticleEmitter("healpakItemParticle", e_particle);
}
{//�Ѿ� ������
	cPartcleEmitter* e_particle = new cPartcleEmitter();
	e_particle->SetActive(true);

	VEC_COLOR colors;
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));


	VEC_SCALE scales;
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);
	scales.push_back(1.0f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource(
		"../Resources/particle/fire.png");

	//��ƼŬ ������ ����
	e_particle->Init(
		3000,                        //�ִ� ��ƼŬ ��
		1000.0f,                     //�ʴ� ��ƼŬ �߻� ��
		0.3f,                        //�ϳ��� ��ƼŬ ���� ������ �ּҰ�
		0.5f,                        //�ϳ��� ��ƼŬ ���� ������ �ִ밪
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 7, 0),            //��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
		colors,                        //�÷� �迭
		scales,                        //������ �迭
		0.5f,                        //����ũ�� �ּҰ�
		0.5f,                        //�ִ밪
		pTex                        //�ؽ���
	);
	EFFECT_MGR->AddParticleEmitter("bulletItemParticle", e_particle);
}
}

void cScene_inGame::LoadObjects(string fileName)
{
	vObjects.clear();
	vCullingObjects.clear();
	COLLISIONMGR->ClearObjects();
	mCharacter->ClearObjects();
	vItems.clear();
	vCullingItems.clear();
	sObjectSaveData tmpData[OBJECTMAXNUM];
	ZeroMemory(&tmpData, OBJECTMAXNUM);
	HANDLE file;
	DWORD read;
	file = CreateFile(fileName.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	ReadFile(file, &tmpData, sizeof(sObjectSaveData)*OBJECTMAXNUM, &read, NULL);
	CloseHandle(file);

	char tmpEquip[100];
	sprintf(tmpEquip, "../Resources/Meshes/Objects/equipVendingMachine.X", 100);
	char tmpSave[100];
	sprintf(tmpSave, "../Resources/Meshes/Objects/saveVendingMachine.X", 100);
	char tmpItem[100];
	sprintf(tmpItem, "../Resources/Meshes/Objects/itemVendingMachine.X", 100);
	for (int i = 0; i < OBJECTMAXNUM; ++i) {
		if (tmpData[i].isSet == false) break;
		if (tmpData[i].haveAnimation) {
			cSkelObject* tmpSkel = new cSkelObject();
			tmpSkel->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis);
			char tmp[100];
			sprintf(tmp, "../Resources/Meshes/Objects/DahlEpicCrate.X", 100);
			if (!strcmp(tmpData[i].sfilePath, tmp)) {
				cItem* tmp = new cItem();
				tmp->Init(mCharacter->GetPlayerInfo(), (eItemKind)RandomIntRange(0, 5), { 0,0,0 });
				tmpSkel->GetSkinned()->AddBoneTransform("TrayLeft", tmp->pTransform);
				tmp->SetInBox(tmpSkel->GetIsOpen());
				vItems.push_back(tmp);
				tmp = new cItem();
				tmp->Init(mCharacter->GetPlayerInfo(), (eItemKind)RandomIntRange(0, 5), { 0,0,0 });
				tmpSkel->GetSkinned()->AddBoneTransform("TrayRight", tmp->pTransform);
				tmp->SetInBox(tmpSkel->GetIsOpen());
				vItems.push_back(tmp);
			}
			else {
				cItem* tmp = new cItem();
				tmp->Init(mCharacter->GetPlayerInfo(), (eItemKind)RandomIntRange(0, 5), { 0,0,0 });
				tmpSkel->GetSkinned()->AddBoneTransform("Bone_Top_Shelf_L", tmp->pTransform);
				tmp->SetInBox(tmpSkel->GetIsOpen());
				vItems.push_back(tmp);
			}
			vObjects.push_back(tmpSkel);
			COLLISIONMGR->AddObejct(tmpSkel);
		}
		else {
			if (!strcmp(tmpData[i].sfilePath, tmpEquip)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, EquipmentStore);
				tmpVending->SetCharAndInven(mCharacter, mCharacter->GetInven());
				vObjects.push_back(tmpVending);
				COLLISIONMGR->AddObejct(tmpVending);
			}
			else if (!strcmp(tmpData[i].sfilePath, tmpSave)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, SaveStore);
				tmpVending->SetCharAndInven(mCharacter, mCharacter->GetInven());
				vObjects.push_back(tmpVending);
				COLLISIONMGR->AddObejct(tmpVending);
			}
			else if (!strcmp(tmpData[i].sfilePath, tmpItem)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, ItemStore);
				tmpVending->SetCharAndInven(mCharacter, mCharacter->GetInven());
				vObjects.push_back(tmpVending);
				COLLISIONMGR->AddObejct(tmpVending);
			}
			else {
				cStaticObject* tmpSkel = new cStaticObject();
				tmpSkel->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis);
				vObjects.push_back(tmpSkel);
				COLLISIONMGR->AddObejct(tmpSkel);
			}
		}
	}

	mCharacter->SetObjects(vObjects);
}

void cScene_inGame::LoadEnemys(string fileName)
{
	vEnemies.clear();
	vCullingEnemies.clear();
	COLLISIONMGR->ClearEnemys();
	sEnemySaveData tmpData[ENEMYMAXNUM];
	ZeroMemory(&tmpData, ENEMYMAXNUM);
	HANDLE file;
	DWORD read;
	file = CreateFile(fileName.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	ReadFile(file, &tmpData, sizeof(sEnemySaveData)*ENEMYMAXNUM, &read, NULL);
	CloseHandle(file);

	char tmpBandit[100];
	sprintf(tmpBandit, "../Resources/Meshes/enemy/Bandit/Bandit.X", 100);
	char tmpSkag[100];
	sprintf(tmpSkag, "../Resources/Meshes/enemy/Skag/Skag.X", 100);
	char tmpDroid[100];
	sprintf(tmpDroid, "../Resources/Meshes/enemy/BattleDroid/BattleDroid.X", 100);
	for (int i = 0; i < ENEMYMAXNUM; ++i) {
		if (tmpData[i].isSet == false) break;
		if (!strcmp(tmpData[i].sfilePath, tmpBandit)) {
			cBandit* tmpEnemy = new cBandit;
			tmpEnemy->SetMap(mMap);
			tmpEnemy->SetendXZ(mCharacter->GetendX(), mCharacter->GetendZ());
			tmpEnemy->SetpPlayer(mCharacter);
			D3DXVECTOR2 pos = { tmpData[i].position.x ,tmpData[i].position.z };
			tmpEnemy->Init(pos);
			COLLISIONMGR->AddEnemy(tmpEnemy);
			vEnemies.push_back(tmpEnemy);
		}
		else if (!strcmp(tmpData[i].sfilePath, tmpSkag)) {
			cSkag* tmpEnemy = new cSkag;
			tmpEnemy->SetMap(mMap);
			tmpEnemy->SetendXZ(mCharacter->GetendX(), mCharacter->GetendZ());
			tmpEnemy->SetpPlayer(mCharacter);
			D3DXVECTOR2 pos = { tmpData[i].position.x ,tmpData[i].position.z };
			tmpEnemy->Init(pos);
			COLLISIONMGR->AddEnemy(tmpEnemy);
			vEnemies.push_back(tmpEnemy);
		}
		else if (!strcmp(tmpData[i].sfilePath, tmpDroid)) {
			cBattleDroid* tmpEnemy = new cBattleDroid;
			tmpEnemy->SetMap(mMap);
			tmpEnemy->SetendXZ(mCharacter->GetendX(), mCharacter->GetendZ());
			tmpEnemy->SetpPlayer(mCharacter);
			D3DXVECTOR2 pos = { tmpData[i].position.x ,tmpData[i].position.z };
			tmpEnemy->Init(pos);
			COLLISIONMGR->AddEnemy(tmpEnemy);
			vEnemies.push_back(tmpEnemy);
		}
	}
}

void cScene_inGame::MoveMap(bool isTown)
{
	if (isTown) {
		SOUND_MGR->Allstop();//����->�ʵ� ,���彺ž
		SOUND_MGR->bgmPlay("����͹��");
		SOUND_MGR->update();
		mMap->ChageMap(false);
		COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
		//mCharacter->GetpPlayerXZTransform()->SetWorldPosition(-162, 0, 234); �����
		mCharacter->GetpPlayerXZTransform()->SetWorldPosition(420, 0, 420);
		//mCharacter->GetpPlayerXZTransform()->SetWorldPosition(-410, 0, -410);
		LoadEnemys("zoneFile/Enemy.save");
		LoadObjects("zoneFile/Object.save");
		InitForBoss();
		mBossTriggerTransform = new cTransform();
		mBossTriggerTransform->SetWorldPosition(-103, 10, -137);
		mBossTriggerBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(30, 30, 10));
		this->SetEnvironment("../Resources/TextureCUBE/cubetest.dds");
	}
	else {
		SOUND_MGR->Allstop();//�ʵ�->���� ,���彺ž
		SOUND_MGR->bgmPlay("�������");
		SOUND_MGR->update();
		COLLISIONMGR->ClearBoss();
		SAFE_DELETE(mBossTriggerTransform);
		vEnemies.clear();
		vCullingEnemies.clear();
		COLLISIONMGR->ClearEnemys();
		mMap->ChageMap(true);
		COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
		mCharacter->GetpPlayerXZTransform()->SetWorldPosition(0, 0, 0);
		LoadObjects("townFile/Object.save");
		this->SetEnvironment("../Resources/TextureCUBE/CubeMap_BlackNight.dds");
	}
}

void cScene_inGame::MoveMapForDie(bool isTown)
{
	if (isTown) {
		SOUND_MGR->Allstop();//����->�ʵ� ,���彺ž
		SOUND_MGR->bgmPlay("����͹��");
		mMap->ChageMap(false);
		COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
		LoadEnemys("zoneFile/Enemy.save");
		LoadObjects("zoneFile/Object.save");
		InitForBoss();
		mBossTriggerTransform = new cTransform();
		mBossTriggerTransform->SetWorldPosition(-103, 10, -137);
		mBossTriggerBoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(30, 30, 10));
		this->SetEnvironment("../Resources/TextureCUBE/cubetest.dds");
	}
	else {
		SOUND_MGR->Allstop();//�ʵ�->���� ,���彺ž
		SOUND_MGR->bgmPlay("�������");
		COLLISIONMGR->ClearBoss();
		SAFE_DELETE(mBossTriggerTransform);
		vEnemies.clear();
		vCullingEnemies.clear();
		COLLISIONMGR->ClearEnemys();
		mMap->ChageMap(true);
		COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
		LoadObjects("townFile/Object.save");
		this->SetEnvironment("../Resources/TextureCUBE/CubeMap_BlackNight.dds");
	}
}

void cScene_inGame::RenderShadow()
{
	this->pDirectionLightCamera->UpdateMatrix();
	cXMesh_Static::SetCamera(this->pDirectionLightCamera);
	cXMesh_Skinned::SetCamera(this->pDirectionLightCamera);

	this->pDirectionLightCamera->RenderShadowBegin();

	for (int i = 0; i < vCullingEnemies.size(); i++) {
		vCullingEnemies.at(i)->RenderShadow();
	}
	for (int i = 0; i < vCullingObjects.size(); i++) {
		vCullingObjects.at(i)->RenderShadow();
	}

	if (!mMap->GetIsTown()) {
		mBoss->RenderShadow();
	}

	this->pDirectionLightCamera->RenderShadowEnd();
}
