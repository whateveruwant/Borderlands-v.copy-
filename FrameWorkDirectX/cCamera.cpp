#include "StdAfx.h"
#include "cPhysicManager.h"
#include "cCamera.h"

cCamera::cCamera(void)
	: bOrtho ( false )

{
	//기본화각 60 도
	this->fov = 60.0f * ONE_RAD;

	//기본 Near
	this->camNear = 0.1f;

	//기본 Far
	this->camFar = 1000.0f;


	//종횡비 Defualt;
	this->aspect = static_cast<float>(WINSIZE_X) / static_cast<float>(WINSIZE_Y);

	this->orthoSize = 10;
}


cCamera::~cCamera(void)
{
	for( int i = 0 ; i < MAX_RENDERTARGET ; i++ )
	{
		SAFE_RELEASE( this->renderTargets[i].m_pRenderSurface );
		SAFE_RELEASE( this->renderTargets[i].m_pRenderTexture );
	}

	SAFE_RELEASE( this->shadow.m_pRenderSurface );
	SAFE_RELEASE( this->shadow.m_pRenderTexture );
}


//투영행렬과 View 행렬을 업데이트
void cCamera::UpdateMatrix()
{
	if( this->bOrtho == false )
	{
		//화각에 의한 Projection 행렬 업데이트
		D3DXMatrixPerspectiveFovLH(
			&matProjection,
			this->fov, 
			this->aspect,
			this->camNear,
			this->camFar );
	}

	else
	{
		D3DXMatrixOrthoLH(
			&matProjection,
			this->aspect * orthoSize, 
			orthoSize,
			this->camNear,
			this->camFar );
	}

	//뷰행렬 카메라 월드위치에 대한 역행렬이다.
	D3DXMatrixInverse( &matView, NULL, &matFinal );

	this->matViewProjection = this->matView * this->matProjection;
}

//View 행렬과 투영 행렬을 셋팅
void cCamera::UpdateCamToDevice( LPDIRECT3DDEVICE9 pDevice )
{
	//행렬 업데이트 해주고 
	this->UpdateMatrix();

	//셋팅
	pDevice->SetTransform( D3DTS_VIEW, &this->matView );
	pDevice->SetTransform( D3DTS_PROJECTION, &this->matProjection );
}

//프러스텀 업데이트
void cCamera::UpdateFrustum()
{
	Frustum.UpdateFrustum( &this->matViewProjection );
}

//화면의 위치를 가지고 카메라의 투영 레이를 얻는다
void cCamera::ComputeRay( LPRay pOutRay, const D3DXVECTOR2* screenPos )
{
	//Device 의 뷰포트를 얻는다.
	D3DVIEWPORT9 viewPort;
	Device->GetViewport( &viewPort );
	
	//스크린의 위치 비율을 얻자 
	float factorX = screenPos->x / viewPort.Width;
	float factorY = ( 1.0f - ( screenPos->y / viewPort.Height ) );

	//0 ~ 1 => -1 ~ 1
	factorX = factorX * 2.0f - 1.0f;
	factorY = factorY * 2.0f - 1.0f;

	//투영행렬을 얻는다.
	D3DXMATRIXA16 matProj = this->GetProjectionMatrix();
	
	//

	//동차로 위치에 화각 스케일량을 나눈다.
	D3DXVECTOR3 direction(
		factorX / matProj._11,		
		factorY / matProj._22, 
		1.0f );

	//카메라 월드 행렬
	D3DXMATRIXA16 matCamWorld = GetFinalMatrix();

	//레이의 방향을 동차에서 얻은 벡터를 카메라 월드 매트릭스
	//월드 방향이 나온다.
	D3DXVec3TransformNormal(
		&direction, &direction, &matCamWorld );

	//방향벡터 정규
	D3DXVec3Normalize( &direction, &direction );

	//레이의 방향
	pOutRay->direction  = direction;

	//레이의 오리진은 카메라 위치가 된다.
	pOutRay->origin = this->GetWorldPosition();

}


//월드 위치로  화면의 위치를 얻는다.
bool cCamera::GetWorldPosToScreenPos( D3DXVECTOR2* pScreenPos, const D3DXVECTOR3* pWorldPos )
{
	//카메라의 ViewProjection 행렬을 얻는다.
	D3DXMATRIXA16 matViewProj = this->GetViewProjectionMatrix();

	//월드 위치를 동차로...
	D3DXVECTOR3 pos;
	D3DXVec3TransformCoord( &pos, pWorldPos, &matViewProj );

	//동차공간
	//				*-------*<-( 1, 1, 1 )
	//			   /|      /|
	//			  *-------* |
	//            | *-----|-*
	//		      |/      |/
	//(-1,-1, 0)->*-------* 

	//0 ~ 1 사이 값에 없으면 이면 카메라 뒤에 있다는예기
	if( pos.z < 0.0f || pos.z > 1.0f )
		return false;

	//화면 중앙에 있다면 pos xy 값은 다음과 같다.
	//pos.x == 0  
	//pos.y == 0

	//화면 절반 사이즈
	float halfScreenW = WINSIZE_X * 0.5f;
	float halfScreenH = WINSIZE_Y * 0.5f;

	//최종 화면 위치
	pScreenPos->x = halfScreenW + ( halfScreenW * pos.x );
	pScreenPos->y = halfScreenH - ( halfScreenH * pos.y );	//Y 값은 반전

	return true;
}


//랜더 Texture 를 준비 한다.
void cCamera::ReadyRenderToTexture( int idx, int width, int height )
{
	SAFE_RELEASE( this->renderTargets[idx].m_pRenderTexture );
	SAFE_RELEASE( this->renderTargets[idx].m_pRenderSurface );

	//RenderTarget 빈 Texture 만들기
	Device->CreateTexture(
		width,						//Texture 가로 해상도 
		height,						//Texture 세로 해상도
		1,							//밉맵체인 레벨
		D3DUSAGE_RENDERTARGET,		//RenderTarget 용 Texture이다
		D3DFMT_A8R8G8B8,			//TEXTURE 포맷 RenderTexture 용은D3DFMT_A8R8G8B8 로하자
		D3DPOOL_DEFAULT,			//RenderTarget 용 Texture 는 Pool 을 Default
		&( this->renderTargets[idx].m_pRenderTexture ),			//생성된 Texture 받아올 포인터
		NULL						
		);

	//Render 할 Surface 
	Device->CreateDepthStencilSurface(
		width,					//Texture 가로 해상도 
		height,					//Texture 세로 해상도
		D3DFMT_D24S8,				//Deapth 는 24 비트 Stencil 은 8 비트	
		D3DMULTISAMPLE_NONE,		//멀티 샘플링 안티알리아싱은 존재하지 않는다, 
		0,							//멀티 샘플링 퀄리티는 0
		TRUE,						//버퍼 교체시 이전 퍼버내용을 유지하지 않니? ( TRUE 로 쓰면 버퍼교체 될때 이전에 써진 버퍼내용을 기억하지 않는다 )
		&(this->renderTargets[idx].m_pRenderSurface ),			//얻어올 포인터...
		NULL);

}


//Shadow Map Texture를 준비한다.
void cCamera::ReadyShadowTexture( int size )
{
	SAFE_RELEASE( shadow.m_pRenderTexture );
	SAFE_RELEASE( shadow.m_pRenderSurface );

	//RenderTarget 빈 Texture 만들기
	Device->CreateTexture(
		size,						//Texture 가로 해상도 
		size,						//Texture 세로 해상도
		1,							//밉맵체인 레벨
		D3DUSAGE_RENDERTARGET,		//RenderTarget 용 Texture이다
		D3DFMT_R32F,				//모든 컬러 버퍼를 Red 로 다사용 ( 4byte float 실수로 사용 )
		D3DPOOL_DEFAULT,			//RenderTarget 용 Texture 는 Pool 을 Default
		&shadow.m_pRenderTexture,			//생성된 Texture 받아올 포인터
		NULL						
		);

	//Render 할 Surface 
	Device->CreateDepthStencilSurface(
		size,					//Texture 가로 해상도 
		size,					//Texture 세로 해상도
		D3DFMT_D24S8,				//Deapth 는 24 비트 Stencil 은 8 비트	
		D3DMULTISAMPLE_NONE,		//멀티 샘플링 안티알리아싱은 존재하지 않는다, 
		0,							//멀티 샘플링 퀄리티는 0
		TRUE,						//버퍼 교체시 이전 퍼버내용을 유지하지 않니? ( TRUE 로 쓰면 버퍼교체 될때 이전에 써진 버퍼내용을 기억하지 않는다 )
		&shadow.m_pRenderSurface,			//얻어올 포인터...
		NULL);

}



//랜더 Texture 로 랜더링 시작
void cCamera::RenderTextureBegin( int stageNumber, int idx, DWORD clearColor )
{
	//현 디바이스의 Target 버퍼의표면과 DepthStencil 버퍼의 표면정보를 기억
	Device->GetRenderTarget( stageNumber, &this->renderTargets[idx].m_pDeviceTargetSurface );
	Device->GetDepthStencilSurface( &this->renderTargets[idx].m_pDeviceDepthAndStencilSurface );

	//RenderTexture 의 Surface 를 얻는다.
	LPDIRECT3DSURFACE9 texSurface = NULL;
	if( SUCCEEDED( this->renderTargets[idx].m_pRenderTexture->GetSurfaceLevel( 0, &texSurface ) ) )
	{
		//Texture 표면을 Device 의 Target 버퍼로 셋팅한다.
		Device->SetRenderTarget( stageNumber, texSurface );

		//셋팅된 Surface 정보는 바로 날려주는 예의를 갖추자...
		SAFE_RELEASE( texSurface );
	}

	//Depth 버퍼와 Stencil 버퍼의 Surface 로 m_pRenderSurface 셋팅
	Device->SetDepthStencilSurface( this->renderTargets[idx].m_pRenderSurface );

	//디바이스 버퍼 클리어 ( 사실 위에서 Setting 된 Textuer 랑 Surface 가 클리어 된다  )
	Device->Clear( 0, NULL, 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		clearColor, 
		1.0f, 
		0 );

}

//랜더 Texture 로 랜더링 종료
void cCamera::RenderTextureEnd(  int stageNumber, int idx )
{
	//Render Texture 에 그릴 작업이 끝났으면 다시 원상복귀하는 센스....
	Device->SetRenderTarget( stageNumber, this->renderTargets[idx].m_pDeviceTargetSurface );
	Device->SetDepthStencilSurface( this->renderTargets[idx].m_pDeviceDepthAndStencilSurface );

	//셋팅된 Surface 정보는 바로 날려주는 예의를 갖추자...
	SAFE_RELEASE( this->renderTargets[idx].m_pDeviceTargetSurface );
	SAFE_RELEASE( this->renderTargets[idx].m_pDeviceDepthAndStencilSurface );	
}

//쉐도우 Map 랜더 시작
void cCamera::RenderShadowBegin()
{
	//현 디바이스의 Target 버퍼의표면과 DepthStencil 버퍼의 표면정보를 기억
	Device->GetRenderTarget( 0, &this->shadow.m_pDeviceTargetSurface );
	Device->GetDepthStencilSurface( &this->shadow.m_pDeviceDepthAndStencilSurface );

	//RenderTexture 의 Surface 를 얻는다.
	LPDIRECT3DSURFACE9 texSurface = NULL;
	if( SUCCEEDED( this->shadow.m_pRenderTexture->GetSurfaceLevel( 0, &texSurface ) ) )
	{
		//Texture 표면을 Device 의 Target 버퍼로 셋팅한다.
		Device->SetRenderTarget( 0, texSurface );

		//셋팅된 Surface 정보는 바로 날려주는 예의를 갖추자...
		SAFE_RELEASE( texSurface );
	}

	//Depth 버퍼와 Stencil 버퍼의 Surface 로 m_pRenderSurface 셋팅
	Device->SetDepthStencilSurface( this->shadow.m_pRenderSurface );

	//디바이스 버퍼 클리어 ( 사실 위에서 Setting 된 Textuer 랑 Surface 가 클리어 된다  )
	Device->Clear( 0, NULL, 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		0xffffffff, 
		1.0f, 
		0 );
}

//쉐도우 Map 랜더 종료
void cCamera::RenderShadowEnd()
{
	//Render Texture 에 그릴 작업이 끝났으면 다시 원상복귀하는 센스....
	Device->SetRenderTarget( 0, this->shadow.m_pDeviceTargetSurface );
	Device->SetDepthStencilSurface( this->shadow.m_pDeviceDepthAndStencilSurface );

	//셋팅된 Surface 정보는 바로 날려주는 예의를 갖추자...
	SAFE_RELEASE( this->shadow.m_pDeviceTargetSurface );
	SAFE_RELEASE( this->shadow.m_pDeviceDepthAndStencilSurface );	
}



//랜터 Texture 얻는다.
LPDIRECT3DTEXTURE9 cCamera::GetRenderTexture( int idx )
{
	return this->renderTargets[idx].m_pRenderTexture;
}


//쉐도우 Texture 얻는다.
LPDIRECT3DTEXTURE9 cCamera::GetShadowTexture()
{
	return this->shadow.m_pRenderTexture;
}