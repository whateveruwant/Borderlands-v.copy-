#pragma once
#include "clight.h"

class cLight_Direction : public cLight
{
public:
	cLight_Direction(void);
	~cLight_Direction(void);

	virtual D3DXMATRIXA16 GetLightMatrix();

	virtual void RenderGizmo();

};

