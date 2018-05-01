#pragma once
#include "cBaseObject.h"
#include "UserSetting.h"
enum eItemKind { money, shotgunBullet, rifleBullet, sniperBullet, healPak };
class cItem : public cBaseObject
{
private:
	eItemKind			mKind;
	sPlayerItems		pPlayer;
	bool*				inBox;

public:
	bool				showInfo;

	cItem();
	~cItem();

	void Init(sPlayerItems pPlayer, eItemKind kind, D3DXVECTOR3 _pos);
	void Release();
	void Update(float timeDelta);
	void Render();
	void RenderSprite();
	void RenderFont();

	bool GetPlayer();
	void SetInBox(bool* _in) {
		inBox = _in;
	}
};