#include "stdafx.h"
#include "Character.h"
#include"cTerrain.h"
#include"cTransform.h"
#include"cCamera.h"
#include"cMap.h"
Character::Character()
{
}


Character::~Character()
{
}

HRESULT Character::Init()
{
	//RESOURCE_SKINNEDXMESH
	lastskullimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/lastskull.png");
	lastbarimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/lastbar.png");
	lastbar2img = RESOURCE_TEXTURE->GetResource("../Resources/ui/lastbar2.png");
	exdbarimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/exdbar.png");
	exdbar2img = RESOURCE_TEXTURE->GetResource("../Resources/ui/exdbar2.png");
	diebg = RESOURCE_TEXTURE->GetResource("../Resources/ui/diebg.png");
	diebg2 = RESOURCE_TEXTURE->GetResource("../Resources/ui/diebg2.png");
	shiledimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/��.png");
	texhpimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/texhp.png");
	texshieldimg = RESOURCE_TEXTURE->GetResource("../Resources/ui/texshield.png");
	aimUB = RESOURCE_TEXTURE->GetResource("../Resources/particle/aimup.png");
	aimRL = RESOURCE_TEXTURE->GetResource("../Resources/particle/aimright.png");
	zoomimg = RESOURCE_TEXTURE->GetResource("../Resources/particle/zoom.png");
	texhp = RESOURCE_TEXTURE->GetResource("../Resources/ui/playerhp.png");
	texshield = RESOURCE_TEXTURE->GetResource("../Resources/ui/shield.png");
	texbullet = RESOURCE_TEXTURE->GetResource("../Resources/ui/playerbullet.png");
	texright = RESOURCE_TEXTURE->GetResource("../Resources/ui/rightbar.png");
	texleft = RESOURCE_TEXTURE->GetResource("../Resources/ui/leftbar.png");


	mainHand = new CharacterHand();
	mainHand->Init("../Resources/Meshes/Character/mainHand.x", 0.1);

	boneCamera = new cTransform();
	boneRHand = new cTransform();
	boneSpine = new cTransform();
	tranCharacter = new cTransform();
	tranCharacterXZ = new cTransform();

	mainHand->GetSkinned()->AddBoneTransform("Camera", boneCamera);
	mainHand->GetSkinned()->AddBoneTransform("R_Hand", boneRHand);
	mainHand->GetSkinned()->AddBoneTransform("Spine3", boneSpine);


	GunrRifle = new Gun();
	GunrRifle->Init(GUN_AR, boneRHand);

	GunSniper = new Gun();
	GunSniper->Init(GUN_SHOTGUN, boneRHand);

	GunShotgun = new Gun();
	GunShotgun->Init(GUN_SR, boneRHand);

	WearGun.push_back(GunrRifle);
	//WearGun.push_back(GunSniper);
	//WearGun.push_back(GunShotgun);
	gunindex = 0;

	mainHand->GetSkinned()->Render(mainHand->pTransform);
	mCamera->SetWorldPosition(boneCamera->GetWorldPosition());
	mCamera->MovePositionWorld(0, 0, 2);

	*tranCharacter = *boneSpine;
	tranCharacter->SetRotateWorld(0, 0, -0 * ONE_RAD);
	tranCharacter->AddChild(mCamera);
	tranCharacter->AddChild(mainHand->pTransform);
	_State = CS_IDLE;
	_SaveState = _State;

	jumpSpeed = 1.5;
	gravity = 0.7;
	zoomCount = 0;
	Characterheight = 0;
	is_jump = false;
	is_zoom = false;
	is_Sitdown = false;
	tranCharacterXZ->SetWorldPosition(tranCharacter->GetWorldPosition());

	characterHP = 100;
	lastTime = 510;
	level = 1;
	exd = 0;
	maxExd = level * 100;

	characterMaxHP = characterHP;

	characterSphere.SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(30.0f, 30.0f, 30.0f));
	bulletRifleNum = 100;
	bulletSniperNum = 100;
	bulletShotgunNum = 100;

	_shiled.name = SHILED_NONE;
	_shiled.shiled = 0;

	money = 0;
	bulletdmg = 0;

	inven = new cInventory();
	inven->Init();
	inven->SetPlayerGun(&WearGun);
	inven->SetShiled(&_shiled);

	inven->SetNum(&bulletRifleNum, &bulletSniperNum, &bulletShotgunNum, &money, &characterHP, &characterMaxHP);
	//inven->SetGun(GunSniper);
	//inven->SetGun(GunShotgun);
	inven->SetTransPlayer(tranCharacterXZ);
	inven->SetShopGun(GunSniper);
	inven->SetShopGun(GunShotgun);

	//mainHand->pTransform->AddChild(WearGun[0]->GetMuzzle());

	tranChaBoundBox.SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(3.0f, 15.0f, 3.0f));
	characterSphere.SetBound(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(10.0f, 10.0f, 10.0f));


	endX = endZ = 0;
	COLLISIONMGR->pPlayerBullets->mTransform = mCamera;
	tranCharacterXZ->SetWorldPosition(0, 0, 0);
	is_fire = false;
	firegravity = 0.01;
	fireHeight = 0;
	fireX = 0;

	lefthitcolor = 0x00ffffff;
	righthitcolor = 0x00ffffff;
	uphitcolor = 0x00ffffff;
	downhitcolor = 0x00ffffff;
	diecolor = 0x00ffffff;
	//�����߰�
	//hitTex = RESOURCE_TEXTURE->GetResource("../Resources/particle/bl3.png");
	//timeM = 0;
	cameraMove = false;

	return S_OK;
}

void Character::Release()
{
	SAFE_DELETE(boneSpine);
	SAFE_DELETE(boneCamera);
	SAFE_DELETE(boneRHand);
	SAFE_DELETE(tranCharacter);
	SAFE_DELETE(tranCharacterXZ);
	SAFE_DELETE(mainHand);
}

void Character::Update(float timeDelta)
{
	if (KEY_MGR->IsOnceDown('0')) {
		money += 100000;
	}
	if (KEY_MGR->IsOnceDown('9')) {
		characterHP += 100000;
		characterMaxHP = characterHP;
	}
	if (cameraMove)
	{
		if (_State != CS_MOVECAMERA)
		{
			saveAngle = tranCharacter->GetWorldRotateQuaternion();
			cameraStartMove = tranCharacter->GetWorldPosition();
			cameraEndMove = D3DXVECTOR3(-100, tranCharacter->GetWorldPosition().y, -130);
			D3DXVECTOR3 len = cameraStartMove - cameraEndMove;
			cameralen = D3DXVec3Length(&len);
			tranCharacter->LookPosition(cameraEndMove);
			tranCharacter->CorrectionUp(D3DXVECTOR3(0, 1, 0), 90 * ONE_RAD);
			tranCharacter->RotateSelf(-50 * ONE_RAD, 0, 0);
			_State = CS_MOVECAMERA;
		}
		cameraMove = false;
	}
	if (_State == CS_MOVECAMERA)
	{
		int speed = 3;
		D3DXVECTOR3 len = tranCharacter->GetWorldPosition() - cameraEndMove;
		float leng = D3DXVec3Length(&len);
		if (leng >150)
		{
			
			tranCharacter->RotateSelf(ONE_RAD / speed, 0, 0);
			tranCharacter->MovePositionSelf(0, 0, cameralen / (100 * speed));
		}
		else
		{
			static int count = 0;
			if (count == 0)
			{
				SOUND_MGR->Allstop();
				SOUND_MGR->play("����������", 3.0f);
				SOUND_MGR->update();
			}

			count++;
			tranCharacter->MovePositionSelf(RandomFloatRange(-1, 1), RandomFloatRange(-1, 1), RandomFloatRange(-1, 1));
			if (count>200)
				_State = CS_CAMERABOSS;
		}
		return;
	}
	else if (_State == CS_CAMERABOSS)
	{
		static int count = 0;
		count++;
		if (count == 20)
		{
			SOUND_MGR->play("��������2");
			SOUND_MGR->update();
		}
		if (count <=40)
		{
			
			D3DXVECTOR3 poss(-350,70,-350);
			tranCharacter->LookPosition(poss, 1 * ONE_RAD);
			tranCharacter->CorrectionUp(D3DXVECTOR3(0, 1, 0), 5 * ONE_RAD);
			if(count>10)
				mCamera->fov -= ONE_RAD;
		}
		else if (count >=190)
		{
			if (count == 190)
			{
				SOUND_MGR->bgmPlay("����͹��", 1.0f);
				SOUND_MGR->update();
			}
			mCamera->fov = 60*ONE_RAD;
			tranCharacter->SetRotateWorld(saveAngle);
			tranCharacter->SetWorldPosition(cameraStartMove);
			_State = CS_IDLE;
		}

		//tranCharacter->SetWorldPosition(-230, 90, -230);
		//tranCharacter->LookPosition(D3DXVECTOR3(-450, 20, -450));
		//static int count = 0;
		//count++;
		//if (count > 80)
		//{
		//	tranCharacter->SetRotateWorld(saveAngle);
		//	tranCharacter->SetWorldPosition(cameraStartMove);
		//	_State = CS_IDLE;
		//}
		return;
	}



	if (characterHP <= 0)
	{
		is_Sitdown = true;
		if (lastTime == 510)
		{
			SOUND_MGR->Allstop();//�����ǹ߾��� ���Ë� �αٵαټҸ����Ë� bgm����
			SOUND_MGR->bgmPlay("�����ǹ߾�", 1.0f);//�αٵαٳ���
			SOUND_MGR->update();
		}
		if (lastTime <= 0)
		{
			_State = CS_DIE;
		}
		else
		{
			lastTime -= 1;
		}
	}
	else
	{
		if (lastTime != 510)
		{
			SOUND_MGR->Allstop();//�ٽû�Ƴ����� �αٵαټҸ� ����
			SOUND_MGR->bgmPlay("����͹��", 1.0f);//�ٽ� bgm����
			SOUND_MGR->update();
		}

		lastTime = 510;
	}

	if (_State == CS_DIE)return;
	if (_shiled.name != SHILED_NONE)
	{
		if (_shiled.maxshiled > _shiled.shiled)
		{
			_shiled.recovertime -= timeDelta;
			if (_shiled.recovertime < 0)
			{
				_shiled.shiled += _shiled.recover;
				_shiled.recovertime = _shiled.recoverResettime;
				if (_shiled.maxshiled< _shiled.shiled)
				{
					_shiled.shiled = _shiled.maxshiled;
				}
			}
		}
	}


	//mCamera->DefaultControl2(timeDelta);
	_cTerrain = _cMap->GetpTerrain();
	bulletdmg = WearGun[0]->Getgun().m_dmg;
	hitCharacter(isHittedDamage);
	////�߷�
	vecCharacter = tranCharacterXZ->GetWorldPosition();
	if (is_jump)
		vecCharacter.y += jumpSpeed;

	float terrainHeight = _cTerrain->GetHeight(vecCharacter.x, vecCharacter.z);
	if (terrainHeight <vecCharacter.y - 10 - gravity)
	{
		gravity += timeDelta;
		vecCharacter.y -= gravity;
	}
	else
	{
		//�⺻�߷�
		gravity = 0.7;
		//�ɱ�
		if (is_Sitdown)
		{
			if (5 < Characterheight)
				Characterheight -= 0.5;
			else
				Characterheight = 5;
		}
		else if (Characterheight<10)
			Characterheight += 0.5;

		vecCharacter.y = terrainHeight + Characterheight;
		if (is_jump)is_jump = false;
	}
	tranCharacterXZ->SetWorldPosition(vecCharacter.x, vecCharacter.y, vecCharacter.z);


	if (_State != CS_CHANGEGUNIN&&_State != CS_CHANGEGUNOUT&&_State != CS_RELOAD)
	{
		for (int i = 1; i < WearGun.size(); ++i) {
			char c_int = i + 1 + 0x30;
			if (KEY_MGR->IsOnceDown(c_int)) {
				gunindex = i;
				_State = CS_CHANGEGUNIN;
				break;
			}
		}
	}


	KeyUpdate(timeDelta);
	SetAnimation();
	mainHand->Update(timeDelta);
	WearGun[0]->Update(timeDelta);

	tranCharacter->SetWorldPosition(tranCharacterXZ->GetWorldPosition());


	if (_State == CS_CHANGEGUNIN&&*mainHand->GetSkinned()->Getloop())
	{
		_State = CS_CHANGEGUNOUT;
	}
	else if (_State == CS_CHANGEGUNOUT&&*mainHand->GetSkinned()->Getloop())
	{
		Gun* tmp = WearGun[gunindex];
		WearGun.erase(WearGun.begin() + gunindex);
		WearGun.insert(WearGun.begin(), tmp);
		_State = CS_IDLE;
	}

	_cTerrain->GetIndex(tranCharacterXZ->GetWorldPosition().x, tranCharacterXZ->GetWorldPosition().z, &endX, &endZ);
}

void Character::Render()
{
	if (_State == CS_MOVECAMERA||_State == CS_CAMERABOSS)return;
	//if (inven->Render())return;
	if (zoomCount != 20 || WearGun[0]->Getgun()._guntype != GUN_SR)
	{
		mainHand->Render();
	}
	EFFECT_MGR->Render();
	if (zoomCount != 20 || WearGun[0]->Getgun()._guntype != GUN_SR)
	{
		WearGun[0]->Render();
	}
}

void Character::Scene_RenderSprite()
{
	if (_State == CS_MOVECAMERA || _State == CS_CAMERABOSS)return;
	if (inven->Scene_RenderSprite())return;

	RECT shiledrc = { 0,0,255,500 };
	static int count = 0;
	count++;
	if (count % 7 == 0)
	{
		if (lefthitcolor != 0x00ffffff)
			lefthitcolor -= (DWORD)0x11000000;
		if (righthitcolor != 0x00ffffff)
			righthitcolor -= (DWORD)0x11000000;
		if (uphitcolor != 0x00ffffff)
			uphitcolor -= (DWORD)0x11000000;
		if (downhitcolor != 0x00ffffff)
			downhitcolor -= (DWORD)0x11000000;
	}






	//  �Ѿ� �����ض� ��
	float indexX = 248;
	//248
	float gunimgX = (indexX / WearGun[0]->Getgun().m_tAmmo*(WearGun[0]->Getgun().m_tAmmo - WearGun[0]->Getgun().m_nAmmo));
	RECT rchp = { indexX - (indexX / characterMaxHP*characterHP),0,indexX,22 };
	RECT rcsh = { indexX - (indexX / _shiled.maxshiled*_shiled.shiled),0,indexX,22 };

	RECT rcbl = { 0,0,indexX - gunimgX ,22 };

	RECT rcright = { 0,0,indexX,22 };

	RECT rctexhp = { 0,0,32,36 };
	RECT rctexshiled = { 0,0,32,33 };


	//����
	float shiledscale = 0.8;
	if (_shiled.name != SHILED_NONE)
	{
		SPRITE_MGR->DrawTexture(texshieldimg, &rctexshiled, 64, WINSIZE_Y - 110, 0.9, 0.9, 0);
		SPRITE_MGR->DrawTexture(texleft, &rcright, 100, WINSIZE_Y - 110, shiledscale, shiledscale, -5 * ONE_RAD);
		SPRITE_MGR->DrawTexture(texshield, &rcsh, 100, WINSIZE_Y - 110, shiledscale, shiledscale, -5 * ONE_RAD);
	}
	////hp �����̹���
	//SPRITE_MGR->DrawTexture(texhpimg, &rctexhp, 60, WINSIZE_Y - 110);
	////hp���
	//SPRITE_MGR->DrawTexture(texleft, &rcright, 100, WINSIZE_Y - 110, 1.3, 1.3, -5 * ONE_RAD);
	////hp��
	//SPRITE_MGR->DrawTexture(texhp, &rchp, 100, WINSIZE_Y - 110, 1.3, 1.3, -5 * ONE_RAD);
	//hp �����̹���
	float hpscale = 1.0;
	float bulletscale = 0.95;
	SPRITE_MGR->DrawTexture(texhpimg, &rctexhp, 64, WINSIZE_Y - 80, 0.9, 0.9, 0);
	//hp���
	SPRITE_MGR->DrawTexture(texleft, &rcright, 100, WINSIZE_Y - 80, hpscale, hpscale, -5 * ONE_RAD);
	//hp��
	SPRITE_MGR->DrawTexture(texhp, &rchp, 100, WINSIZE_Y - 80, hpscale, hpscale, -5 * ONE_RAD);


	SPRITE_MGR->DrawTexture(texright, &rcright, WINSIZE_X - 340, WINSIZE_Y - 100, bulletscale*1.1, bulletscale*0.9, 5 * ONE_RAD);
	SPRITE_MGR->DrawTexture(texbullet, &rcbl, WINSIZE_X - 340 + gunimgX*bulletscale*1.1, WINSIZE_Y - 100 + gunimgX*0.1, bulletscale*1.1, bulletscale*0.9, 5 * ONE_RAD);


	//����ġ��
	RECT exdrc = { 0,0,510,16 };

	float exdnum = (510 / maxExd);
	float exdnum2 = exdnum*exd;
	RECT exdrc2 = { 0,0, (510 / (float)maxExd)*(float)exd,16 };


	SPRITE_MGR->DrawTexture(exdbar2img, &exdrc, 440, WINSIZE_Y - 120, 0.8, 0.8, 0);
	SPRITE_MGR->DrawTexture(exdbarimg, &exdrc2, 440, WINSIZE_Y - 120, 0.8, 0.8, 0);


	//�Ѿ˼�

	RECT rczoom = { 130,0,1280 + 640,800 + 400 };
	RECT rczoom2 = { 0,0,WINSIZE_X,WINSIZE_Y + 130 };

	//������
	if (zoomCount == 20 && WearGun[0]->Getgun()._guntype == GUN_SR)
	{
		SPRITE_MGR->DrawArea(zoomimg, &rczoom, &rczoom2);
	}

	//����
	if (lefthitcolor != (DWORD)0x00ffffff)
		SPRITE_MGR->DrawTexture(shiledimg, &shiledrc, 0, 100, 0.9, 1.2, 0, lefthitcolor);

	//����
	if (uphitcolor != (DWORD)0x00ffffff)
		SPRITE_MGR->DrawTexture(shiledimg, &shiledrc, 1100, 0, 0.6, 1.7, 90 * ONE_RAD, uphitcolor);

	//������
	if (righthitcolor != (DWORD)0x00ffffff)
		SPRITE_MGR->DrawTexture(shiledimg, &shiledrc, 1280, 700, 0.9, 1.2, 180 * ONE_RAD, righthitcolor);

	//�Ʒ�
	if (downhitcolor != (DWORD)0x00ffffff)
		SPRITE_MGR->DrawTexture(shiledimg, &shiledrc, 200, 800, 0.6, 1.7, -90 * ONE_RAD, downhitcolor);



	if (characterHP <= 0)
	{
		if (diecolor != 0xffffffff)
			diecolor += (DWORD)0x11000000;
	}
	else
	{
		if (diecolor != 0x00ffffff)
			diecolor -= (DWORD)0x11000000;
	}

	if (diecolor != (DWORD)0x00ffffff)
		SPRITE_MGR->DrawArea(diebg2, &rczoom, &rczoom2, diecolor);

	RECT lastbar2rc = { 0,0,510,30 };
	RECT lastbarrc = { 0,0,lastTime,30 };
	RECT lastskullrc = { 0,0,300,300 };

	if (characterHP <= 0)
	{
		SPRITE_MGR->DrawTexture(lastbar2img, &lastbar2rc, 490, 500, 0.7, 1.7, 0);
		SPRITE_MGR->DrawTexture(lastbarimg, &lastbarrc, 490, 500, 0.7, 1.7, 0);
		SPRITE_MGR->DrawTexture(lastskullimg, &lastskullrc, 430, 485, 0.6, 0.6, 0);
	}
	if (is_zoom || _State == CS_RUN)return;

	//����
	RECT rcub = { 0,0,4,20 };
	RECT rcrl = { 0,0,20,4 };

	float area = (100 - WearGun[0]->Getgun().m_argap)*0.3;

	//��
	SPRITE_MGR->DrawTexture(aimUB, &rcub, WINSIZE_X / 2 - 2, WINSIZE_Y / 2 - 22 - area);

	//�Ʒ�
	SPRITE_MGR->DrawTexture(aimUB, &rcub, WINSIZE_X / 2 - 2, WINSIZE_Y / 2 + area);

	//����
	SPRITE_MGR->DrawTexture(aimRL, &rcrl, WINSIZE_X / 2 - 22 - area, WINSIZE_Y / 2 - 2);

	//�Ʒ�
	SPRITE_MGR->DrawTexture(aimRL, &rcrl, WINSIZE_X / 2 + area, WINSIZE_Y / 2 - 2);


	///
	//RECT hitRc = { 0, 0, 500, 500 };
	//if (isHittedDamage != 0 && timeM < 0)	{
	//	 
	//	SPRITE_MGR->DrawTexture(hitTex, &hitRc, RandomFloatRange(10, 100), RandomFloatRange(10, 100), 1.0, 1.0, RandomFloatRange(0, 0));
	//	timeM = 50;
	//} 

}

void Character::Scene_RenderFont()
{
	if (_State == CS_MOVECAMERA || _State == CS_CAMERABOSS)return;
	//if (inven->Scene_RenderFont())return;

	if (characterHP <= 0)
	{
		string text = "������ ����!";
		DXFONT_MGR->PrintStyleTextOutline("playerUI", text, 520, 495, 0XFFFFFFFF);

		text = "���� óġ�� ��Ȱ�Ͻʽÿ�!";
		DXFONT_MGR->PrintStyleTextOutline("playerUI6", text, 690, 545, 0XFFFFFFFF);
	}

	if (WearGun[0]->Getgun().m_nAmmo<WearGun[0]->Getgun().m_tAmmo / 3)
		DXFONT_MGR->PrintText("[R]RELOAD", WINSIZE_X / 2 + 40, WINSIZE_Y / 2 + 40, 0XFFFFFFFF);

	string text = to_string(characterHP);
	DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 110, WINSIZE_Y - 85, 0XFFFFFFFF);
	if (_shiled.name != SHILED_NONE)
	{
		text = to_string(_shiled.shiled);
		DXFONT_MGR->PrintStyleTextOutline("playerUI2", text, 110, WINSIZE_Y - 115, 0xFFFFFFFF);
	}
	text = "LV ";
	if (level < 10)
		text += "0";

	text += to_string(level);
	DXFONT_MGR->PrintStyleTextOutline("playerUI5", text, 490, WINSIZE_Y - 92, 0XFFFFFFFF);
	/*
	text = to_string(exd);
	text += "/";
	text += to_string(maxExd);
	DXFONT_MGR->PrintStyleTextOutline("playerUI5", text, 490, WINSIZE_Y - 50, 0XFFFFFFFF);*/


	string maxbullet = to_string(WearGun[0]->Getgun().m_nAmmo);
	maxbullet += " / ";
	if (WearGun[0]->Getgun()._guntype == GUN_AR)
		maxbullet += to_string(bulletRifleNum);
	else if (WearGun[0]->Getgun()._guntype == GUN_SHOTGUN)
		maxbullet += to_string(bulletShotgunNum);
	else if (WearGun[0]->Getgun()._guntype == GUN_SR)
		maxbullet += to_string(bulletSniperNum);

	DXFONT_MGR->PrintStyleTextOutline("playerUI2", maxbullet, WINSIZE_X - 180, WINSIZE_Y - 88, 0XFFFFFFFF);
}
void Character::SetAnimation()
{
	if (_State != _SaveState)
	{
		mainHand->GetSkinned()->SetPlaySpeed(1);
		_SaveState = _State;
		switch (_State)
		{
		case CS_IDLE:
		{
			mainHand->GetSkinned()->Play("Idle", 0.3f);
			WearGun[0]->SetPlay("idle");
		}break;
		case CS_FIRE:
		{
			if (!is_zoom)
			{
				mainHand->GetSkinned()->SetPlaySpeed(2);
				mainHand->GetSkinned()->Play("Fire", 0.0f);
			}
			WearGun[0]->SetPlay("fire");

		}break;
		case CS_RELOAD:
		{
			if (WearGun[0]->Getgun()._guntype == GUN_AR)
			{
				SOUND_MGR->Play3DSound("����������", mCamera->GetWorldPosition(), 1.0f);
				SOUND_MGR->update();
				mainHand->GetSkinned()->PlayOneShot("Reload_BanditMade_Var2", 0.2);
			}
			else if (WearGun[0]->Getgun()._guntype == GUN_SHOTGUN) {
				SOUND_MGR->Play3DSound("��������", mCamera->GetWorldPosition(), 1.0f);
				SOUND_MGR->update();
				mainHand->GetSkinned()->PlayOneShot("Reload_Jakobs", 0.2);
			}
			else if (WearGun[0]->Getgun()._guntype == GUN_SR) {
				SOUND_MGR->Play3DSound("��������", mCamera->GetWorldPosition(), 1.0f);
				SOUND_MGR->update();
				mainHand->GetSkinned()->PlayOneShot("Reload_Maliwan", 0.2);
			}

			WearGun[0]->SetPlay("reload");
		}break;
		case CS_RUN:
		{
			mainHand->GetSkinned()->Play("Sprint", 0.3f);
		}break;
		case CS_CHANGEGUNIN:
		{
			SOUND_MGR->Play3DSound("�ѹٲٴ¼Ҹ�", mCamera->GetWorldPosition(), 0.5f);
			SOUND_MGR->update();
			mainHand->GetSkinned()->PlayOneShotNext("Holster", "Draw", 0.3);
		}break;
		case CS_CHANGEGUNOUT:
		{
			//mainHand->GetSkinned()->PlayOneShot("Draw", 0.0);
			//mainHand->GetSkinned()->PlayOneShot("Draw");
			//mainHand->GetSkinned()->SetPlaySpeed(1);
		}break;

		default:
			break;
		}
	}
}

void Character::KeyUpdate(float timeDelta)
{

	//����Ʈ ��Ʈ���� ���� ī�޶� Angle ��
	static float nowAngleH = 0.0f;			//����ޱ�
	static float nowAngleV = 0.0f;			//�����ޱ�
	static float maxAngleV = 85.0f;			//���� �ִ� �ޱ�
	static float minAngleV = -85.0f;			//���� ���� �ޱ�
	static float sensitivityH = 0.1f;					//���� �ΰ���
	static float sensitivityV = 0.1f;					//���� �ΰ��� ( �̰��� ������ Invert Mouse )
	static D3DXVECTOR3 nowVelocity(0, 0, 0);			//���� ����� �ӵ��� ���� ����

	static float accelate = 30.0f;						//�ʴ� �̵� ������
	static float nowSpeed = 3.0f;						//���� �ӵ�
	static float maxSpeed = 10.0f;						//�ְ� �ӵ� 

	D3DXVECTOR3 inputVector(0, 0, 0);


	bool is_RunMove = false;

	if (_State == CS_RUN)
	{
		WearGun[0]->Fire();
		if (mCamera->fov > 57 * ONE_RAD)
		{
			mCamera->fov -= ONE_RAD;
		}
	}
	else if (zoomCount == 0)
	{
		if (mCamera->fov < 60 * ONE_RAD)
		{
			mCamera->fov += ONE_RAD;
		}
	}
	if (KEY_MGR->IsStayDown(gGameKey->name.LEFT)) {
		inputVector.x = -1.0f;
		is_RunMove = true;
	}

	else if (KEY_MGR->IsStayDown(gGameKey->name.RIGHT)) {
		inputVector.x = 1.0f;
		is_RunMove = true;
	}
	if (KEY_MGR->IsStayDown(gGameKey->name.UP)) {

		inputVector.z = 1.0f;
		is_RunMove = true;
	}

	else if (KEY_MGR->IsStayDown(gGameKey->name.DOWN)) {
		inputVector.z = -1.0f;
		is_RunMove = true;
	}

	if (_State != CS_RUN)
		maxSpeed = 20;

	if (_State == CS_RELOAD&&WearGun[0]->GetReload())
	{
		if (WearGun[0]->Getgun()._guntype == GUN_AR)
			WearGun[0]->SetReload(&bulletRifleNum);
		else if (WearGun[0]->Getgun()._guntype == GUN_SHOTGUN)
			WearGun[0]->SetReload(&bulletShotgunNum);
		else if (WearGun[0]->Getgun()._guntype == GUN_SR)
			WearGun[0]->SetReload(&bulletSniperNum);

		_State = CS_IDLE;
	}

	if (WearGun[0]->Getgun().m_nAmmo == 0)
	{
		if (WearGun[0]->Getgun()._guntype == GUN_AR&&bulletRifleNum != 0)
			_State = CS_RELOAD;
		else if (WearGun[0]->Getgun()._guntype == GUN_SHOTGUN&&bulletShotgunNum != 0)
			_State = CS_RELOAD;
		else if (WearGun[0]->Getgun()._guntype == GUN_SR&&bulletSniperNum != 0)
			_State = CS_RELOAD;
	}

	if (WearGun[0]->GetReload() && KEY_MGR->IsOnceDown(gGameKey->name.RELOAD))
	{
		_State = CS_RELOAD;
	}

	if (!is_jump&&KEY_MGR->IsOnceDown(gGameKey->name.JUMP))
	{
		SOUND_MGR->play("ĳ��������", 1.0f);
		SOUND_MGR->update();
		is_jump = true;
		if (_State != CS_RELOAD)
		{
			_State = CS_IDLE;
		}
	}

	if (!is_jump&&KEY_MGR->IsStayDown(gGameKey->name.SITDOWN))
	{
		is_Sitdown = true;
		maxSpeed = 10;
		if (_State == CS_RUN)
			_State = CS_IDLE;
	}
	else
	{
		is_Sitdown = false;
	}
	if (!is_RunMove&&_State != CS_RELOAD&&_State != CS_CHANGEGUNIN&&_State != CS_CHANGEGUNOUT)
		_State = CS_IDLE;
	int zoomSpeed = 20;
	if (_State != CS_RELOAD)
	{
		/////////�� �ξƿ�
		if (KEY_MGR->IsStayDown(gGameKey->name.ZOOM))
		{
			_State = CS_IDLE;
			is_zoom = true;
			if (zoomCount == zoomSpeed / 2)
			{
				zoomCount++;
				mainHand->pTransform->MovePositionSelf(-(4.07 / zoomSpeed)*(zoomSpeed / 2), (0.57 / zoomSpeed)*(zoomSpeed / 2), 0);
				mainHand->pTransform->RotateLocal(0.24 * ONE_RAD, 0.1 * ONE_RAD, -5.5*ONE_RAD);
				zoomCount = zoomSpeed;
			}
			else if (zoomCount<zoomSpeed / 2)
			{
				mCamera->fov -= 1 * ONE_RAD;
				mainHand->pTransform->MovePositionSelf(-(4.07 / zoomSpeed), (0.57 / zoomSpeed), 0);
			}
			if (zoomCount<zoomSpeed)
				zoomCount++;

			if (WearGun[0]->Getgun()._guntype == GUN_SR&&zoomCount == zoomSpeed)
				mCamera->fov = 10 * ONE_RAD;

		}
		else
		{
			is_zoom = false;
			if (WearGun[0]->Getgun()._guntype == GUN_SR&&zoomCount == 20)
				mCamera->fov = 50 * ONE_RAD;
			if (zoomCount == zoomSpeed)
			{
				mainHand->pTransform->SetRotateWorld(tranCharacter->GetWorldRotateQuaternion());
				mainHand->pTransform->MovePositionSelf((4.07 / zoomSpeed)*(zoomSpeed / 2), -(0.57 / zoomSpeed)*(zoomSpeed / 2), 0);
				zoomCount = zoomSpeed / 2 + 1;
			}
			else if (zoomCount<zoomSpeed / 2 + 1 && zoomCount>0)
			{
				mCamera->fov += 1 * ONE_RAD;
				mainHand->pTransform->MovePositionSelf((4.07 / zoomSpeed), -(0.57 / zoomSpeed), 0);
			}
			if (zoomCount>0)
				zoomCount--;



		}

		/////�ѽ�
		if ((KEY_MGR->IsStayDown(gGameKey->name.FIRE) && WearGun[0]->Getgun().m_nAmmo != 0))//&& zoomCount==0|| zoomCount==20)
		{
			_State = CS_FIRE;
			if (WearGun[0]->Getfirespeed())
			{
				fireHeight += WearGun[0]->Getgun().m_reaction / 10;

				if (is_zoom)
					fireX = RandomFloatRange(-WearGun[0]->Getgun().m_reactionX / 2, WearGun[0]->Getgun().m_reactionX / 2);
				else
					fireX = RandomFloatRange(-WearGun[0]->Getgun().m_reactionX, WearGun[0]->Getgun().m_reactionX);
				firegravity = 0;
				/*if (WearGun[0]->Getgun().m_reaction>fireHeight)
				fireHeight += WearGun[0]->Getgun().m_reaction / 6;*/
				//�ѱ� ����Ʈ//////////////////////////////////////////////////////////////
				if (WearGun[0]->Getgun()._guntype == GUN_AR)//����
				{
					EFFECT_MGR->StartParticleMuzzleEmitter("rifleBurst", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					EFFECT_MGR->StartParticleMuzzleEmitter("����ȿ��", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					EFFECT_MGR->StartQuadParticleEmitter("smoke", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					SOUND_MGR->Play3DSound("�������ѼҸ�", this->tranCharacter->GetWorldPosition(), 0.4f);
					SOUND_MGR->update();
				}
				else if (WearGun[0]->Getgun()._guntype == GUN_SR)//����
				{
					EFFECT_MGR->StartParticleMuzzleEmitter("sniperBurst", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					SOUND_MGR->Play3DSound("���������ѼҸ�", this->tranCharacter->GetWorldPosition(), 0.4f);
					SOUND_MGR->update();
				}
				else if (WearGun[0]->Getgun()._guntype == GUN_SHOTGUN)//����
				{
					EFFECT_MGR->StartParticleMuzzleEmitter("shotgunBurst", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					EFFECT_MGR->StartQuadParticleEmitter("smoke", 0.05f, WearGun[0]->GetMuzzle(is_zoom));
					SOUND_MGR->Play3DSound("�����ѼҸ�", this->tranCharacter->GetWorldPosition(), 0.4f);
					SOUND_MGR->update();
				}
				///////////////////////////////////////////////////////////////////////////

				WearGun[0]->Fire();
				WearGun[0]->SetFirebullet();
				for (int i = 0; i < WearGun[0]->Getgun().m_bulletnum; i++)
				{

					COLLISIONMGR->pPlayerBullets->randAngle = (100 - WearGun[0]->Getgun().m_argap)*0.12;
					COLLISIONMGR->FirePlayer();
				}
				//Ray ry;
				//D3DXVECTOR3 pos;
				//ry.origin = WearGun[0]->GetMuzzle()->GetWorldPosition();
				//ry.direction = WearGun[0]->GetMuzzle()->GetForward();
				//_cTerrain->IsIntersectRay(&pos, &ry);
				//tranCharacter->RotateWorld(WearGun[0]->Getgun().m_reaction*ONE_RAD, 0, 0);
			}
			//is_fire = false;
		}
		else
		{
			/*if(!is_RunMove)
			_State = CS_IDLE;*/
			if (!is_Sitdown&&is_RunMove&&inputVector.z != -1 && KEY_MGR->IsStayDown(gGameKey->name.RUN) && !is_zoom&&_State != CS_CHANGEGUNIN&&_State != CS_CHANGEGUNOUT)
			{
				maxSpeed = 35;
				_State = CS_RUN;
			}
			if (_State != CS_RUN&&_State != CS_CHANGEGUNIN&&_State != CS_CHANGEGUNOUT)
				_State = CS_IDLE;
			//partclebullet->StopEmission();
			WearGun[0]->NoFire(is_zoom);
		}
	}
	else
	{
		is_zoom = false;
		if (WearGun[0]->Getgun()._guntype == GUN_SR&&zoomCount == 20)
			mCamera->fov = 50 * ONE_RAD;
		if (zoomCount == zoomSpeed)
		{
			mainHand->pTransform->SetRotateWorld(tranCharacter->GetWorldRotateQuaternion());
			mainHand->pTransform->MovePositionSelf((4.07 / zoomSpeed)*(zoomSpeed / 2), -(0.57 / zoomSpeed)*(zoomSpeed / 2), 0);
			zoomCount = zoomSpeed / 2 + 1;
		}
		else if (zoomCount<zoomSpeed / 2 + 1 && zoomCount>0)
		{
			mCamera->fov += 1 * ONE_RAD;
			mainHand->pTransform->MovePositionSelf((4.07 / zoomSpeed), -(0.57 / zoomSpeed), 0);
		}
		if (zoomCount>0)
			zoomCount--;


		if (is_RunMove&&inputVector.z != -1 && KEY_MGR->IsStayDown(gGameKey->name.RUN))
		{
			maxSpeed = 35;
		}
	}
	static int countRun = 0;
	if (is_RunMove)
	{
		if (countRun % 20 == 0 && maxSpeed == 35)
		{
			SOUND_MGR->Play3DSound("ĳ���Ͷٱ�", tranCharacter->GetWorldPosition(), 1.0f);
			SOUND_MGR->update();
		}
		else if (countRun % 40 == 0)
		{
			SOUND_MGR->Play3DSound("ĳ���Ͷٱ�", tranCharacter->GetWorldPosition(), 1.0f);
			SOUND_MGR->update();
		}


		countRun++;
	}
	else
	{

		countRun = 0;
	}

	//if (!is_RunMove)
	//{
	//	if (_State != CS_FIRE&&_State != CS_RELOAD&&_State != CS_CHANGEGUNIN&&_State != CS_CHANGEGUNOUT)
	//		_State = CS_IDLE;
	//}

	if (VECTORZERO(inputVector) == false)
	{
		//����ȭ
		D3DXVec3Normalize(&inputVector, &inputVector);
	}

	D3DXVECTOR3 target = inputVector * maxSpeed;
	target *= timeDelta;

	tranCharacterXZ->MovePositionSelf(target);
	for (int i = 0; i < pObjects.size(); ++i) {
		D3DXVECTOR3 distance = pObjects[i]->pTransform->GetWorldPosition() - tranCharacterXZ->GetWorldPosition();
		if (D3DXVec3Length(&distance) >= 100) continue;
		PHYSICS_MGR->IsBlocking(tranCharacterXZ, &tranChaBoundBox, pObjects[i]->pTransform, &pObjects[i]->BoundBox, 1.0f);
	}
	int eX, eZ;
	float heY = 3 - Characterheight - _cTerrain->GetHeight(tranCharacterXZ->GetWorldPosition().x, tranCharacterXZ->GetWorldPosition().z);
	_cTerrain->GetIndex(tranCharacterXZ->GetLocalPosition().x, tranCharacterXZ->GetLocalPosition().z, &eX, &eZ);
	if (!is_jump)
		tranCharacterXZ->MovePositionLocal(_cMap->GetpAstarTile()[eZ*(((int)sqrt(_cTerrain->GetTotalCellNum())) / 4) + eX].slant*timeDelta);

	/*if (!is_jump&&tranCharacter->GetWorldPosition().y>heY&&_cMap->GetpAstarTile()[eZ*(1024 / 4) + eZ].canWalk)
	{
	tranCharacterXZ->MovePositionSelf(-target*1.1);
	}*/
	int screenCenterX = WINSIZE_X / 2;
	int screenCenterY = WINSIZE_Y / 2;

	//���� ���콺 ��ġ
	POINT mousePos = GetMousePos();

	//�̵��� ( �߾ӿ��� �־��� �� )
	float deltaX = mousePos.x - screenCenterX;
	float deltaY = mousePos.y - screenCenterY;

	//�ޱ� �߰�


	if (fireHeight > 0)
	{
		if (is_zoom)
		{
			if (!is_fire)
			{
				firegravity += 0.01;
				fireHeight -= firegravity;
			}

			if (is_fire&&WearGun[0]->Getgun().m_reaction / 2 > fireHeight)
			{
				fireHeight += WearGun[0]->Getgun().m_reaction / 10;
			}
			else
			{
				is_fire = false;
			}
		}
		else
		{
			if (!is_fire)
			{
				firegravity += 0.02;
				fireHeight -= firegravity;
			}

			if (is_fire&&WearGun[0]->Getgun().m_reaction > fireHeight)
			{
				fireHeight += WearGun[0]->Getgun().m_reaction / 10;
			}
			else
			{
				is_fire = false;
			}
		}

	}
	else
	{
		is_fire = true;
		firegravity = 0.0;
	}
	nowAngleH += fireX;
	nowAngleV = nowAngleV - fireHeight;
	;
	if (is_zoom)
	{
		if (abs(deltaX * sensitivityH) > sensitivityH)
		{
			nowAngleH += (deltaX * sensitivityH / 3)*KEY_MGR->_mouseSpeed;
		}
		if (abs(deltaY * sensitivityV) > sensitivityV)
		{
			nowAngleV += (deltaY * sensitivityV / 3)*KEY_MGR->_mouseSpeed;
		}
	}
	else
	{
		if (abs(deltaX * sensitivityH) > sensitivityH)
		{
			nowAngleH += (deltaX * sensitivityH)*KEY_MGR->_mouseSpeed;
		}
		if (abs(deltaY * sensitivityV) > sensitivityV)
		{
			nowAngleV += (deltaY * sensitivityV)*KEY_MGR->_mouseSpeed;
		}
	}


	//�ޱ۰��� min max ���� ������
	nowAngleV = Clamp(nowAngleV, minAngleV, maxAngleV);

	//�ٽ� ���콺 ��ġ�� ���ͷ�...

	SetMousePos(screenCenterX, screenCenterY);

	tranCharacter->SetRotateWorld(nowAngleV * ONE_RAD, nowAngleH * ONE_RAD, 0.0f);
	tranCharacterXZ->SetRotateWorld(0.0f, nowAngleH * ONE_RAD, 0.0f);

	fireX = 0;
	nowAngleV = nowAngleV + fireHeight;
}

void Character::hitCharacter(int damge)
{

	if (damge == 0)return;

	_shiled.recovertime = _shiled.recoverResetdmgtime;
	int dam = damge;
	if (_shiled.name != SHILED_NONE&&_shiled.shiled > 0)
	{
		SOUND_MGR->Play3DSound("����Ÿ����", this->tranCharacter->GetWorldPosition(), 1.0f);
		SOUND_MGR->update();
		if (_shiled.shiled < dam)
		{
			dam -= _shiled.shiled;
			_shiled.shiled = 0;
			characterHP -= dam;
			if (characterHP <= 0)
			{
				characterHP = 0;
			}
		}
		else
		{
			_shiled.shiled -= dam;
		}
	}
	else
	{
		characterHP -= dam;
		if (characterHP <= 0)
			characterHP = 0;
	}
	isHittedDamage = 0;

}

void Character::SetExd(int _exd)
{
	if (characterHP <= 0)
	{
		characterHP = characterMaxHP / 2;
	}
	exd += _exd;
	if (exd >= maxExd)
	{
		SOUND_MGR->play("������", 1.0f);
		SOUND_MGR->update();
		level++;
		exd -= maxExd;
		maxExd = 100 * level;
		characterMaxHP += 20;;
		characterHP = characterMaxHP;
	}
}

bool Character::LoadPlayer()
{
	bool isTown;
	WearGun.clear();
	inven->InvenGunClear();
	inven->InvenShiledClead();
	GUNTYPE WearGuns[3];
	GUNTYPE InvenGuns[5];
	SHILEDNAME saveshiled[4];
	HANDLE file;
	DWORD read;
	file = CreateFile("player.save",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);


	ReadFile(file, &bulletRifleNum, sizeof(int), &read, NULL);
	ReadFile(file, &bulletSniperNum, sizeof(int), &read, NULL);
	ReadFile(file, &bulletShotgunNum, sizeof(int), &read, NULL);
	ReadFile(file, &money, sizeof(int), &read, NULL);
	ReadFile(file, &characterHP, sizeof(int), &read, NULL);
	ReadFile(file, &characterMaxHP, sizeof(int), &read, NULL);
	ReadFile(file, &WearGuns, sizeof(GUNTYPE) * 3, &read, NULL);
	ReadFile(file, &InvenGuns, sizeof(GUNTYPE) * 5, &read, NULL);
	ReadFile(file, &_shiled, sizeof(tag_shiled), &read, NULL);
	ReadFile(file, &saveshiled, sizeof(SHILEDNAME) * 4, &read, NULL);
	ReadFile(file, &savepos, sizeof(D3DXVECTOR3), &read, NULL);
	ReadFile(file, &isTown, sizeof(bool), &read, NULL);
	CloseHandle(file);


	for (int i = 0; i < 3; i++)
	{
		if (WearGuns[i] == GUN_AR)
			WearGun.push_back(GunrRifle);

		else if (WearGuns[i] == GUN_SHOTGUN)
			WearGun.push_back(GunShotgun);

		else if (WearGuns[i] == GUN_SR)
			WearGun.push_back(GunSniper);
	}
	for (int i = 0; i < 5; i++)
	{
		if (InvenGuns[i] == GUN_AR)
			inven->SetGun(GunrRifle);

		else if (InvenGuns[i] == GUN_SHOTGUN)
			inven->SetGun(GunShotgun);

		else if (InvenGuns[i] == GUN_SR)
			inven->SetGun(GunSniper);
	}
	for (int i = 0; i < 4; i++)
	{
		if (saveshiled[i] != SHILED_NONE)
		{
			for (int j = 0; j < inven->GetShopShiled().size(); j++)
			{
				if (inven->GetShopShiled()[j]->name == saveshiled[i])
				{
					inven->SetInvenShiled(inven->GetShopShiled()[j]);
					break;
				}
			}
		}
	}

	_State = CS_IDLE;

	tranCharacterXZ->SetWorldPosition(savepos);
	return isTown;
}

void Character::SavePlayer()
{
	bool isTown = _cMap->GetIsTown();
	SHILEDNAME saveshiled[4];
	savepos = tranCharacterXZ->GetWorldPosition();
	for (int i = 0; i < 4; i++)
	{
		if (inven->GetInvenShiled().size() > i)
		{
			saveshiled[i] = inven->GetInvenShiled()[i]->name;
		}
		else
		{
			saveshiled[i] = SHILED_NONE;
		}
	}
	GUNTYPE WearGuns[3];
	for (int i = 0; i < 3; i++)
	{
		if (WearGun.size() > i)
			WearGuns[i] = WearGun[i]->Getgun()._guntype;
		else
			WearGuns[i] = GUN_NONE;
	}
	GUNTYPE InvenGuns[5];
	for (int i = 0; i < 5; i++)
	{
		if (inven->GetInvenGun().size() > i)
			InvenGuns[i] = inven->GetInvenGun()[i]->Getgun()._guntype;
		else
			InvenGuns[i] = GUN_NONE;
	}

	HANDLE file;
	DWORD write;
	file = CreateFile("player.save",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	WriteFile(file, &bulletRifleNum, sizeof(int), &write, NULL);
	WriteFile(file, &bulletSniperNum, sizeof(int), &write, NULL);
	WriteFile(file, &bulletShotgunNum, sizeof(int), &write, NULL);
	WriteFile(file, &money, sizeof(int), &write, NULL);
	WriteFile(file, &characterHP, sizeof(int), &write, NULL);
	WriteFile(file, &characterMaxHP, sizeof(int), &write, NULL);
	WriteFile(file, &WearGuns, sizeof(GUNTYPE) * 3, &write, NULL);
	WriteFile(file, &InvenGuns, sizeof(GUNTYPE) * 5, &write, NULL);
	WriteFile(file, &_shiled, sizeof(tag_shiled), &write, NULL);
	WriteFile(file, &saveshiled, sizeof(SHILEDNAME) * 4, &write, NULL);
	WriteFile(file, &savepos, sizeof(D3DXVECTOR3), &write, NULL);
	WriteFile(file, &isTown, sizeof(bool), &write, NULL);

	CloseHandle(file);
}
