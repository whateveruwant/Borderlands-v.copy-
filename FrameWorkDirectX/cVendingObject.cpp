#include "stdafx.h"
#include "cVendingObject.h"


cVendingObject::cVendingObject()
{
}


cVendingObject::~cVendingObject()
{
}

void cVendingObject::Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3], eVendingKind kind)
{
	mObjKind = eObjectKind_vending;
	mVendingKind = kind;
	cObject::Init(filePath, scale, pos, axis);
	cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource(filePath));
	pTransform->SetScale(scale);
	pTransform->SetWorldPosition(pos);
	pTransform->SetAxises(axis);
	cBaseObject::SetActive(true);
	isOpen = false;
	isSee = false;
}

void cVendingObject::Update(float timeDelta)
{
	if (isSee && !isOpen) {
		if (KEY_MGR->IsOnceDown('E')) {
			ActionKind();
		}
	}
	cObject::Update(timeDelta);
	isSee = false;
}

void cVendingObject::Release()
{
	cObject::Release();
}

void cVendingObject::Render()
{
	cObject::Render();
}

void cVendingObject::RenderSprite()
{
}

void cVendingObject::RenderFont()
{
	cObject::RenderFont();
	if (isSee && !isOpen) {
		switch (mVendingKind)
		{
		case EquipmentStore:
			DXFONT_MGR->PrintStyleText("playerUI", "아이템자판기작동(E)", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
			break;
		case SaveStore:
			DXFONT_MGR->PrintStyleText("playerUI", "저장자판기작동(E)", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
			break;
		case ItemStore:
			DXFONT_MGR->PrintStyleText("playerUI", "장비자판기작동(E)", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
			break;
		default:
			break;
		}
	}
}

void cVendingObject::ActionKind()
{
	switch (mVendingKind)
	{
	case EquipmentStore:
		SOUND_MGR->Play3DSound("상점열때", pTransform->GetWorldPosition(), 1.0f);
		SOUND_MGR->update();
		pInventory->OpenbulletShop();
		break;
	case SaveStore:
		SOUND_MGR->Play3DSound("저장상점열때", pTransform->GetWorldPosition(), 1.0f);
		SOUND_MGR->update();
		pCharcter->SavePlayer();
		break;
	case ItemStore:
		SOUND_MGR->Play3DSound("상점열때", pTransform->GetWorldPosition(), 1.0f);
		SOUND_MGR->update();
		pInventory->OpenShop();
		break;
	default:
		break;
	}
}