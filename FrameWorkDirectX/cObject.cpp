#include "stdafx.h"
#include "cObject.h"


cObject::cObject()
{
}


cObject::~cObject()
{
}

void cObject::Init(string filePath, D3DXVECTOR3 scale, D3DXVECTOR3 pos, D3DXVECTOR3 axis[3])
{
	this->filePath = filePath;
}

void cObject::Update(float timeDelta)
{
	cBaseObject::Update(timeDelta);
}

void cObject::Release()
{
}

void cObject::Render()
{
	cBaseObject::Render();
	//BoundBox.RenderGizmo(pTransform);
}

void cObject::RenderSprite()
{
}

void cObject::RenderFont()
{
}
