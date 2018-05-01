#include "stdafx.h"
#include "cScene_Option.h"
#include "cTerrain.h"
#include "cTransform.h"
#include "cLight_Direction.h"


cScene_Option::cScene_Option()
{
}


cScene_Option::~cScene_Option()
{
}

HRESULT cScene_Option::Scene_Init()
{
	m_pTerrain = new cTerrain();
	m_pTerrain->Init(
		"../Resources/Testures/Sheep_Height.bmp",
		"../Resources/Testures/terrain1.jpg",
		"../Resources/Testures/terrain2.png",
		"../Resources/Testures/terrain3.png",
		"../Resources/Testures/terrain4.png",
		"../Resources/Testures/Sheep_Splat.png",
		1.0f,
		200.0f,
		3,
		100);
	this->SetEnvironment("../Resources/TextureCUBE/grassenvmap1024.dds");
	return S_OK;
}

void cScene_Option::Scene_Release()
{
}

void cScene_Option::Scene_Update(float timDelta)
{
	this->pSceneBaseDirectionLight->pTransform->DefaultControl2(timDelta);
}

void cScene_Option::Scene_Render1()
{
	m_pTerrain->Render(pMainCamera, pSceneBaseDirectionLight);
}
