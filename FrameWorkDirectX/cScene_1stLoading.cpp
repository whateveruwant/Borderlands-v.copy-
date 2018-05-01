#include "StdAfx.h"
#include "cScene_1stLoading.h"
#include "cLight_Direction.h"
#include "cXMesh_Static.h"
#include "cBaseObject.h"
#include "cTransform.h"
#include "cCamera.h"
#include "cTerrain.h"
#include "cSkinnedAnimation.h"
#include "cLight_Point.h"
#include "cStaticObject.h"
#include "cSkelObject.h"
#include "time.h"
#include "cVendingObject.h"

cScene_1stLoading::cScene_1stLoading(void)
{
}

cScene_1stLoading::~cScene_1stLoading(void)
{
}

HRESULT cScene_1stLoading::Scene_Init()
{

	srand(time(NULL));
	mMap = new cMap();
	mMap->Init();


	COLLISIONMGR->Init();
	COLLISIONMGR->SetTerrain(mMap->GetpTerrain());

	mCharacter = new Character();
	mCharacter->SetCamera(pMainCamera);
	mCharacter->Init();
	mCharacter->SetGameKey(&gGameKey);
	mCharacter->SetTerrain(mMap->GetpTerrain());
	mCharacter->SetMap(mMap);
	COLLISIONMGR->SetPlayer(mCharacter);

	this->SetEnvironment("../Resources/TextureCUBE/grassenvmap1024.dds");
	this->pSceneBaseDirectionLight->pTransform->RotateWorld(
		90.0f * ONE_RAD, 0, 0);

	EFFECT_MGR->Init();
	EFFECT_MGR->SetCamera(pMainCamera);
	InitParticle();
	LoadObjects();
	return S_OK;
}

void cScene_1stLoading::Scene_Release()
{
	mCharacter->Release();
	for (int i = 0; i < vObjects.size(); ++i) {
		SAFE_DELETE(vObjects[i]);
	}
	for (int i = 0; i < vItems.size(); ++i) {
		SAFE_DELETE(vItems[i]);
	}
	//매니져 해제
	COLLISIONMGR->Release();
	cBulletCollisionMGR::ReleaseInstance();
	EFFECT_MGR->Release();
	cEffectMGR::ReleaseInstance();

}

void cScene_1stLoading::Scene_Update(float timeDelta)
{
	ShowCursor(FALSE);
	mMap->Update(timeDelta);
	mCharacter->Update(timeDelta);
	Culling(timeDelta);

	this->pSceneBaseDirectionLight->pTransform->DefaultControl2(timeDelta);
	COLLISIONMGR->Update(timeDelta);
	EFFECT_MGR->Update(timeDelta);
}

void cScene_1stLoading::Scene_Render1()
{
	mMap->Render(pMainCamera, pSceneBaseDirectionLight, pDirectionLightCamera);
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->Render();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->Render();
	}
	mCharacter->Render();
	COLLISIONMGR->Render();
	cXMesh_Skinned::SetBaseLight(this->pSceneBaseDirectionLight);
	cXMesh_Skinned::SetCamera(this->pMainCamera);
	cXMesh_Static::SetBaseLight(this->pSceneBaseDirectionLight);
	cXMesh_Static::SetCamera(this->pMainCamera);
}


void cScene_1stLoading::Scene_RenderSprite()
{
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->RenderSprite();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->RenderSprite();
	}
	mCharacter->Scene_RenderSprite();
}

void cScene_1stLoading::Scene_RenderFont()
{
	mCharacter->Scene_RenderFont();
	for (int i = 0; i < vCullingObjects.size(); ++i) {
		vCullingObjects[i]->RenderFont();
	}
	for (int i = 0; i < vCullingItems.size(); ++i) {
		vCullingItems[i]->RenderFont();
	}
}

void cScene_1stLoading::Culling(float timeDelta)
{
	this->pMainCamera->UpdateFrustum();
	this->vCullingObjects.clear();
	this->vCullingItems.clear();

	for (int i = 0; i < vObjects.size(); ++i) {
		if (this->pMainCamera->Frustum.IsInFrustum(this->vObjects[i]))
			this->vCullingObjects.push_back(this->vObjects[i]);
		vObjects[i]->Update(timeDelta);
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
		case eObjectKind_static:
			break;
		case eObjectKind_vending:
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
			break;
		}

	}
	for (int i = 0; i < vItems.size(); ++i) {
		if (this->pMainCamera->Frustum.IsInFrustum(this->vItems[i]))
			this->vCullingItems.push_back(this->vItems[i]);
		if (!vItems[i]->IsActive()) continue;;
		vItems[i]->Update(timeDelta);
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

void cScene_1stLoading::InitParticle()
{
	{//피1
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

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0),//회전
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bloodFirst", q_particle);
	}
	{//피2
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

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bloodSecond", q_particle);
	}
	{//헤드샷 피
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

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bloodHeadshot", q_particle);
	}
	{//로봇피
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

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("robotblood", q_particle);
	}


	//////////////////////////////////////////////////////////////////////////////////////////
	{//총구 이펙트(라플)
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
		scales.push_back(1.0f);
		scales.push_back(1.0f);
		scales.push_back(1.0f);
		scales.push_back(1.0f);

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
		D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bulletfireRifle.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0),//-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bulletFireRifle", q_particle);
	}
	{//스나
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
		scales.push_back(1.5f);
		scales.push_back(1.5f);
		scales.push_back(1.5f);
		scales.push_back(1.5f);

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
		D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bulletfireSniper.png", 256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bulletFireSniper", q_particle);
	}
	{//샷건
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
		scales.push_back(1.5f);
		scales.push_back(1.5f);
		scales.push_back(1.5f);
		scales.push_back(1.5f);

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
		D3DXCreateTextureFromFileEx(Device, "../Resources/particle/bulletfireShotgun.png", 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			1.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("bulletFireShotgun", q_particle);
	}
	{//연기
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
		scales.push_back(0.5f);
		scales.push_back(0.8f);
		scales.push_back(1.0f);
		scales.push_back(1.2f);

		LPDIRECT3DTEXTURE9	pNewTex = NULL;
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
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),		//초기시작시 회전Min
			//D3DXVECTOR3(-90 * ONE_RAD, 0, 0),     //초기시작시 회전Max
			D3DXVECTOR3(0, 0, 0), //-90 * ONE_RAD),
			D3DXVECTOR3(0, 0, 0),
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 회전 량 Min
			D3DXVECTOR3(0, 0, 1),				//초당 회전할 화전 량 Max
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Min
			D3DXVECTOR3(0, 1, 0),				//초당 회전 가속 Max
			colors,
			scales,
			1.0f,
			2.0f,
			pNewTex,
			true);
		EFFECT_MGR->AddQuadParticleEmitter("smoke", q_particle);
	}
}

void cScene_1stLoading::LoadObjects()
{
	sObjectSaveData tmpData[OBJECTMAXNUM];
	ZeroMemory(&tmpData, OBJECTMAXNUM);
	HANDLE file;
	DWORD read;
	file = CreateFile("townFile/townFinal.save",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	ReadFile(file, &tmpData, sizeof(sObjectSaveData)*OBJECTMAXNUM, &read, NULL);
	CloseHandle(file);

	for (int i = 0; i < OBJECTMAXNUM; ++i) {
		if (tmpData[i].isSet == false) break;
		if (tmpData[i].haveAnimation) {
			cSkelObject* tmpSkel = new cSkelObject();
			tmpSkel->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis);
			if (tmpData[i].sfilePath == "../Resources/Meshes/Objects/DahlEpicCrate.X") {
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
		}
		else {
			char tmpEquip[100];
			sprintf(tmpEquip, "../Resources/Meshes/Objects/equipVendingMachine.X", 100);
			char tmpSave[100];
			sprintf(tmpSave, "../Resources/Meshes/Objects/saveVendingMachine.X", 100);
			char tmpItem[100];
			sprintf(tmpItem, "../Resources/Meshes/Objects/itemVendingMachine.X", 100);
			if (!strcmp(tmpData[i].sfilePath, tmpEquip)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, EquipmentStore);
				vObjects.push_back(tmpVending);
			}
			else if (!strcmp(tmpData[i].sfilePath, tmpSave)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, SaveStore);
				vObjects.push_back(tmpVending);
			}
			else if (!strcmp(tmpData[i].sfilePath, tmpItem)) {
				cVendingObject* tmpVending = new cVendingObject;
				tmpVending->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis, ItemStore);
				vObjects.push_back(tmpVending);
			}
			else {
				cStaticObject* tmpSkel = new cStaticObject();
				tmpSkel->Init(tmpData[i].sfilePath, tmpData[i].scale, tmpData[i].position, tmpData[i].axis);
				vObjects.push_back(tmpSkel);
			}
		}
	}
}

