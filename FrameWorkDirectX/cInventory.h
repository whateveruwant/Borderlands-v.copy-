#pragma once
#include "Gun.h"

enum SHILEDNAME
{
	SHILED_NONE,
	SHILED_ONE,
	SHILED_TWO
};
struct tag_shiled
{
	SHILEDNAME	name;
	int			shiled;				//쉴드양
	int			maxshiled;				//쉴드양
	int			recover;			//회복양
	float		recovertime;		//회복시간
	float		recoverResettime;	//
	float		recoverResetdmgtime;	//
	int			price;
};


enum MENUSTATE
{
	MS_MAP,
	MS_INVEN
};
enum INVENTORYSTATE
{
	IV_LEFT,
	IV_RIGHT
};
enum SHOPSTATE
{
	SS_INVEN,
	SS_SHOP
};
enum MENUCHOICESTATE
{
	MCS_NONE,
	MCS_SHOP,
	MCS_MENU,
	MCS_BULLETSHOP
};
enum ITEMCHOICESTATE
{
	CHOICE_NONE,
	CHOICE_GUN,
	CHOICE_SHIELD
};

enum SHOPITEM
{
	ITEM_RIFLEBULLET,
	ITEM_SHOTGUNBULLET,
	ITEM_SNIPERBULLET,
	ITEM_HEAL,
	ITEM_NONE
};
class cInventory
{
private:
	cTransform* pPlayer;
	vector<Gun*>shopGun;
	vector<Gun*>invenGun;
	vector<Gun*>*playerGun;

	tag_shiled* playerShiled;
	vector<tag_shiled*>shopshiled;
	vector<tag_shiled*>invenshiled;
	RECT rcLeft;
	RECT rcRight;

	int rightimgX;
	int rightimgY;
	int leftimgX;
	int leftimgY;

	float rightscaleX;
	float rightscaleY;
	float leftscaleimgX;
	float leftscaleimgY;

	int* bulletRifleNum;
	int* bulletSniperNum;
	int* bulletShotgunNum;
	int* money;
	int* hp;
	int* maxhp;

	int gunindex;

	MENUCHOICESTATE		mcs;
	SHOPSTATE			shop;
	ITEMCHOICESTATE		choice;
	INVENTORYSTATE		Inven;
	MENUSTATE			Menu;

	LPDIRECT3DTEXTURE9 mouseimg;
	LPDIRECT3DTEXTURE9 mainbox;
	LPDIRECT3DTEXTURE9 smallbox;

	LPDIRECT3DTEXTURE9 arimg;
	LPDIRECT3DTEXTURE9 srimg;
	LPDIRECT3DTEXTURE9 sgimg;

	LPDIRECT3DTEXTURE9 arbulletimg;
	LPDIRECT3DTEXTURE9 srbulletimg;
	LPDIRECT3DTEXTURE9 sgbulletimg;

	LPDIRECT3DTEXTURE9 num1;
	LPDIRECT3DTEXTURE9 num2;
	LPDIRECT3DTEXTURE9 num3;

	LPDIRECT3DTEXTURE9 shiled1;
	LPDIRECT3DTEXTURE9 shiled2;

	LPDIRECT3DTEXTURE9 alpha;

	LPDIRECT3DTEXTURE9 minimapimg;


	LPDIRECT3DTEXTURE9 mapimg;
	LPDIRECT3DTEXTURE9 shopimg;
	LPDIRECT3DTEXTURE9 invenimg;
	LPDIRECT3DTEXTURE9 shieldimg;
	LPDIRECT3DTEXTURE9 line;
	LPDIRECT3DTEXTURE9 line2;


	LPDIRECT3DTEXTURE9 bulletRifleimg;
	LPDIRECT3DTEXTURE9 bulletShotgunimg;
	LPDIRECT3DTEXTURE9 bulletsniperimg;
	LPDIRECT3DTEXTURE9 healimg;

	LPDIRECT3DTEXTURE9 playerimg;
	LPDIRECT3DTEXTURE9 minimapbox;
	float miniscale;
	int count;
public:
	cInventory();
	~cInventory();


	HRESULT Init();
	void Release();
	bool Update(float timeDelta);
	bool Render();
	bool Scene_RenderSprite();
	bool Scene_RenderFont();

	vector<tag_shiled*>GetShopShiled() { return shopshiled; }
	void InvenShiledClead() { invenshiled.clear(); }
	void SetInvenShiled(tag_shiled* _shiled) { invenshiled.push_back(_shiled); }
	vector<tag_shiled*>GetInvenShiled() { return invenshiled; }

	vector<Gun*>GetInvenGun() { return invenGun; }
	void InvenGunClear() { invenGun.clear(); }
	void SetPlayerGun(vector<Gun*>* _gun) { playerGun = _gun; }
	void SetShopGun(Gun* _gun) { shopGun.push_back(_gun); }
	void SetGun(Gun* _gun) { invenGun.push_back(_gun); }
	void SetShiled(tag_shiled* _shiled) { playerShiled = _shiled; }
	void SetNum(int* _bulletRifleNum, int* _bulletSniperNum, int* _bulletShotgunNum, int* _money, int* _hp, int* _maxhp)
	{
		bulletRifleNum = _bulletRifleNum;
		bulletSniperNum = _bulletSniperNum;
		bulletShotgunNum = _bulletShotgunNum;
		money = _money;
		hp = _hp;
		maxhp = _maxhp;
	}
	void SetTransPlayer(cTransform* tran) { pPlayer = tran; }
	void GunTextRender(Gun* _gun);
	void ShiledTextRender(tag_shiled* _shiled);
	void GunTextRender2(Gun* _gun);
	void ShiledTextRender2(tag_shiled* _shiled);
	void OpenShop() { mcs = MCS_SHOP; };
	void OpenbulletShop() { mcs = MCS_BULLETSHOP; };

};
