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
	SOUND_MGR->Allstop();//메뉴 bgm스탑

	SCENE_MGR->fProgress = 0.0f;
	SCENE_MGR->fString = "세계 생성 중";
	mMap = new cMap();
	mMap->Init();


	COLLISIONMGR->Init();
	COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
	

	SCENE_MGR->fProgress = 20.0f;
	SCENE_MGR->fString = "캐릭터 생성 중";
	mCharacter = new Character();
	mCharacter->SetCamera(pMainCamera);
	mCharacter->Init();
	mCharacter->SetGameKey(&gGameKey);
	mCharacter->SetTerrain(mMap->GetpTerrain());
	mCharacter->SetMap(mMap);
	COLLISIONMGR->SetPlayer(mCharacter);

	SCENE_MGR->fProgress = 40.0f;
	SCENE_MGR->fString = "오브젝트,적 생성 중";
	LoadObjects("townFile/Object.save");

	SCENE_MGR->fProgress = 60.0f;
	SCENE_MGR->fString = "이펙트 정리 중";
	EFFECT_MGR->Init();
	EFFECT_MGR->SetCamera(pMainCamera);

	SCENE_MGR->fProgress = 80.0f;
	SCENE_MGR->fString = "파티클 정리 중";
	InitParticle();

	this->SetEnvironment("../Resources/TextureCUBE/CubeMap_BlackNight.dds");
	//this->SetEnvironment("../Resources/TextureCUBE/cubetest.dds");
	this->pSceneBaseDirectionLight->pTransform->RotateWorld(
		90.0f * ONE_RAD, 0, 0);

	SCENE_MGR->fProgress = 100.0f;
	SCENE_MGR->fString = "로딩 끝";
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

	//매니져 해제
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
			if (!vEnemies[i]->isLive) {//리스폰
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
			//적 폰트보기
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
			//폰트보기 끝
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
	//메뉴
	//SOUND_MGR->addSound("타이틀메뉴", "../Resources/Sounds/MenuSound.mp3", true, true);
	SOUND_MGR->addSound("마을배경", "../Resources/Sounds/bgm3.mp3", true, true);
	SOUND_MGR->addSound("사냥터배경", "../Resources/Sounds/사냥터배경음.mp3", true, true);
	SOUND_MGR->addSound("보스배경", "../Resources/Sounds/BossSound/BossBgm1.mp3", true, true);
	SOUND_MGR->addSound("게임클리어", "../Resources/Sounds/GameClear.mp3", true, true);

	//SOUND_MGR->addSound("메뉴버튼", "../Resources/Sounds/MenuButtonClick.mp3", false, false);
	SOUND_MGR->addSound("버튼클릭", "../Resources/Sounds/ButtonClick2.mp3", false, false);

	SOUND_MGR->addSound("보스출현2", "../Resources/Sounds/BossSound/보스출현2.mp3", false, false);

	//캐릭터
	SOUND_MGR->add3DSound("라이플총소리", "../Resources/Sounds/CharacterSound/Rifle.mp3", 0, 1000);
	//SOUND_MGR->addSound("라이플총소리", "../Resources/Sounds/CharacterSound/Rifle.mp3", false, false);
	SOUND_MGR->add3DSound("스나이퍼총소리", "../Resources/Sounds/CharacterSound/Sniper.mp3", 0, 1000);
	//SOUND_MGR->addSound("스나이퍼총소리", "../Resources/Sounds/CharacterSound/Sniper.mp3", false, false);
	SOUND_MGR->add3DSound("샷건총소리", "../Resources/Sounds/CharacterSound/Shotgun.mp3", 0, 1000);
	//SOUND_MGR->addSound("샷건총소리", "../Resources/Sounds/CharacterSound/Shotgun.mp3", false, false);

	SOUND_MGR->add3DSound("라이플장전", "../Resources/Sounds/CharacterSound/라이플장전.mp3", 0, 1000);
	SOUND_MGR->add3DSound("스나장전", "../Resources/Sounds/CharacterSound/스나장전.mp3", 0, 1000);
	SOUND_MGR->add3DSound("샷건장전", "../Resources/Sounds/CharacterSound/샷건장전.mp3", 0, 1000);

	SOUND_MGR->add3DSound("총바꾸는소리", "../Resources/Sounds/CharacterSound/총바꾸는소리.mp3", 0, 1000);

	SOUND_MGR->addSound("캐릭터걷기", "../Resources/Sounds/CharacterSound/.mp3", false, false);
	SOUND_MGR->addSound("캐릭터뛰기", "../Resources/Sounds/CharacterSound/달리기테스트2.mp3", false, false);//달리기테스트2d
	SOUND_MGR->add3DSound("캐릭터뛰기", "../Resources/Sounds/CharacterSound/달리기테스트2.mp3", 0, 1000);//달리기테스트3d
	SOUND_MGR->addSound("캐릭터점프", "../Resources/Sounds/CharacterSound/플레이어_점프.mp3", false, false);
	SOUND_MGR->addSound("최후의발악", "../Resources/Sounds/CharacterSound/최후의발악.mp3", true, true);//루프필요

	SOUND_MGR->addSound("레벨업", "../Resources/Sounds/CharacterSound/레벨업.mp3", false, false);
	SOUND_MGR->add3DSound("캐릭터타격음", "../Resources/Sounds/CharacterSound/캐릭터타격음.mp3", 0, 1000);
	SOUND_MGR->add3DSound("쉴드타격음", "../Resources/Sounds/CharacterSound/쉴드타격.mp3", 0, 1000);

	SOUND_MGR->addSound("인벤토리열때", "../Resources/Sounds/CharacterSound/인벤토리.mp3", false, false);
	SOUND_MGR->addSound("미니맵", "../Resources/Sounds/CharacterSound/미니맵.mp3", false, false);

	//몬스터
	SOUND_MGR->add3DSound("몬스터헤드샷음", "../Resources/Sounds/MonsterSound/MonsterHeadshot.mp3", 0, 500);//헤드샷

	SOUND_MGR->add3DSound("Bandit공격", "../Resources/Sounds/MonsterSound/Bandit공격.mp3", 0, 500);//공격
	SOUND_MGR->add3DSound("Skag공격", "../Resources/Sounds/MonsterSound/Skag공격.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid공격", "../Resources/Sounds/MonsterSound/Droid공격.mp3", 0, 500);

	SOUND_MGR->add3DSound("Bandit인식", "../Resources/Sounds/MonsterSound/Bandit인식.mp3", 0, 500);//인식
	SOUND_MGR->add3DSound("Skag인식", "../Resources/Sounds/MonsterSound/Skag인식.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid인식", "../Resources/Sounds/MonsterSound/Droid인식.mp3", 0, 500);

	SOUND_MGR->add3DSound("Bandit죽음", "../Resources/Sounds/MonsterSound/Bandit죽음.mp3", 0, 500);//죽음
	SOUND_MGR->add3DSound("Skag죽음", "../Resources/Sounds/MonsterSound/Skag죽음.mp3", 0, 500);
	SOUND_MGR->add3DSound("Droid죽음", "../Resources/Sounds/MonsterSound/Droid죽음.mp3", 0, 500);

	//보스
	SOUND_MGR->add3DSound("BossDie", "../Resources/Sounds/BossSound/Boss죽음.mp3", 0, 2000);

	//아이템
	SOUND_MGR->addSound("구매", "../Resources/Sounds/ObjectsSound/구매.mp3", false, false);
	SOUND_MGR->add3DSound("상자열림음", "../Resources/Sounds/ObjectsSound/상자열림음.mp3", 0, 500);
	SOUND_MGR->add3DSound("아이템먹을때", "../Resources/Sounds/ObjectsSound/아이템먹을때.mp3", 0, 500);
	SOUND_MGR->add3DSound("상점열때", "../Resources/Sounds/ObjectsSound/상점열때.mp3", 0, 500);
	SOUND_MGR->add3DSound("저장상점열때", "../Resources/Sounds/ObjectsSound/저장상점열때.mp3", 0, 500);
	SOUND_MGR->addSound("텔포이동", "../Resources/Sounds/워프.mp3", false, false);
	SOUND_MGR->addSound("잠시만", "../Resources/Sounds/잠시만.mp3", false, false);
	SOUND_MGR->addSound("변기속", "../Resources/Sounds/변기속.mp3", false, false);
	SOUND_MGR->addSound("돌내려갈때", "../Resources/Sounds/돌내려갈때.mp3", false, false);


	//보스
	SOUND_MGR->add3DSound("보스출현", "../Resources/Sounds/BossSound/보스출현.mp3", 0, 500);
	SOUND_MGR->add3DSound("보스죽을때", "../Resources/Sounds/BossSound/보스죽을때.mp3", 0, 500);

	SOUND_MGR->add3DSound("촉수경고음.", "../Resources/Sounds/BossSound/촉수경고음.mp3", 0, 500);
	SOUND_MGR->add3DSound("촉수나올때", "../Resources/Sounds/BossSound/촉수나올때.mp3", 0, 500);
	SOUND_MGR->add3DSound("촉수죽을때", "../Resources/Sounds/BossSound/촉수죽을때.mp3", 0, 500);
	SOUND_MGR->add3DSound("보스미사일", "../Resources/Sounds/BossSound/보스미사일.mp3", 0, 500);
	SOUND_MGR->add3DSound("촉수타격", "../Resources/Sounds/BossSound/보스타격음.mp3", 0, 500);

	//마을배경 실행
	SOUND_MGR->bgmPlay("마을배경");
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
{//사람 피1
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0),//회전
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodFirst", q_particle);
}
{//사람 피2
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodSecond", q_particle);
}
{//사람 피3
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodThird", q_particle);
}
{//사람 헤드샷 보조 피
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("bloodHeadshot", q_particle);
}
{//강아지 피1-----------------------------------------------------------------------
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0),//회전
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBlood1", q_particle);
}
{//강아지 피2
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBlood2", q_particle);
}
{//강아지 헤드샷 피
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBloodHeadshot", q_particle);
}
{//강아지 헤드샷 보조 피
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("alienBloodHead", q_particle);
}
{//로봇피--------------------------------------------------------------------------------
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("robotblood", q_particle);
}
{//로봇헤드샷
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.0f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("robotbloodHeadshot", q_particle);
}

//총 이펙트 --------------------------------------------------------------------------------
{//총알 밖히는 파티클
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		0.5f,
		0.5f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("endBullet", q_particle);
}

//사용 대기중인것------------------------------------------------------------------------------
{//연기
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
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),      //초기시작시 회전Min
		//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
		D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
		D3DXVECTOR3(0, 0, 0),
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 회전 량 Min
		D3DXVECTOR3(0, 0, 1),            //초당 회전할 화전 량 Max
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Min
		D3DXVECTOR3(0, 1, 0),            //초당 회전 가속 Max
		colors,
		scales,
		1.0f,
		1.5f,
		pNewTex,
		true);
	EFFECT_MGR->AddQuadParticleEmitter("smoke", q_particle);
}

//muzzle 이펙트
{//라이플 버스트
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		3000,            //최대 파티클 수
		500.0f,            //초당 파티클 발생 량
		0.001,               //하나의 파티클 입자 라이프 최소값
		0.001,               //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0), //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,            //컬러 배열
		scales,            //스케일 배열
		0.1f,            //입자크기 최소값
		0.3f,            //최대값
		pTex,            //텍스쳐
		false);
	e_particle->ParticleBurst(5, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.0f, 0.0f);
	EFFECT_MGR->AddParticleMuzzleEmitter("rifleBurst", e_particle);
}
{//스나이퍼 버스트
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		1000,            //최대 파티클 수
		200.0f,            //초당 파티클 발생 량
		0.1,               //하나의 파티클 입자 라이프 최소값
		0.1,               //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(1, 1, 1),   //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(-1, -1, -1), //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,            //컬러 배열
		scales,            //스케일 배열
		0.1f,            //입자크기 최소값
		0.3f,            //최대값
		pTex,            //텍스쳐
		false);


	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("sniperBurst", e_particle);
}
{//샷건 버스트 테스트
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		1000,            //최대 파티클 수
		200.0f,            //초당 파티클 발생 량
		0.1,               //하나의 파티클 입자 라이프 최소값
		0.1,               //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(1, 1, 1),   //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(-1, -1, -1), //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,            //컬러 배열
		scales,            //스케일 배열
		0.1f,            //입자크기 최소값
		0.3f,            //최대값
		pTex,            //텍스쳐
		false);

	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("shotgunBurst", e_particle);
}
{//보조효과 버스트
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		1000,            //최대 파티클 수
		200.0f,            //초당 파티클 발생 량
		0.1,               //하나의 파티클 입자 라이프 최소값
		0.1,               //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0, 0),   //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(1, 1, 1),   //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(-1, -1, -1), //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,            //컬러 배열
		scales,            //스케일 배열
		0.1f,            //입자크기 최소값
		0.3f,            //최대값
		pTex,            //텍스쳐
		false);


	e_particle->ParticleBurst(5, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, -2.0f, 50.0f, 50.0f, 0.0f, 0.0f);

	EFFECT_MGR->AddParticleMuzzleEmitter("보조효과", e_particle);
}


//아이템
{//돈
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		3000,                        //최대 파티클 수
		1000.0f,                     //초당 파티클 발생 량
		0.3f,                        //하나의 파티클 입자 라이프 최소값
		0.5f,                        //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,                        //컬러 배열
		scales,                        //스케일 배열
		0.2f,                        //입자크기 최소값
		0.5f,                        //최대값
		pTex                        //텍스쳐
	);
	EFFECT_MGR->AddParticleEmitter("moneyItemParticle", e_particle);
}
{//힐팩
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		3000,                        //최대 파티클 수
		1000.0f,                     //초당 파티클 발생 량
		0.3f,                        //하나의 파티클 입자 라이프 최소값
		0.5f,                        //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,                        //컬러 배열
		scales,                        //스케일 배열
		0.5f,                        //입자크기 최소값
		0.5f,                        //최대값
		pTex                        //텍스쳐
	);
	EFFECT_MGR->AddParticleEmitter("healpakItemParticle", e_particle);
}
{//총알 아이템
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

	//파티클 랜더러 셋팅
	e_particle->Init(
		3000,                        //최대 파티클 수
		1000.0f,                     //초당 파티클 발생 량
		0.3f,                        //하나의 파티클 입자 라이프 최소값
		0.5f,                        //하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 7, 0),            //파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.2f, -0.5f),      //파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, 0.4f, -1.0f),      //파티클 입자 가속 최대값 ( 로컬기준 )
		colors,                        //컬러 배열
		scales,                        //스케일 배열
		0.5f,                        //입자크기 최소값
		0.5f,                        //최대값
		pTex                        //텍스쳐
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
		SOUND_MGR->Allstop();//마을->필드 ,사운드스탑
		SOUND_MGR->bgmPlay("사냥터배경");
		SOUND_MGR->update();
		mMap->ChageMap(false);
		COLLISIONMGR->SetTerrain(mMap->GetpTerrain());
		//mCharacter->GetpPlayerXZTransform()->SetWorldPosition(-162, 0, 234); 변기앞
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
		SOUND_MGR->Allstop();//필드->마을 ,사운드스탑
		SOUND_MGR->bgmPlay("마을배경");
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
		SOUND_MGR->Allstop();//마을->필드 ,사운드스탑
		SOUND_MGR->bgmPlay("사냥터배경");
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
		SOUND_MGR->Allstop();//필드->마을 ,사운드스탑
		SOUND_MGR->bgmPlay("마을배경");
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
