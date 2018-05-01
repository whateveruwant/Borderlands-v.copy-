#pragma once
#include "cObject.h"
class cSkelObject : public cObject
{
private:

public:

	cSkelObject();
	~cSkelObject();

	virtual void Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3]);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();

	void Open();
	bool* GetIsOpen();
};