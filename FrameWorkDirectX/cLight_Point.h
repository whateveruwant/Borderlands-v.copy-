#pragma once
#include "clight.h"

class cLight_Point : public cLight
{
public:
	float				minRange;			//빛의 최소거리 
	float				maxRange;			//빛의 최대거리
	float				distancePow;		//거리 감쇠율


public:
	cLight_Point(void);
	~cLight_Point(void);

	virtual D3DXMATRIXA16 GetLightMatrix();

	virtual void RenderGizmo();

};

