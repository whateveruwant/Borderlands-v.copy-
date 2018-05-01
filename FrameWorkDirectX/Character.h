#pragma once

#include"cScene.h"
#include"CharacterHand.h"
#include"cBaseObject.h"
#include"Gun.h"
#include"UserSetting.h"
#include"cPartcleEmitter.h"
#include"cInventory.h"
class cObject;
struct sPlayerSaveData {

};
enum CharacterState
{
	CS_IDLE,
	CS_RELOAD,
	CS_FIRE,
	CS_RUN,
	CS_CHANGEGUNIN,
	CS_CHANGEGUNOUT,
	CS_DIE,
	CS_MOVECAMERA,
	CS_CAMERABOSS
};
class cMap;
class cCamera;
class cTerrain;
class cTransform;
class Character
{
private:
	LPDIRECT3DTEXTURE9 lastskullimg;
	LPDIRECT3DTEXTURE9 lastbarimg;
	LPDIRECT3DTEXTURE9 lastbar2img;
	LPDIRECT3DTEXTURE9 exdbarimg;
	LPDIRECT3DTEXTURE9 exdbar2img;
	LPDIRECT3DTEXTURE9 diebg;
	LPDIRECT3DTEXTURE9 diebg2;
	LPDIRECT3DTEXTURE9 shiledimg;
	LPDIRECT3DTEXTURE9 aimUB;
	LPDIRECT3DTEXTURE9 aimRL;
	LPDIRECT3DTEXTURE9 zoomimg;
	LPDIRECT3DTEXTURE9 texhpimg;
	LPDIRECT3DTEXTURE9 texshieldimg;
	LPDIRECT3DTEXTURE9 texhp;
	LPDIRECT3DTEXTURE9 texshield;
	LPDIRECT3DTEXTURE9 texbullet;
	LPDIRECT3DTEXTURE9 texright;
	LPDIRECT3DTEXTURE9 texleft;
	cMap* _cMap;
	cTerrain* _cTerrain;
	cCamera* mCamera;

	cInventory* inven;
	///////////////////////////////////////////////////////

	CharacterState _State;
	CharacterState _SaveState;

	///////////////////////////////////////////////////////

	CharacterHand*	mainHand;
	CharacterHand*	mainHand2;

	Gun* GunrRifle;
	Gun* GunSniper;
	Gun* GunShotgun;

	vector<Gun*>WearGun;


	int gunindex;
	///////////////////////////////////////////////////////

	cTransform*		boneSpine;
	cTransform*		boneCamera;
	cTransform*		boneRHand;

	cTransform*		tranCharacterXZ;
	D3DXVECTOR3		vecCharacter;

	sPlayerItems *pitem;

	int bulletRifleNum;
	int bulletSniperNum;
	int bulletShotgunNum;
	int money;
	float lastTime;
	int characterHP;
	int characterMaxHP;

	int bulletdmg;

	int level;
	int exd;
	int maxExd;

	tag_shiled _shiled;
	///////////////////////////////////////////////////////

	sGameKeySetting* gGameKey;

	int count;
	int zoomCount;

	bool is_Sitdown;
	bool is_jump;
	bool is_zoom;

	int endX, endZ;

	float gravity;
	float jumpSpeed;
	float Characterheight;

	float fireHeight;
	float fireX;
	float firegravity;
	bool is_fire;

	LPDIRECT3DTEXTURE9 hitTex;
	float timeM;



	D3DXVECTOR3 savepos;
	D3DXVECTOR3 cameraStartMove;
	D3DXVECTOR3 cameraEndMove;
	D3DXQUATERNION saveAngle;
	float cameralen;



	DWORD lefthitcolor;
	DWORD righthitcolor;
	DWORD uphitcolor;
	DWORD downhitcolor;
	DWORD diecolor;
	vector<cObject*> pObjects;

	bool cameraMove;
public:
	cTransform*		tranCharacter;
	cBoundBox		tranChaBoundBox;
	cBoundSphere	characterSphere;
	int isHittedDamage;

	Character();
	~Character();

	void SetObjects(vector<cObject*> pObjects) {
		this->pObjects = pObjects;
	}
	void ClearObjects() {
		pObjects.clear();
	}
	void SetGameKey(sGameKeySetting* gGameKey)
	{
		this->gGameKey = gGameKey;
	}
	void SetCamera(cCamera* _cCamera)
	{
		mCamera = _cCamera;
	}
	HRESULT Init();
	void Release();
	void Update(float timeDelta);
	void Render();
	void Scene_RenderSprite();
	void Scene_RenderFont();

	cInventory* GetInven() { return inven; }

	void SetAnimation();
	void KeyUpdate(float timeDelta);
	void SetTerrain(cTerrain* __cTerrain) { _cTerrain = __cTerrain; }
	void SetMap(cMap* __cMap) { _cMap = __cMap; }
	D3DXVECTOR3* GetpPlayerPosition() { return tranCharacter->GetWorldpPosition(); }
	cTransform* GetpPlayerTransform() { return tranCharacter; }
	cTransform* GetpPlayerXZTransform() { return tranCharacterXZ; }
	void PlayCameraMove() { cameraMove = true; }

	cBoundBox* GetpPlayerBoundBox() { return &tranChaBoundBox; }
	int* GetendX() { return &endX; }
	int* GetendZ() { return &endZ; }

	bool playerDie()
	{
		if (characterHP>0)
			return false;
		else
			return true;
	}
	//±¸Ãæµ¹ test
	cBoundSphere* GetpPlayerSphere() { return &characterSphere; }

	int* GetHit() {
		return &isHittedDamage;
	}
	void hitCharacter(int damge);
	int Getbulletdmg() { return bulletdmg; }
	void SetExd(int _exd);
	float Getcri() { return WearGun[0]->Getgun().cri; }

	sPlayerItems GetPlayerInfo() {
		sPlayerItems tmp;
		tmp.hp = &characterHP;
		tmp.maxhp = &characterMaxHP;
		tmp.maxshield = &_shiled.maxshiled;
		tmp.money = &money;
		tmp.rifleBullet = &bulletRifleNum;
		tmp.shield = &_shiled.shiled;
		tmp.shotgunBullet = &bulletShotgunNum;
		tmp.sniperBullet = &bulletSniperNum;
		return tmp;
	}

	bool moveEndCamera()
	{
		if (_State == CS_MOVECAMERA)
		{
			int speed = 3;
			D3DXVECTOR3 len = tranCharacter->GetWorldPosition() - cameraEndMove;
			float leng = D3DXVec3Length(&len);
			if (leng <= 150)
			{
				return true;
			}
		}
		return false;
	}
	bool CameraMove()
	{
		if (_State == CS_MOVECAMERA || _State == CS_CAMERABOSS)return false;
		else return true;
	}

	void SetLeftHit() { lefthitcolor = (DWORD)0xffffffff; }
	void SetRightHit() { righthitcolor = (DWORD)0xffffffff; }
	void SetUpHit() { uphitcolor = (DWORD)0xffffffff; }
	void SetDownHit() { downhitcolor = (DWORD)0xffffffff; }
	void SavePlayer();
	bool LoadPlayer();

	bool GetPlayerDie() {
		if (_State == CS_DIE)
			return true;
		else return false;
	}
};