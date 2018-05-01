#include "StdAfx.h"
#include "cFrustum.h"
#include "cTransform.h"

cFrustum::cFrustum(void)
{
	//동차 좌표 정보 저장
	//					  4-------5  <-( 1, 1, 1 )
	//					 /|		 /|
	//					0-------1 |
	//					| 6-----|-7
	//					|/      |/
	//( -1, -1, 0 )->	2-------3
	m_RHWPos[0] = D3DXVECTOR3( -1,  1,  0 );
	m_RHWPos[1] = D3DXVECTOR3(  1,  1,  0 );
	m_RHWPos[2] = D3DXVECTOR3( -1, -1,  0 );
	m_RHWPos[3] = D3DXVECTOR3(  1, -1,  0 );
	m_RHWPos[4] = D3DXVECTOR3( -1,  1,  1 );
	m_RHWPos[5] = D3DXVECTOR3(  1,  1,  1 );
	m_RHWPos[6] = D3DXVECTOR3( -1, -1,  1 );
	m_RHWPos[7] = D3DXVECTOR3(  1, -1,  1 );

}


cFrustum::~cFrustum(void)
{
}

//프러스텀 정보 업데이트 ( View * Projection 행렬을 요구한다 )
void cFrustum::UpdateFrustum( const D3DXMATRIXA16* pmatViewProj )
{
	//ViewProjection 의 역행렬
	D3DXMATRIXA16		matInv;
	D3DXMatrixInverse( &matInv, NULL, pmatViewProj );

	//동차 좌표의 정점을 역행렬에 적용
	for( int i = 0 ; i < 8 ;  i++ ){
		D3DXVec3TransformCoord( m_Pos + i, m_RHWPos + i, &matInv );
	}

	
	//플랜 업데이트
	//동차 좌표 정보 저장
	//					  4-------5  <-( 1, 1, 1 )
	//					 /|		 /|
	//					0-------1 |
	//					| 6-----|-7
	//					|/      |/
	//( -1, -1, 0 )->	2-------3

	//정면 플랜
	PHYSICS_MGR->CreatePlane( &m_Plane[0], &m_Pos[0], &m_Pos[1], &m_Pos[2] );

	//후면
	PHYSICS_MGR->CreatePlane( &m_Plane[1], &m_Pos[5], &m_Pos[4], &m_Pos[7] );

	//우측
	PHYSICS_MGR->CreatePlane( &m_Plane[2], &m_Pos[1], &m_Pos[5], &m_Pos[3] );

	//좌측
	PHYSICS_MGR->CreatePlane( &m_Plane[3], &m_Pos[4], &m_Pos[0], &m_Pos[6] );

	//상방
	PHYSICS_MGR->CreatePlane( &m_Plane[4], &m_Pos[4], &m_Pos[5], &m_Pos[0] );

	//하단
	PHYSICS_MGR->CreatePlane( &m_Plane[5], &m_Pos[2], &m_Pos[3], &m_Pos[6] );

}

//Base 오브젝트가 프러스텀 안에있는지 체크
bool cFrustum::IsInFrustum( cBaseObject* pObject )
{
	//월드 구정보
	D3DXVECTOR3 worldCenter;
	float radius;

	pObject->BoundBox.GetWorldCenterRadius( 
		pObject->pTransform, 
		&worldCenter, 
		&radius );
		

	//worldCenter 가 절두체의 각 6 면안쪽에 있는지 확인
	for( int i = 1 ; i < 6 ; i++ )
	{
		//면과의 거리
		float dist = PHYSICS_MGR->PlaneDot( 
			&m_Plane[i], &worldCenter );

		//반지름 이상 벗어나면 절두체 안에 없음....
		if( dist > radius )
			return false;
	}

	return true;
}

//프러스텀 기즈모를 그린다.
void cFrustum::RenderGizmo()
{
	DWORD color = 0xff00ffff;
	//동차 좌표 정보 저장
	//					  4-------5  <-( 1, 1, 1 )
	//					 /|		 /|
	//					0-------1 |
	//					| 6-----|-7
	//					|/      |/
	//( -1, -1, 0 )->	2-------3

	GIZMO_MGR->Line( m_Pos[0], m_Pos[1], color );
	GIZMO_MGR->Line( m_Pos[1], m_Pos[3], color );
	GIZMO_MGR->Line( m_Pos[3], m_Pos[2], color );
	GIZMO_MGR->Line( m_Pos[2], m_Pos[0], color );
							   
							   
	GIZMO_MGR->Line( m_Pos[4], m_Pos[5], color );
	GIZMO_MGR->Line( m_Pos[5], m_Pos[7], color );
	GIZMO_MGR->Line( m_Pos[7], m_Pos[6], color );
	GIZMO_MGR->Line( m_Pos[6], m_Pos[4], color );
							   
							   
	GIZMO_MGR->Line( m_Pos[0], m_Pos[4], color );
	GIZMO_MGR->Line( m_Pos[1], m_Pos[5], color );
	GIZMO_MGR->Line( m_Pos[3], m_Pos[7], color );
	GIZMO_MGR->Line( m_Pos[2], m_Pos[6], color );
}
