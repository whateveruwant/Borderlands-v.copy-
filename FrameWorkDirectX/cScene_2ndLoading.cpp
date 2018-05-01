#include "StdAfx.h"
#include "cScene_2ndLoading.h"
#include "cLight_Direction.h"
#include "cXMesh_Static.h"
#include "cBaseObject.h"
#include "cTransform.h"
#include "cCamera.h"
#include "cTerrain.h"
#include "cSkinnedAnimation.h"
#include "cLight_Point.h"

cScene_2ndLoading::cScene_2ndLoading(void)
{
}

cScene_2ndLoading::~cScene_2ndLoading(void)
{
}

HRESULT cScene_2ndLoading::Scene_Init()
{
	D3DXCreateTextureFromFile(Device, "../Resources/ui/loading.png", &tx);
	return S_OK;
}

void cScene_2ndLoading::Scene_Release()
{

}

void cScene_2ndLoading::Scene_Update(float timDelta)
{
}

void cScene_2ndLoading::Scene_Render1()
{
	cXMesh_Static::SetCamera(this->pMainCamera);
	cXMesh_Static::SetTechniqueName("Base");      //쉐도우랑 같이 그릴려면 ReciveShadow 로 Technique 셋팅
	cXMesh_Static::SetBaseLight(this->pSceneBaseDirectionLight);;
	cXMesh_Skinned::SetCamera(this->pMainCamera);
	cXMesh_Skinned::SetTechniqueName("ReciveShadow");
	cXMesh_Skinned::sSkinnedMeshEffect->SetTexture("Ramp_Tex", RESOURCE_TEXTURE->GetResource("../Resources/Testures/Ramp_1.png"));
	cXMesh_Skinned::SetBaseLight(this->pSceneBaseDirectionLight);
}


void cScene_2ndLoading::Scene_RenderSprite()
{
	RECT rc = { 0, 0, 2050, 900 };
	RECT rcClient = { -8, -31, 1288, 808 };
	SPRITE_MGR->DrawArea(tx, &rc, &rcClient);
}

void cScene_2ndLoading::Scene_RenderFont()
{
	DXFONT_MGR->PrintStyleText("playerUI", SCENE_MGR->fString, 0, 0, 0xffffffff);
	char szTemp[128];
	sprintf(szTemp, "%.2f 퍼센트 로딩중", SCENE_MGR->fProgress);
	DXFONT_MGR->PrintStyleText("playerUI", szTemp, 0, 30, 0xffffffff);
}