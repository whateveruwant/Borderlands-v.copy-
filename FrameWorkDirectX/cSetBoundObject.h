#pragma once
#include "cbaseobject.h"


class cSetBoundObject : public cBaseObject
{
public:
	D3DXVECTOR3		center;
	D3DXVECTOR3		halfSize;

public:
	cSetBoundObject(void);
	~cSetBoundObject(void);

	virtual void BaseObjectUpdate( float timeDelta ) override;


};

