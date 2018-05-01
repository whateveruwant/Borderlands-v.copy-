#pragma once

#include "cBoundSphere.h"


class cBoundBox : public cBoundSphere
{
public:
	D3DXVECTOR3 localMinPos;
	D3DXVECTOR3 localMaxPos;

public:
	cBoundBox(void);
	~cBoundBox(void);

	void GetWorldBox(const cTransform* pTrans, D3DXVECTOR3* outBoxPos );
	void GetWorldAABBMinMax( const cTransform* pTrans, D3DXVECTOR3* min, D3DXVECTOR3* max );


	//월드 단위로 그려진다.
	virtual void RenderGizmo( const cTransform* pTrans ) override;
	virtual void SetBound( const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize ) override;

};

