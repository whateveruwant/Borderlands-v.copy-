#pragma once
#include "cBaseObject.h"
#define	OBJECTMAXNUM 100

struct sObjectSaveData {
	bool			isSet;
	
	char			sfilePath[100];
	
	D3DXVECTOR3		scale;
	D3DXVECTOR3		position;
	D3DXVECTOR3		axis[3];
	//D3DXVECTOR3		right;
	//D3DXVECTOR3		up;
	//D3DXVECTOR3		forward;
	
	bool			haveAnimation;
};
enum eObjectKind { eObjectKind_skel, eObjectKind_static, eObjectKind_vending};
class cObject :public cBaseObject
{
protected:
	string filePath;
	bool ChangeMap;

public:
	eObjectKind mObjKind;
	bool isSee;
	bool isOpen;

	cObject();
	~cObject();

	virtual void Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3]);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual bool GetPlayAnim() { return false;	 }

	string GetFileName() {
		return filePath;
	}
	void SetChangeMap(bool tf) { ChangeMap = tf; }
	bool GetChangeMap() {
		return ChangeMap;
	}
};