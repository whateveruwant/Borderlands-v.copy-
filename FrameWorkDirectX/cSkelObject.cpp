#include "stdafx.h"
#include "cSkelObject.h"
#include "UserSetting.h"


cSkelObject::cSkelObject()
{
}


cSkelObject::~cSkelObject()
{
}

void cSkelObject::Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3])
{
	mObjKind = eObjectKind_skel;
	cObject::Init(filePath, scale, pos, axis);
	cBaseObject::SetMesh(RESOURCE_SKINNEDXMESH->GetResource(filePath));
	pTransform->SetScale(scale);
	pTransform->SetWorldPosition(pos);
	pTransform->SetAxises(axis);
	cBaseObject::SetActive(true);
	isOpen = false;
	isSee = false;
	if (this->filePath == "../Resources/Meshes/Objects/DahlEpicCrate.X") {
		cBaseObject::GetSkinned()->PlayOneShotAfterHold("Open", 0.0f);
		cBaseObject::BoundBox.SetBound(&D3DXVECTOR3(0.0f, 20.0f, 0.0f), &D3DXVECTOR3(100.0f, 70.0f, 130.0f));
	}
	else {
		cBaseObject::GetSkinned()->PlayOneShotAfterHold("Closed_Idle", 0.0f);
		cBaseObject::BoundBox.SetBound(&D3DXVECTOR3(0.0f, 5.0f, 0.0f), &D3DXVECTOR3(110.0f, 70.0f, 60.0f));
	}
}

void cSkelObject::Update(float timeDelta)
{
	if (isSee && !isOpen) {
		if (KEY_MGR->IsOnceDown('E')) {
			SOUND_MGR->Play3DSound("상자열림음", pTransform->GetWorldPosition(), 1.0f);
			Open();
		}
	}
	cObject::Update(timeDelta);
	isSee = false;
}

void cSkelObject::Release()
{
	cObject::Release();
}

void cSkelObject::Render()
{
	cObject::Render();
}

void cSkelObject::RenderSprite()
{
}

void cSkelObject::RenderFont()
{
	cObject::RenderFont();
	if (isSee && !isOpen) {
 		DXFONT_MGR->PrintStyleText("playerUI", "열기(E)", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
	}
}

void cSkelObject::Open()
{
	isOpen = true;
	if (this->filePath == "../Resources/Meshes/Objects/DahlEpicCrate.X") {
		cBaseObject::GetSkinned()->PlayOneShotAfterHold("Closed_Idle", 0.0f);
	}
	else {
		cBaseObject::GetSkinned()->PlayOneShotAfterHold("Open", 0.0f);
	}
}

bool * cSkelObject::GetIsOpen()
{
	return &isOpen;
}
