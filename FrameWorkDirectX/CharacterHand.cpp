#include "stdafx.h"
#include "CharacterHand.h"

CharacterHand::CharacterHand()
{
}


CharacterHand::~CharacterHand()
{
}

void CharacterHand::Init(string filename, float scaling)
{
	this->bActive = true;
	D3DXMATRIXA16 mat;
	D3DXMATRIXA16 scale;

	D3DXMATRIXA16 matX;
	D3DXMatrixRotationX(&matX, -3.6 * ONE_RAD);
	//	D3DXMatrixRotationY(&mat,-90*ONE_RAD);
	D3DXMatrixRotationY(&mat, -82 * ONE_RAD);
	D3DXMatrixScaling(&scale, scaling, scaling, scaling);
	SetMesh(RESOURCE_SKINNEDXMESH->GetResource(filename, &(mat*matX*scale)));

	pSkinned->Play("Idle", 0.3);
}
