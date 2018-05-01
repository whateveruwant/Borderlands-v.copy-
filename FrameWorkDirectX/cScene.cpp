#include "StdAfx.h"
#include "cScene.h"
#include "cCamera.h"
#include "cLight_Direction.h"
#include "cTerrain.h"

cScene::cScene(void)
	: evironmentTexture( NULL ), evironmemtSphereMesh( NULL )
{
	//환경 Effect 물어 놓는다.
	this->evironmentEffect = 
		RESOURCE_FX->GetResource( "../Resources/Shaders/EvironmentCUBE.fx");

	this->pMainCamera = new cCamera();
	//기본 광원 생성
	this->pSceneBaseDirectionLight = new cLight_Direction();
	//방향성 광원에 카메라 를 생성한다.
	this->pDirectionLightCamera = new cCamera();

	//방향성광원 자식에 붙인다.
	//this->pDirectionLightCamera->AttachTo( this->pSceneBaseDirectionLight->pTransform );

	//기본 값
	this->pSceneBaseDirectionLight->Color = D3DXCOLOR( 1, 1, 1, 1 );
	this->pSceneBaseDirectionLight->Intensity = 0.8f;
	for (int i = 0; i < 4; ++i) {
		cLight_Direction* tmpLight = new cLight_Direction();
		tmpLight->Color = D3DXCOLOR(1, 1, 1, 1);
		tmpLight->Intensity = 0.35f;
		tmpLight->pTransform->RotateWorld( 0, 90.0f * ONE_RAD * i, 0);
		pSceneDarkLight.push_back(tmpLight);
	}
	//그림자 거리
	shadowDistance = 100.0f;

	//카메라의 투영방식을 바꾼다...
	this->pDirectionLightCamera->bOrtho = true;
	this->pDirectionLightCamera->camNear = 0.1f;
	this->pDirectionLightCamera->camFar = shadowDistance * 2.0f;
	this->pDirectionLightCamera->aspect = 1;
	this->pDirectionLightCamera->orthoSize = shadowDistance * 1.5f;	//투영크기는 그림자크기로...

	//방향성광원 카메라의 RenderToTexture 준비
	this->pDirectionLightCamera->ReadyShadowTexture( 4096 );
}


cScene::~cScene(void)
{
	SAFE_RELEASE(evironmentTexture);
	SAFE_RELEASE(evironmemtSphereMesh);

	SAFE_DELETE(this->pMainCamera);

	SAFE_DELETE(this->pSceneBaseDirectionLight);
	SAFE_DELETE(this->pDirectionLightCamera);
	for (int i = 0; i < 4; ++i) {
		SAFE_DELETE(pSceneDarkLight[i]);
	}
}


HRESULT cScene::Init()
{
	//씬의 초기화 이루어진다.
	if( FAILED( this->Scene_Init() ) )
		return E_FAIL;

	//메인카메라 RenderToTexture 준비
	this->pMainCamera->ReadyRenderToTexture( 0, WINSIZE_X, WINSIZE_Y );
	this->pMainCamera->ReadyRenderToTexture( 1, WINSIZE_X, WINSIZE_Y );
	this->pMainCamera->ReadyRenderToTexture( 2, WINSIZE_X, WINSIZE_Y );

	this->pMainCamera->ReadyRenderToTexture( 5, WINSIZE_X / 4, WINSIZE_Y / 4 );		//Blur Temp1
	this->pMainCamera->ReadyRenderToTexture( 6, WINSIZE_X / 4, WINSIZE_Y / 4 );		//Blur Temp2
	
	this->pMainCamera->ReadyRenderToTexture( 9, WINSIZE_X, WINSIZE_Y );		//Grap 용


	//0----1
	//|   /|
	//|  / |
	//| /  |
	//|/   |
	//3----2

	this->scenePlaneVertex[0].pos = D3DXVECTOR3( -1.0f, 1.0f, 0.0f );
	this->scenePlaneVertex[1].pos = D3DXVECTOR3(  1.0f, 1.0f, 0.0f );
	this->scenePlaneVertex[2].pos = D3DXVECTOR3(  1.0f,-1.0f, 0.0f );
	this->scenePlaneVertex[3].pos = D3DXVECTOR3( -1.0f,-1.0f, 0.0f );

	this->scenePlaneVertex[0].uv = D3DXVECTOR2( 0.0f, 0.0f );
	this->scenePlaneVertex[1].uv = D3DXVECTOR2( 1.0f, 0.0f );
	this->scenePlaneVertex[2].uv = D3DXVECTOR2( 1.0f, 1.0f );
	this->scenePlaneVertex[3].uv = D3DXVECTOR2( 0.0f, 1.0f );

	this->scenePlaneIndex[0] = 0;
	this->scenePlaneIndex[1] = 1;
	this->scenePlaneIndex[2] = 3;
	this->scenePlaneIndex[3] = 3;
	this->scenePlaneIndex[4] = 1;
	this->scenePlaneIndex[5] = 2;


	this->postEffect = RESOURCE_FX->GetResource( "../Resources/Shaders/PostEffect.fx" );

	//키설정
	gGameKey.name.JUMP = VK_SPACE;
	gGameKey.name.RUN = VK_SHIFT;
	gGameKey.name.SITDOWN = 'C';
	gGameKey.name.FIRE = VK_LBUTTON;
	gGameKey.name.PREVWEAPON = 'R'; //wheelUP();
	gGameKey.name.NEXTWEAPON = 'R'; // wheelDOWN();
	gGameKey.name.RELOAD = 'R';
	gGameKey.name.ATTACK = 'V';
	gGameKey.name.DRAWBOOM = 'G';
	gGameKey.name.WEAPON1st = '1';
	gGameKey.name.WEAPON2st = '2';
	gGameKey.name.WEAPON3st = '3';

	gGameKey.name.DOWN = 'S';
	gGameKey.name.UP = 'W';
	gGameKey.name.LEFT = 'A';
	gGameKey.name.RIGHT = 'D';
	gGameKey.name.TAKE = 'E';
	gGameKey.name.ZOOM = VK_RBUTTON;

	gGameElse.background = 1.0f;
	gGameElse.effect = 1.0f;
	gGameElse.mouseSpeed = 1.0f;

	return S_OK;
}

void cScene::Release()
{
	//씬의 해제가 일어난다.
	this->Scene_Release();
}

void cScene::Update( float timeDelta )
{
	//pMainCamera->DefaultControl( timeDelta );
	this->Scene_Update(timeDelta);

	pMainCamera->UpdateCamToDevice( Device );
	pMainCamera->UpdateFrustum();
	//메인카메라에 DirectionLight 를 방향을 유지한체 따라다니게....
	
	//광원 위치
	D3DXVECTOR3 camPos = pMainCamera->GetWorldPosition();		//메인카메라의 위치
	D3DXVECTOR3 camFront = pMainCamera->GetForward();			//메인카메라의 정면
	D3DXVECTOR3 lightDir = pSceneBaseDirectionLight->pTransform->GetForward();	//방향성 광원의 방향

	D3DXVECTOR3 lightPos = camPos + 
		( camFront * ( shadowDistance * 0.5f ) ) + 
		( -lightDir * shadowDistance );

	this->pDirectionLightCamera->SetWorldPosition(lightPos.x, lightPos.y, lightPos.z );
	this->pDirectionLightCamera->LookDirection( lightDir );

	//씬의 업데이트가 일어난다.
	//this->Scene_Update(timeDelta);

}

void cScene::Render()
{
	this->pMainCamera->RenderTextureBegin( 0, 0, 0x00101010 );		//0 번 스테이지에 0 번 RenderTarget
	this->pMainCamera->RenderTextureBegin( 1, 1, 0xff000000 );		//1 번 스테이지에 1 번 RenderTarget
	//this->pMainCamera->RenderTextureBegin( 2, 2, 0x00000000 );		//2 번 스테이지에 2 번 RenderTarget

	//환경 랜더
	this->RenderEnvironment();

	//랜더된다.
	this->Scene_Render0();
	this->Scene_Render1();
	this->Scene_Render2();
	
	
#ifdef _DEBUG		//디버그 모드에서만 실행
		//디바이스 랜더링 종료 명령
		//월드 그리드
		//GIZMO_MGR->WorldGrid( 1, 10 );
#endif
	
	//중요 RenderTextureBegin 역순으로 간다.
	//this->pMainCamera->RenderTextureEnd( 2, 2 );		//2 번스테이지에 셋팅된 2 RenderTarget 을 종료 시킨다. 
	this->pMainCamera->RenderTextureEnd( 1, 1 );		//1 번스테이지에 셋팅된 1 RenderTarget 을 종료 시킨다. 
	this->pMainCamera->RenderTextureEnd( 0, 0 );		//0 번스테이지에 셋팅된 0 RenderTarget 을 종료 시킨다. 
	

	//
	// Post Effect
	//

	this->RenderBase();
	this->RenderOutline();		//아웃라인 그려줌
	//this->RenderGlow();		//밝게 보이게 해줌


	this->Scene_Render_AfterPostEffect( this->pMainCamera->GetRenderTexture( 0 ) );


	//스플라이트 랜더
	SPRITE_MGR->BeginSpriteRender();
	this->Scene_RenderSprite();
	SPRITE_MGR->EndSpriteRender();

	Scene_RenderFont();

}

//메인 카메라의 RenderToTexture 만 업데이트한다.
void cScene::RenderToMainCamTexture()
{
}


void cScene::SetEnvironment( std::string cubeFilePath )
{
	//기존의 환경맵이 셋팅되어있을지 몰라..
	SAFE_RELEASE( evironmentTexture );

	//환경맵 로딩
	D3DXCreateCubeTextureFromFile(
		Device,
		cubeFilePath.c_str(),
		&evironmentTexture );

	//환경구 로딩되어있지 않다면....
	if( this->evironmemtSphereMesh == NULL )
	{
		D3DXCreateSphere( Device, 
			3.0f,
			20, 
			20, 
			&evironmemtSphereMesh, NULL );
	}

}


//환경구 랜더
void cScene::RenderEnvironment()
{
	//환경 맵 로딩된게 없다면 리턴해라.....
	if( this->evironmentTexture == NULL )
		return;

	//환경 Effect 셋팅
	this->evironmentEffect->SetTexture("MyCube_Tex", this->evironmentTexture);

	//WVP 매트릭스
	D3DXMATRIXA16 matWorld = this->pMainCamera->GetFinalMatrix();
	D3DXMATRIXA16 matViewProj = this->pMainCamera->GetViewProjectionMatrix();
	D3DXMATRIXA16 matWVP = matWorld * matViewProj;

	this->evironmentEffect->SetMatrix( "matWVP", &matWVP );
	this->evironmentEffect->SetMatrix( "matWorld", &matWorld );


	//그려라...
	UINT numPass;
	this->evironmentEffect->Begin( &numPass, 0 );		//셰이더로 그린다는 것을 알리고 pass 수를 얻는다.

	for( UINT i = 0 ; i < numPass ; i++ )	//공정 수대로 돈다.
	{
		this->evironmentEffect->BeginPass( i );			//i번째 공정시작

		this->evironmemtSphereMesh->DrawSubset(0);

		this->evironmentEffect->EndPass();				//i번째 공정끝
	}
	this->evironmentEffect->End();						//셰이더로 그리기 끝



}


void cScene::ReadyShadowMap( std::vector<cBaseObject*>* renderObjects, cTerrain* pTerrain )
{
	//방향성광원에 붙은 카메라의 Frustum 업데이트
	this->pDirectionLightCamera->UpdateMatrix();
	this->pDirectionLightCamera->UpdateFrustum();


	//다이렉션라이팅 카메라에 들어오는 애들만 그린다...
	static std::vector<cBaseObject*>		shadowCullObject;
	shadowCullObject.clear();

	for( int i = 0 ; i < renderObjects->size() ; i++ ){

		//프러스텀 안에 있니?
		if( this->pDirectionLightCamera->Frustum.IsInFrustum( (*renderObjects)[i] ) )
		{
			shadowCullObject.push_back( (*renderObjects)[i] );
		}
	}


	//쉐도우 맵 그린다.
	this->pDirectionLightCamera->RenderShadowBegin();

	cXMesh_Static::SetCamera( this->pDirectionLightCamera );
	cXMesh_Skinned::SetCamera( this->pDirectionLightCamera );

	for( int i = 0 ; i < shadowCullObject.size() ; i++ ){
		shadowCullObject[i]->RenderShadow();
	}

	//만약 Terrain 도 쉐도우 맵을 그려야한다면...
	if( pTerrain != NULL )
	{
		pTerrain->RenderShadow( this->pDirectionLightCamera ); 
	}

	this->pDirectionLightCamera->RenderShadowEnd();

	
	//만약 Terrain 도 쉐도우 맵을 셋팅한다면...
	if( pTerrain != NULL )
	{
		pTerrain->m_pTerrainEffect->SetTexture( "Shadow_Tex", 
				this->pDirectionLightCamera->GetShadowTexture() );

		pTerrain->m_pTerrainEffect->SetMatrix( "matLightViewProjection", 
				&this->pDirectionLightCamera->GetViewProjectionMatrix() );
	}



	//쉐도우 Texture
	cXMesh_Static::sStaticMeshEffect->SetTexture( "Shadow_Tex", 
		this->pDirectionLightCamera->GetShadowTexture() );
	
	//쉐도우 행렬
	cXMesh_Static::sStaticMeshEffect->SetMatrix( "matLightViewProjection", 
		&this->pDirectionLightCamera->GetViewProjectionMatrix() );


	//쉐도우 Texture
	cXMesh_Skinned::sSkinnedMeshEffect->SetTexture( "Shadow_Tex", 
		this->pDirectionLightCamera->GetShadowTexture() );
	
	//쉐도우 행렬
	cXMesh_Skinned::sSkinnedMeshEffect->SetMatrix( "matLightViewProjection", 
		&this->pDirectionLightCamera->GetViewProjectionMatrix() );
}


//메인카메라의 랜더 Texture 를 얻는다.
LPDIRECT3DTEXTURE9 cScene::GetTexture(){
		return this->pMainCamera->GetRenderTexture(0);
}



/// Post Effect Functions ///////////////////////////////////////////////////////////////////

void cScene::RenderBase()
{
	this->postEffect->SetTechnique( "Base" );

	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );

	float pixelU = 1.0f / WINSIZE_X;
	float pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();
}


void cScene::RenderOutline()
{
	this->postEffect->SetTechnique( "OutLine" );

	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	this->postEffect->SetTexture( "normalTex", this->pMainCamera->GetRenderTexture(1) );

	float pixelU = 1.0f / WINSIZE_X;
	float pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();
}


void cScene::RenderBBo()
{
	
	//
	// Blur Scene 랜더
	//

	float pixelU = 1.0f / WINSIZE_X;
	float pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );


	for( int i = 0 ; i < 10 ; i++ )
	{

		//BlurX

		this->pMainCamera->RenderTextureBegin( 0, 5, 0x00101010 );
		this->postEffect->SetTechnique( "BlurX" );

		if( i == 0 )
		{
			this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
		}

		else
		{
			this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(6) );
		}

		UINT iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 5 );


		//BlurY

		pixelU = 1.0f / ( WINSIZE_X / 4 );		//5 번 Texture 의 Size는 WINSIZE_X / 2 
		pixelV = 1.0f / ( WINSIZE_Y / 4 );		//5 번 Texture 의 Size는 WINSIZE_X / 2 
		this->postEffect->SetFloat( "pixelSizeU", pixelU );
		this->postEffect->SetFloat( "pixelSizeV", pixelV );
		this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
		this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );


		this->pMainCamera->RenderTextureBegin( 0, 6, 0x00101010 );
		this->postEffect->SetTechnique( "BlurY" );
		this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(5) );

		iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 6 );

	}
	
	//
	// BBo Render
	//
	this->postEffect->SetTechnique( "BBo" );
	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	this->postEffect->SetTexture( "blurTex", this->pMainCamera->GetRenderTexture(6) );
	
	pixelU = 1.0f / WINSIZE_X;
	pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();
}


void cScene::RenderDepthOfField()
{
	//
	// Blur Scene 랜더
	//

	float pixelU = 1.0f / WINSIZE_X;
	float pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );


	for( int i = 0 ; i < 10 ; i++ )
	{

		//BlurX

		this->pMainCamera->RenderTextureBegin( 0, 5, 0x00101010 );
		this->postEffect->SetTechnique( "BlurX" );

		if( i == 0 )
		{
			this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
		}

		else
		{
			this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(6) );
		}

		UINT iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 5 );


		//BlurY

		pixelU = 1.0f / ( WINSIZE_X / 4 );		//5 번 Texture 의 Size는 WINSIZE_X / 2 
		pixelV = 1.0f / ( WINSIZE_Y / 4 );		//5 번 Texture 의 Size는 WINSIZE_X / 2 
		this->postEffect->SetFloat( "pixelSizeU", pixelU );
		this->postEffect->SetFloat( "pixelSizeV", pixelV );
		this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
		this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );


		this->pMainCamera->RenderTextureBegin( 0, 6, 0x00101010 );
		this->postEffect->SetTechnique( "BlurY" );
		this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(5) );

		iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 6 );

	}



	this->postEffect->SetTechnique( "DepthOfField" );
	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	this->postEffect->SetTexture( "blurTex", this->pMainCamera->GetRenderTexture(6) );
	this->postEffect->SetTexture( "normalTex", this->pMainCamera->GetRenderTexture(1) );
	
	pixelU = 1.0f / WINSIZE_X;
	pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();
}


void cScene::RenderColorLevel()
{
	//
	// Color Level
	//

	this->postEffect->SetTechnique( "ColorLevel" );
	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	
	this->postEffect->SetFloat( "InBlack", 79 );
	this->postEffect->SetFloat( "InWhite", 216 );
	this->postEffect->SetFloat( "Gamma", 0.39 );
	this->postEffect->SetFloat( "OutBlack", 15 );
	this->postEffect->SetFloat( "OutWhite", 255 );

	float pixelU = 1.0f / WINSIZE_X;
	float pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();
}


void cScene::RenderGlow()
{
	//
	// Color Level
	//

	//5 번에 그린다.
	this->pMainCamera->RenderTextureBegin( 0, 5, 0x00101010 );

	this->postEffect->SetTechnique( "ColorLevel" );
	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	
	this->postEffect->SetFloat( "InBlack", 79 );
	this->postEffect->SetFloat( "InWhite", 139 );
	this->postEffect->SetFloat( "Gamma", 0.54f );
	this->postEffect->SetFloat( "OutBlack", 0 );
	this->postEffect->SetFloat( "OutWhite", 200 );

	float pixelU = 1.0f / ( WINSIZE_X / 4 );
	float pixelV = 1.0f / ( WINSIZE_Y / 4 );
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	UINT iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );

		this->postEffect->EndPass();
	}
	this->postEffect->End();

	this->pMainCamera->RenderTextureEnd( 0, 5 );

	//
	// Color Level 조정된거 블러 처리
	//
	for( int i = 0 ; i < 10 ; i++ )
	{
		//BlurX

		this->pMainCamera->RenderTextureBegin( 0, 6, 0x00101010 );
		this->postEffect->SetTechnique( "BlurX" );
		this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(5) );

		UINT iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 6 );


		//BlurY
		this->pMainCamera->RenderTextureBegin( 0, 5, 0x00101010 );
		this->postEffect->SetTechnique( "BlurY" );
		this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(6) );

		iPass;
		this->postEffect->Begin( &iPass, 0 );
		for( UINT i = 0 ; i < iPass ; i++ ){
			this->postEffect->BeginPass(i);

			Device->SetFVF( SCENE_VERTEX::FVF );
			Device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 
				0, 
				4, 
				2, 
				this->scenePlaneIndex, 
				D3DFMT_INDEX16, 
				this->scenePlaneVertex, 
				sizeof( SCENE_VERTEX ) );


			this->postEffect->EndPass();
		}
		this->postEffect->End();

		this->pMainCamera->RenderTextureEnd( 0, 5 );

	}



	this->postEffect->SetTechnique( "BBo" );
	this->postEffect->SetFloat( "blurAmout", 1.5f );
	this->postEffect->SetTexture( "screenTex", this->pMainCamera->GetRenderTexture(0) );
	this->postEffect->SetTexture( "blurTex", this->pMainCamera->GetRenderTexture(5) );
	
	pixelU = 1.0f / WINSIZE_X;
	pixelV = 1.0f / WINSIZE_Y;
	this->postEffect->SetFloat( "pixelSizeU", pixelU );
	this->postEffect->SetFloat( "pixelSizeV", pixelV );
	this->postEffect->SetFloat( "pixelHalfSizeU", pixelU * 0.5f );
	this->postEffect->SetFloat( "pixelHalfSizeV", pixelV * 0.5f );

	iPass;
	this->postEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		this->postEffect->BeginPass(i);

		Device->SetFVF( SCENE_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->scenePlaneIndex, 
			D3DFMT_INDEX16, 
			this->scenePlaneVertex, 
			sizeof( SCENE_VERTEX ) );


		this->postEffect->EndPass();
	}
	this->postEffect->End();


}
