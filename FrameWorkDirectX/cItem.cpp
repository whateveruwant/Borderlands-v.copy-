#include "stdafx.h"
#include "cItem.h"


cItem::cItem()
{
}


cItem::~cItem()
{
}

void cItem::Init(sPlayerItems pPlayer, eItemKind kind, D3DXVECTOR3 _pos)
{
	this->pPlayer = pPlayer;
	this->mKind = kind;
	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.1f, 0.1f, 0.1f);
	switch (this->mKind)
	{
	case money:
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource("../Resources/Meshes/Item/money.X", matCorrection));
		break;
	case shotgunBullet:
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource("../Resources/Meshes/Item/ShotgunBullet.X", matCorrection));
		break;
	case rifleBullet:
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource("../Resources/Meshes/Item/RifleBullet.X", matCorrection));
		break;
	case sniperBullet:
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource("../Resources/Meshes/Item/SniperBullet.X", matCorrection));
		break;
	case healPak:
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource("../Resources/Meshes/Item/Heal.X", matCorrection));
		break;
	}
	pTransform->SetWorldPosition(_pos);
	cBaseObject::SetActive(true);
	showInfo = false;
	inBox = new bool;
	*inBox = true;
}

void cItem::Release()
{
}

void cItem::Update(float timeDelta)
{
	if (showInfo && *inBox) {
		if (KEY_MGR->IsOnceDown('E')) {
			SOUND_MGR->Play3DSound("아이템먹을때", pTransform->GetWorldPosition(), 1.0f);
			SOUND_MGR->update();
			if (GetPlayer()) {
				SetActive(false);
			}
		}
	}
	cBaseObject::Update(timeDelta);
	showInfo = false;
}

void cItem::Render()
{
	cBaseObject::Render();
}

void cItem::RenderSprite()
{
	if (showInfo && *inBox) {

	}
}

void cItem::RenderFont()
{
	if (showInfo && *inBox && IsActive()) {
		switch (this->mKind)
		{
		case money:
			DXFONT_MGR->PrintStyleText("playerUI","money", WINSIZE_X / 2, WINSIZE_Y / 2, 0xffffffff);
			break;
		case shotgunBullet:
			DXFONT_MGR->PrintStyleText("playerUI", "shotgunBullet", WINSIZE_X / 2, WINSIZE_Y / 2, 0xffffffff);
			break;
		case rifleBullet:
			DXFONT_MGR->PrintStyleText("playerUI", "rifleBullet", WINSIZE_X / 2, WINSIZE_Y / 2, 0xffffffff);
			break;
		case sniperBullet:
			DXFONT_MGR->PrintStyleText("playerUI", "sniperBullet", WINSIZE_X / 2, WINSIZE_Y / 2, 0xffffffff);
			break;
		case healPak:
			DXFONT_MGR->PrintStyleText("playerUI", "healPak", WINSIZE_X / 2, WINSIZE_Y / 2, 0xffffffff);
			break;
		}
	}
}

bool cItem::GetPlayer()
{
	switch (mKind)
	{
	case money:
		*pPlayer.money += 10;
		break;
	case shotgunBullet:
		*pPlayer.shotgunBullet += 10;
		break;
	case rifleBullet:
		*pPlayer.rifleBullet += 10;
		break;
	case sniperBullet:
		*pPlayer.sniperBullet += 10;
		break;
	case healPak:
		if (*pPlayer.hp < *pPlayer.maxhp) {
			*pPlayer.hp += 10;
			if (*pPlayer.hp >= *pPlayer.maxhp) {
				*pPlayer.hp = *pPlayer.maxhp;
			}
		}
		else {
			return false;
		}
		break;
	}
	return true;
}
