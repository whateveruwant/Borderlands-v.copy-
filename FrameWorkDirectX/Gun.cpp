#include "stdafx.h"
#include "Gun.h"
#include "cTransform.h"


//
Gun::Gun()
{

}


Gun::~Gun()
{
}

//
void Gun::Init(GUNTYPE _gunType, cTransform* _hand)
{
	_gun._guntype = _gunType;

	//
	switch (_gunType)
	{
	case GUN_AR:
		fileName = "../Resources/Meshes/Character/¶óÀÌÇÃ.x";
		_gun.m_range = 10;      // »ç°Å¸®
		_gun.m_dmg = 7;         // °ø°Ý·Â
		_gun.m_fireSpeed = 0.1;   // ¿¬»ç
		_gun.m_reaction = 5;      // ¹Ýµ¿
		_gun.m_ar = 50;         // ¸íÁß·ü
		_gun.m_argap = _gun.m_ar;         // ¸íÁß·ü ¹ú¾îÁú²¨
		_gun.m_arpower = 5;         // ¸íÁß·ü ¹ú¾îÁö´Â Á¤µµ
		_gun.m_tAmmo = 30;      // ÃÑÀÌ °¡Áú ¼ö ÀÖ´Â ÃÑ ÃÑ¾Ë
		_gun.m_reactionX = ONE_RAD * 3;
		_gun.m_bulletnum = 1;
		_gun.m_price = 200;
		_gun.name = "Rifle";
		_gun.cri = 1.5;
		break;
	case GUN_SHOTGUN:
		fileName = "../Resources/Meshes/Character/Shotgun.x";
		_gun.m_range = 10;      // »ç°Å¸®
		_gun.m_dmg = 7;         // °ø°Ý·Â
		_gun.m_fireSpeed = 1;   // ¿¬»ç
		_gun.m_reaction = 8;      // ¹Ýµ¿
		_gun.m_ar = 20;         // ¸íÁß·ü
		_gun.m_argap = _gun.m_ar;         // ¸íÁß·ü ¹ú¾îÁú²¨
		_gun.m_arpower = 5;         // ¸íÁß·ü ¹ú¾îÁö´Â Á¤µµ
		_gun.m_tAmmo = 10;      // ÃÑÀÌ °¡Áú ¼ö ÀÖ´Â ÃÑ ÃÑ¾Ë
		_gun.m_bulletnum = 6;
		_gun.m_reactionX = 0;
		_gun.m_price = 200;
		_gun.name = "Shotgun";
		_gun.cri = 1.5;
		break;
	case GUN_SR:
		fileName = "../Resources/Meshes/Character/GestaltDef_SniperRifle_GestaltSkeletalMesh.X";
		_gun.m_range = 10;      // »ç°Å¸®
		_gun.m_dmg = 20;         // °ø°Ý·Â
		_gun.m_fireSpeed = 1;   // ¿¬»ç
		_gun.m_reaction = 8;      // ¹Ýµ¿
		_gun.m_ar = 80;         // ¸íÁß·ü
		_gun.m_argap = _gun.m_ar;         // ¸íÁß·ü ¹ú¾îÁú²¨
		_gun.m_arpower = 5;         // ¸íÁß·ü ¹ú¾îÁö´Â Á¤µµ
		_gun.m_tAmmo = 6;      // ÃÑÀÌ °¡Áú ¼ö ÀÖ´Â ÃÑ ÃÑ¾Ë
		_gun.m_bulletnum = 1;
		_gun.m_reactionX = 0;
		_gun.m_price = 200;
		_gun.name = "Sniper";
		_gun.cri = 2.5;
		break;
	}
	_gun.shotTime = 0.0f;
	_gun.m_nAmmo = _gun.m_tAmmo;// ÇöÀç ÃÑ¾Ë
	Max_ar = -50;

	//
	D3DXMATRIXA16 matCorrection_rotY;
	D3DXMatrixRotationY(&matCorrection_rotY, 90 * ONE_RAD);
	//D3DXMATRIXA16 matCorrection_rotX;
	//D3DXMatrixRotationX(&matCorrection_rotX, -10 * ONE_RAD);
	//D3DXMATRIXA16 matCorrection_Rot = matCorrection_rotX * matCorrection_rotY;
	D3DXMATRIXA16 matCorrection_Rot = matCorrection_rotY;
	D3DXMATRIXA16 matCorrection_Scale;
	D3DXMatrixScaling(&matCorrection_Scale, 0.1f, 0.1f, 0.1f);
	D3DXMATRIXA16 matCorrection = matCorrection_Rot * matCorrection_Scale;
	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(fileName, &matCorrection);

	//
	transmain = new cTransform();
	m_handle = new cTransform;
	m_muzzle = new cTransform;
	m_muzzle2 = new cTransform();

	m_body = new cBaseObject();
	m_body->SetMesh(pSkinned);
	m_body->SetActive(true);
	if (_gun._guntype == GUN_SR)
		m_body->GetSkinned()->Play("reload");
	else
		m_body->GetSkinned()->Play("idle");

	m_body->GetSkinned()->AddApplyTransform("WeaponOffset", m_handle);
	m_body->GetSkinned()->Render(m_body->pTransform);
	*transmain = *m_handle;
	transmain->AddChild(m_body->pTransform);
	m_muzzle->SetWorldPosition(-0.2, 1.9, 1.7);
	m_muzzle->SetRotateWorld(0, 90 * ONE_RAD, 0);

	m_muzzle2->SetWorldPosition(-0.2, 1.9, 1.7);
	m_muzzle2->SetRotateWorld(0, 90 * ONE_RAD, 0);
	//   transmain->AddChild(m_muzzle);
	m_body->pTransform->AddChild(m_muzzle);
	m_body->pTransform->AddChild(m_muzzle2);
	_hand->AddChild(transmain);
	transmain->MovePositionWorld(1.4, 0.3, 0);
	m_muzzle->MovePositionWorld(1.4, 0, 0);
	m_muzzle2->MovePositionWorld(1.4, -0.4, -1.7);

	is_reload = m_body->GetSkinned()->Getloop();

	//GestaltDef_AssaultRifle_GestaltSkeletalMesh
	//WeaponOffset
	//Root
}

void Gun::Update(float timeDelta)
{
	//m_muzzle->DefaultControl2(timeDelta);
	m_body->Update(timeDelta);
	_gun.shotTime -= timeDelta;

}

void Gun::Render()
{
	m_muzzle2->RenderGimozo();
	//m_muzzle->RenderGimozo();
	this->m_body->Render();
	//RESOURCE_SKINNEDXMESH->GetResource(fileName)->ShowAnimationName(0, 100);
	//GIZMO_MGR->WireSphere(m_muzzle->GetWorldPosition(), 3);
}

//bool Gun::Getfirespeed(float timeDelta, int nVKey)
bool Gun::Getfirespeed()
{

	if (_gun.shotTime <= 0.0f)
	{
		return true;
	}
	return false;
	//_gun.shotTime -= timeDelta;
	//if (_gun.shotTime <= 0.0f)
	//{
	//   if (GetAsyncKeyState(nVKey) & 0x8000)
	//   {
	//      _gun.shotTime = _gun.m_fireSpeed;
	//      if (_gun.m_argap > Max_ar)
	//      {
	//         //¿¡ÀÓ ¹ú¾îÁü
	//         _gun.m_argap -= _gun.m_arpower*1.5;
	//      }
	//      return true;
	//   }
	//   else
	//   {
	//      //¿¡ÀÓ ÁÙ¾îµë
	//      _gun.m_argap += _gun.m_arpower*0.7;
	//   }
	//   if (_gun.m_ar < _gun.m_argap)
	//   {
	//      //°íÁ¤ ¸íÁß·üº¸´Ù ³·¾ÆÁö¸é º¸Á¤
	//      _gun.m_argap = _gun.m_ar;
	//   }
	//}
	//return false;
}

void Gun::SetPlay(string an)
{
	if (_gun._guntype == GUN_SR)
	{
		if (an == "reload")
			this->m_body->GetSkinned()->PlayOneShot("ÀåÀü2");
		else
			this->m_body->GetSkinned()->PlayOneShot("reload");

	}
	else if (an == "reload")
	{
		this->m_body->GetSkinned()->PlayOneShot(an);
	}
	else
	{
		this->m_body->GetSkinned()->Play(an);
	}
}

void Gun::SetReload(int * bulletnum)
{

	if (*bulletnum<_gun.m_tAmmo)
	{

		if (_gun.m_nAmmo > 0)
		{
			_gun.m_nAmmo += *bulletnum;
			*bulletnum = 0;
			if (_gun.m_nAmmo > _gun.m_tAmmo)
			{
				int num = _gun.m_nAmmo - _gun.m_tAmmo;
				*bulletnum = num;
			}
		}
		else
		{
			_gun.m_nAmmo = *bulletnum;
			*bulletnum = 0;
		}

	}
	else if (*bulletnum == 0)
	{
		*bulletnum -= _gun.m_tAmmo;
		_gun.m_nAmmo = _gun.m_tAmmo;
	}
	else
	{
		int num = _gun.m_tAmmo - _gun.m_nAmmo;
		*bulletnum -= num;
		_gun.m_nAmmo = _gun.m_tAmmo;
	}
}