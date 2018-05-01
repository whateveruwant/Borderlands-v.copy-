#include "StdAfx.h"
#include "cBoundSphere.h"
#include "cTransform.h"


cBoundSphere::cBoundSphere(void)
{
}


cBoundSphere::~cBoundSphere(void)
{
}


//월드 단위의 센터와 반지름을 얻는다.
void cBoundSphere::GetWorldCenterRadius( const  cTransform* pTrans, D3DXVECTOR3* center, float* raidus )
{
	D3DXMATRIXA16 matFinal = pTrans->GetFinalMatrix();

	//월드 센터
	D3DXVec3TransformCoord( center, &this->localCenter, &matFinal );

	D3DXVECTOR3 scale = pTrans->GetScale();

	//구의 길이
	D3DXVECTOR3 halfSize;
	halfSize.x = this->halfSize.x * scale.x;
	halfSize.y = this->halfSize.y * scale.y;
	halfSize.z = this->halfSize.z * scale.z;

	*raidus = D3DXVec3Length( &halfSize );

}

//월드 단위로 그려진다.
void cBoundSphere::RenderGizmo( const  cTransform* pTrans )
{
	D3DXVECTOR3 worldCenter;
	float radius;

	this->GetWorldCenterRadius( pTrans, &worldCenter,&radius );
	GIZMO_MGR->WireSphere( worldCenter, radius, 0xffffff00 );
}



void cBoundSphere::SetBound( const D3DXVECTOR3* pCenter, const D3DXVECTOR3* pHalfSize )
{
	this->localCenter = *pCenter;
	this->halfSize = *pHalfSize;
	this->radius = D3DXVec3Length( pHalfSize );
}