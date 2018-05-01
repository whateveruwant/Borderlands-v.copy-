#include "stdafx.h"
#include "cStaticObject.h"


cStaticObject::cStaticObject()
{
}


cStaticObject::~cStaticObject()
{
}

void cStaticObject::Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3])
{
	mObjKind = eObjectKind_static;
	cObject::Init(filePath, scale, pos, axis);
	if (filePath == "../Resources/Meshes/Objects/Potal.X") {
		cBaseObject::SetMesh(RESOURCE_SKINNEDXMESH->GetResource(filePath));
		pTransform->SetScale(scale);
		pTransform->SetWorldPosition(pos);
		pTransform->SetAxises(axis);
		cBaseObject::SetActive(true);
		isOpen = false;
		isSee = false;
		ChangeMap = false;
		playAnim = false;
		cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(filePath)->GetAnimSet()[3]->GetName(), 0.0f);
		//0 open
		//1 opened
		//2 close
		//3 closed
		BoundBox.SetBound(&D3DXVECTOR3(35, 0, 0), &D3DXVECTOR3(50, 140, 25));
	}
	else if (filePath == "../Resources/Meshes/Objects/Toilet.X") {
			cBaseObject::SetMesh(RESOURCE_SKINNEDXMESH->GetResource(filePath));
			pTransform->SetScale(scale);
			pTransform->SetWorldPosition(pos);
			pTransform->SetAxises(axis);
			cBaseObject::SetActive(true);
			isOpen = false;
			isSee = false;
			ChangeMap = false;
			playAnim = false;
			cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(filePath)->GetAnimSet()[2]->GetName(), 0.0f);
			//0 closed
			//1 open
			//2 opened
			BoundBox.SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(20, 50, 20));
		}
	else {
		cBaseObject::SetMesh(RESOURCE_STATICXMESH->GetResource(filePath));
		pTransform->SetScale(scale);
		pTransform->SetWorldPosition(pos);
		pTransform->SetAxises(axis);
		cBaseObject::SetActive(true);
		isOpen = false;
		isSee = false;
		ChangeMap = false;
		playAnim = false;
	}
}

void cStaticObject::Update(float timeDelta)
{
	cObject::Update(timeDelta);
	if (isSee && !isOpen) {
		if (KEY_MGR->IsOnceDown('E')) {
			if (filePath == "../Resources/Meshes/Objects/Potal.X") {
				SOUND_MGR->play("잠시만", 0.5f);
				SOUND_MGR->update();
				cBaseObject::GetSkinned()->SetPlaySpeed(2.5);
				cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(filePath)->GetAnimSet()[0]->GetName(), 0.0f);
				SOUND_MGR->play("텔포이동", 1.0f);
				SOUND_MGR->update();
			}
			else if (filePath == "../Resources/Meshes/Objects/Toilet.X") {
				cBaseObject::GetSkinned()->PlayOneShot(RESOURCE_SKINNEDXMESH->GetResource(filePath)->GetAnimSet()[1]->GetName(), 0.0f);
			}
			playAnim = true;
		}
	}
	if (playAnim) {
		if (*cBaseObject::GetSkinned()->Getloop()) {
			ChangeMap = true;
			cBaseObject::GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(filePath)->GetAnimSet()[0]->GetName(), 0.0f);
			*cBaseObject::GetSkinned()->Getloop() = false;
		}
	}
	cObject::Update(timeDelta);
	isSee = false;
}

void cStaticObject::Release()
{
	cObject::Release();
}

void cStaticObject::Render()
{
	cObject::Render();
	//BoundBox.RenderGizmo(pTransform);
}

void cStaticObject::RenderSprite()
{
}

void cStaticObject::RenderFont()
{
	cObject::RenderFont();
	if (filePath == "../Resources/Meshes/Objects/Potal.X") {
		if (playAnim) {
			DXFONT_MGR->PrintStyleText("playerUI", "잠시만기다려주십시오", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
		}
		else if (isSee && !isOpen) {
			DXFONT_MGR->PrintStyleText("playerUI", "월드이동(E)", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
		}
	}
	else if (filePath == "../Resources/Meshes/Objects/Toilet.X") {
		if (playAnim) {
			//DXFONT_MGR->PrintStyleText("playerUI", "Thresher에게 갈 수 있는 문이 열렸습니다.", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
		}
		else if (isSee && !isOpen) {
			DXFONT_MGR->PrintStyleText("playerUI", "변기를 올려본다", WINSIZE_X / 2 + 10, WINSIZE_Y / 2 + 10, 0xffffffff);
		}
	}
	
}
