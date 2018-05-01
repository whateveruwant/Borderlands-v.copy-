#include "stdafx.h"
#include "cMap.h"


cMap::cMap()
{
}


cMap::~cMap()
{
}

void cMap::Init()
{
	///////////////////////////////////////////////////////////////////				<< Terrain
	mDungeon = new cTerrain();
	mDungeon->Init(
		"zoneFile/height.png",
		"../Resources/Terrain/snow.png",
		"../Resources/Testures/terrain2.png",
		"../Resources/Testures/terrain3.png",
		"../Resources/Testures/terrain4.png",
		"zoneFile/splat.png",
		1.0f,
		200.0f,
		1,
		100);

	mTown = new cTerrain();
	mTown->Init(
		"townFile/height.png",
		"../Resources/Testures/terrain1.jpg",
		"../Resources/Testures/terrain2.png",
		"../Resources/Testures/terrain3.png",
		"../Resources/Terrain/snow.png",
		"townFile/splat.png",
		1.0f,
		200.0f,
		1,
		100);

	isTown = true;
	mTerrain = mTown;
	///////////////////////////////////////////////////////////////////				<< AstarTile
	InitForAstarTile();

	///////////////////////////////////////////////////////////////////				<< 날씨효과
	InitForWeather();
	InitForWater();
}

void cMap::InitForAstarTile()
{
	SAFE_DELETE_ARR(mATile);

	int div = 4;
	int gr = 40;
	int lineTileNum = sqrt(mTerrain->GetTotalCellNum()) / div;
	mATile = new sAstarTile[lineTileNum*lineTileNum];
	//mATile = new sAstarTile[100];
	//int lineTileNum = sqrt(100);
	for (int z = 0; z < lineTileNum; ++z) {
		for (int x = 0; x < lineTileNum; ++x) {
			mATile[z*lineTileNum + x].canWalk = false;
			// 02
			// 13
			mATile[z*lineTileNum + x].pos[0] = mTerrain->GetpTerrainVertices()[z*div*(lineTileNum *div + 1) + div*x].pos;
			mATile[z*lineTileNum + x].pos[1] = mTerrain->GetpTerrainVertices()[z*div*(lineTileNum *div + 1) + div*(x + 1)].pos;
			mATile[z*lineTileNum + x].pos[2] = mTerrain->GetpTerrainVertices()[(z + 1)* div*(lineTileNum * div + 1) + div*x].pos;
			mATile[z*lineTileNum + x].pos[3] = mTerrain->GetpTerrainVertices()[(z + 1)* div*(lineTileNum * div + 1) + div*(x + 1)].pos;

			mATile[z*lineTileNum + x].height = (mATile[z*lineTileNum + x].pos[0].y + mATile[z*lineTileNum + x].pos[1].y + mATile[z*lineTileNum + x].pos[2].y + mATile[z*lineTileNum + x].pos[3].y) / 4;

			mATile[z*lineTileNum + x].centerPos.y = (mATile[z*lineTileNum + x].pos[0].y + mATile[z*lineTileNum + x].pos[1].y + mATile[z*lineTileNum + x].pos[2].y + mATile[z*lineTileNum + x].pos[3].y) / 4;
			mATile[z*lineTileNum + x].centerPos.x = (mATile[z*lineTileNum + x].pos[0].x + mATile[z*lineTileNum + x].pos[1].x + mATile[z*lineTileNum + x].pos[2].x + mATile[z*lineTileNum + x].pos[3].x) / 4;
			mATile[z*lineTileNum + x].centerPos.z = (mATile[z*lineTileNum + x].pos[0].z + mATile[z*lineTileNum + x].pos[1].z + mATile[z*lineTileNum + x].pos[2].z + mATile[z*lineTileNum + x].pos[3].z) / 4;

			mATile[z*lineTileNum + x].normal[0] = mTerrain->GetpTerrainVertices()[z*div*(lineTileNum *div + 1) + div*x].normal;
			mATile[z*lineTileNum + x].normal[1] = mTerrain->GetpTerrainVertices()[z*div*(lineTileNum *div + 1) + div*(x + 1)].normal;
			mATile[z*lineTileNum + x].normal[2] = mTerrain->GetpTerrainVertices()[(z + 1)* div*(lineTileNum * div + 1) + div*x].normal;
			mATile[z*lineTileNum + x].normal[3] = mTerrain->GetpTerrainVertices()[(z + 1)* div*(lineTileNum * div + 1) + div*(x + 1)].normal;


			mATile[z*lineTileNum + x].centerNormal.y = (mATile[z*lineTileNum + x].normal[0].y + mATile[z*lineTileNum + x].normal[1].y + mATile[z*lineTileNum + x].normal[2].y + mATile[z*lineTileNum + x].normal[3].y) / 4;
			mATile[z*lineTileNum + x].centerNormal.x = (mATile[z*lineTileNum + x].normal[0].x + mATile[z*lineTileNum + x].normal[1].x + mATile[z*lineTileNum + x].normal[2].x + mATile[z*lineTileNum + x].normal[3].x) / 4;
			mATile[z*lineTileNum + x].centerNormal.z = (mATile[z*lineTileNum + x].normal[0].z + mATile[z*lineTileNum + x].normal[1].z + mATile[z*lineTileNum + x].normal[2].z + mATile[z*lineTileNum + x].normal[3].z) / 4;
			mATile[z*lineTileNum + x].slant = D3DXVECTOR3(0, 0, 0);
			float tmp = 40.0f;

			for (int i = 0; i < 4; ++i) {
				float angle = atan(mATile[z*lineTileNum + x].normal[i].y / mATile[z*lineTileNum + x].normal[i].z);
				angle /= ONE_RAD;
				while (angle < 0) angle += 360.0f;
				if (angle <= 0.0f + tmp || (angle >= 180.0f - tmp && angle < 180.0f)) {
					mATile[z*lineTileNum + x].canWalk = true;
					mATile[z*lineTileNum + x].slant += D3DXVECTOR3(0, 0, (angle / 180)*gr);
					break;
				}
				if ((angle > 180.0f && angle <= 180.0f + tmp) || angle >= 360.0f - tmp) {
					mATile[z*lineTileNum + x].canWalk = true;
					mATile[z*lineTileNum + x].slant += D3DXVECTOR3(0, 0, (-(angle - 180) / 180)*gr);
					break;
				}
			}

			for (int i = 0; i < 4; ++i) {
				/*if (mATile[z*lineTileNum + x].canWalk) {
				break;
				}*/
				float angle = atan(mATile[z*lineTileNum + x].normal[i].y / mATile[z*lineTileNum + x].normal[i].x);
				angle /= ONE_RAD;
				while (angle < 0) angle += 360.0f;
				if (angle <= 0.0f + tmp || (angle >= 180.0f - tmp && angle < 180.0f)) {
					mATile[z*lineTileNum + x].canWalk = true;
					mATile[z*lineTileNum + x].slant += D3DXVECTOR3((angle / 180)*gr, 0, 0);
					break;
				}
				if ((angle > 180.0f && angle <= 180.0f + tmp) || angle >= 360.0f - tmp) {
					mATile[z*lineTileNum + x].canWalk = true;
					mATile[z*lineTileNum + x].slant += D3DXVECTOR3((-(angle - 180) / 180)*gr, 0, 0);
					break;
				}
			}
			mATile[z*lineTileNum + x].savecanWalk = mATile[z*lineTileNum + x].canWalk;

		}
	}
}

void cMap::InitForWeather()
{
	weatherEffect = new cPartcleEmitter();
	weatherEffect->SetActive(true);//활성화 비활성화
	weatherEffect->pTransform->RotateLocal(180.0f * ONE_RAD, 0, 0);//떨어지는 방향

	VEC_COLOR colors;//색상
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	VEC_SCALE scales;//크기
	scales.push_back(1.0f);
	scales.push_back(3.0f);
	scales.push_back(5.0f);

	LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource("../Resources/particle/Paticle_Snow.png");

	//눈
	//파티클 렌더 세팅
	weatherEffect->Init(
		5000,						//최대 파티클 수
		1000.0f,						//초당 파티클 발생 량
		7,							//하나의 파티클 입자 라이프 최소값
		10,							//하나의 파티클 입자 라이프 최대값
		D3DXVECTOR3(0, -1.0f, 0),	//파티클 입자 속도 최소값 ( 로컬기준 )
		D3DXVECTOR3(0, -2.0f, 0),	//파티클 입자 속도 최대값 ( 로컬기준 )
		D3DXVECTOR3(1, 3, 1),		//파티클 입자 가속 최소값 ( 로컬기준 )
		D3DXVECTOR3(-1, 5, -1),		//파티클 입자 가속 최대값 ( 로컬기준 )
		colors,						//컬러 배열
		scales,						//스케일 배열
		0.1f,						//입자크기 최소값
		0.5f,						//최대값
		pTex,						//텍스쳐
		false
	);

	//눈이 떨어지는 범위
	weatherEffect->EmissionType = PATICLE_EMISSION_TYPE::BOX;
	//weatherEffect->EmissionType = PATICLE_EMISSION_TYPE::SPHERE_OUTLINE;
	weatherEffect->SphereEmissionRange = 30.0f;

	EffectRange(weatherEffect, -1000.0f, 1000.0f, 100.0f, 100.0f, -1000.0f, 1000.0f);

	weatherEffect->StartEmission();
}

void cMap::InitForWater()
{
	this->pWater = new cQuad();
	this->pWaterTrans = new cTransform();
	this->pWaterTrans->SetWorldPosition(D3DXVECTOR3(-450, 0, -450));
	pDistortNormal = RESOURCE_TEXTURE->GetResource("../Resources/Testures/Stone-normal.jpg");
}

void cMap::Update(float timeDelta)
{
	weatherEffect->Update(timeDelta);
}

void cMap::Release()
{
	weatherEffect->Release();
	SAFE_DELETE(weatherEffect);
	mTerrain->Release();
	SAFE_DELETE(mTerrain);
	SAFE_DELETE(mATile);
	ReleaseForWater();
}

void cMap::ReleaseForWater()
{
	SAFE_DELETE(pWater);
	SAFE_DELETE(pWaterTrans);
}

void cMap::Render(cCamera* pCam, cLight_Direction* pDirectLight, cCamera* pDirectionLightCamera)
{
	mTerrain->Render(pCam, pDirectLight, pDirectionLightCamera);
	weatherEffect->Render();
	RenderForWater(pCam);
	if (KEY_MGR->IsStayDown(VK_CONTROL))
		RenderForAstarTile();
}

void cMap::RenderForWater(cCamera* pCam)
{
	this->pWaterTrans->SetScale(1000, 4, 1000);
	GetDeviceGrabTexture(Device, pCam->GetRenderTexture(9));
	this->pWater->Render(this->pWaterTrans, pCam->GetRenderTexture(9), pDistortNormal, 0.025f);
}

void cMap::RenderForAstarTile()
{
	int tmp = sqrt(mTerrain->GetTotalCellNum()) / 4;
	for (int z = 0; z < tmp; ++z) {
		for (int x = 0; x <tmp; ++x) {
			if (mATile[z* tmp + x].canWalk) {
				GIZMO_MGR->Line(mATile[z* tmp + x].pos[1], mATile[z* tmp + x].pos[0], 0xffff0000);
				GIZMO_MGR->Line(mATile[z* tmp + x].pos[2], mATile[z* tmp + x].pos[0], 0xff00ff00);
				GIZMO_MGR->Line(mATile[z* tmp + x].pos[3], mATile[z* tmp + x].pos[1], 0xff0000ff);
				GIZMO_MGR->Line(mATile[z* tmp + x].pos[3], mATile[z* tmp + x].pos[2], 0xff000000);
			}
		}
	}
}

void cMap::EffectRange(cPartcleEmitter * className, int MinEmissionRange_x, int MaxEmissionRange_x, int MinEmissionRange_y, int MaxEmissionRange_y, int MinEmissionRange_z, int MaxEmissionRange_z)
{
	className->MinEmissionRangeX = MinEmissionRange_x;	//-100.0f;
	className->MaxEmissionRangeX = MaxEmissionRange_x;	//100.0f;

	className->MinEmissionRangeY = MinEmissionRange_y;	//40.0f;
	className->MaxEmissionRangeY = MaxEmissionRange_y;	//40.0f;

	className->MinEmissionRangeZ = MinEmissionRange_z;	//-100.0f;
	className->MaxEmissionRangeZ = MaxEmissionRange_z;	//100.0f;
}
