#pragma once

#include "cTransform.h"
#include "cFrustum.h"

#define MAX_RENDERTARGET 10

class cCamera : public cTransform
{
public:
	struct RenderTarget{
		LPDIRECT3DTEXTURE9		m_pRenderTexture;				//랜더링될 Texture
		LPDIRECT3DSURFACE9		m_pRenderSurface;				//랜더 Texture Deapth 버퍼와 Stencil 버퍼가 있는 Surface
		LPDIRECT3DSURFACE9		m_pDeviceTargetSurface;
		LPDIRECT3DSURFACE9		m_pDeviceDepthAndStencilSurface;

		RenderTarget(){
			m_pRenderTexture = NULL;			
			m_pRenderSurface = NULL;						
			m_pDeviceTargetSurface = NULL;		
			m_pDeviceDepthAndStencilSurface = NULL;		
		}
	};


public:
	float fov;			//카메라 화각
	float camNear;		//카메라 Near
	float camFar;		//카메라 Far
	float aspect;		//카메라 종횡비
	bool bOrtho;		//직교니?
	float orthoSize;	//직교시사이즈

protected:
	D3DXMATRIXA16		matView;		//뷰행렬
	D3DXMATRIXA16		matProjection;	//투영행렬
	D3DXMATRIXA16		matViewProjection;	//뷰 * 투영행렬

private:
	RenderTarget		renderTargets[MAX_RENDERTARGET];
	RenderTarget		shadow;

public:
	cFrustum			Frustum;


public:
	cCamera(void);
	~cCamera(void);


	//투영행렬과 View 행렬을 업데이트
	void UpdateMatrix();

	//View 행렬과 투영 행렬을 셋팅
	void UpdateCamToDevice( LPDIRECT3DDEVICE9 pDevice );

	//프러스텀 업데이트
	void UpdateFrustum();

	//카메라 관련 행렬을 얻는다.
	D3DXMATRIXA16 GetViewMatrix() const{
		return this->matView;
	}

	D3DXMATRIXA16 GetProjectionMatrix() const{
		return this->matProjection;
	}

	D3DXMATRIXA16 GetViewProjectionMatrix() const{
		return this->matViewProjection;
	}

	//화각 셋팅
	void SetFov( float fov ){
		this->fov = fov;
	}

	float GetFov(){
		return this->fov;
	}

	void OrthoToggle(){
		this->bOrtho = !this->bOrtho;
	}

	//화면의 위치를 가지고 카메라의 투영 레이를 얻는다
	void ComputeRay( LPRay pOutRay, const D3DXVECTOR2* screenPos ); 

	//월드 위치로  화면의 위치를 얻는다.
	bool GetWorldPosToScreenPos( D3DXVECTOR2* pScreenPos, const D3DXVECTOR3* pWorldPos );




	//랜더 Texture 를 준비 한다. ( 생성된 RenderTarget 인덱스 )
	void ReadyRenderToTexture( int idx, int width, int height );

	//Shadow Map Texture를 준비한다.
	void ReadyShadowTexture( int size );

	//랜더 Texture 로 랜더링 시작 ( 랜더링될 스테이지 넘버 와 RenderTarget Index )
	void RenderTextureBegin( int stageNumber, int idx, DWORD clearColor );

	//랜더 Texture 로 랜더링 종료 ( 랜더링이 종료될 스테이지 넘버 와 RenderTarget Index )
	void RenderTextureEnd( int stageNumber, int idx);

	//쉐도우 Map 랜더 시작
	void RenderShadowBegin();

	//쉐도우 Map 랜더 종료
	void RenderShadowEnd();



	//랜터 Texture 얻는다. ( 얻어올 랜더 인덱스 )
	LPDIRECT3DTEXTURE9 GetRenderTexture( int idx );

	//쉐도우 Texture 얻는다.
	LPDIRECT3DTEXTURE9 GetShadowTexture();


};

