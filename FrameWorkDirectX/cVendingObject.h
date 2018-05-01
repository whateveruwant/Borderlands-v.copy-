#pragma once
#include "cObject.h"
#include "Character.h"
#include "cInventory.h"
enum eVendingKind { EquipmentStore, SaveStore, ItemStore };
class cVendingObject : public cObject
{
private:
	eVendingKind			mVendingKind;
	bool					isTrigger;					// ¿€µø
	
	///////////////////////////////////////////////////////////////////////////
	Character*				pCharcter;
	cInventory*				pInventory;

public:
	cVendingObject();
	~cVendingObject();

	virtual void Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3], eVendingKind kind);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();

	void SetCharAndInven(Character* cha, cInventory* inven) {
		pCharcter = cha;
		pInventory = inven;
	}
	void ActionKind();
};