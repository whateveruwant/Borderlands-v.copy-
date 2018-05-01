#include "StdAfx.h"
#include "cQuad.h"
#include "cTransform.h"


cQuad::cQuad(void)
{
	//0----1
	//|   /|
	//|  / |
	//| /  |
	//|/   |
	//3----2

	this->quadVertex[0].pos = D3DXVECTOR3( -1.0f, 1.0f, 1.0f );
	this->quadVertex[1].pos = D3DXVECTOR3(  1.0f, 1.0f, 1.0f );
	this->quadVertex[2].pos = D3DXVECTOR3(  1.0f, 1.0f, 0.0f );
	this->quadVertex[3].pos = D3DXVECTOR3( -1.0f, 1.0f, 0.0f );

	this->quadVertex[0].uv = D3DXVECTOR2( 0.0f, 0.0f );
	this->quadVertex[1].uv = D3DXVECTOR2( 1.0f, 0.0f );
	this->quadVertex[2].uv = D3DXVECTOR2( 1.0f, 1.0f );
	this->quadVertex[3].uv = D3DXVECTOR2( 0.0f, 1.0f );


	this->quadPlaneIndex[0] = 0;
	this->quadPlaneIndex[1] = 1;
	this->quadPlaneIndex[2] = 3;
	this->quadPlaneIndex[3] = 3;
	this->quadPlaneIndex[4] = 1;
	this->quadPlaneIndex[5] = 2;
}


cQuad::~cQuad(void)
{
	
}

void cQuad::Render( cTransform* pTransform, LPDIRECT3DTEXTURE9 grab, LPDIRECT3DTEXTURE9 distortNormal, float distrotAmount )
{
	cXMesh_Static::sStaticMeshEffect->SetMatrix( "matWorld", &pTransform->GetFinalMatrix() );
	cXMesh_Static::sStaticMeshEffect->SetTexture( "GrabTex", grab );		//이전까지 그려진 화면의 내용 Texture
	cXMesh_Static::sStaticMeshEffect->SetTexture( "Normal_Tex", distortNormal );	//왜곡형태를 결정하는 Normal Texture
	cXMesh_Static::sStaticMeshEffect->SetTechnique( "Distort" );
	cXMesh_Static::sStaticMeshEffect->SetFloat( "distortAmout", distrotAmount );

	cXMesh_Static::sStaticMeshEffect->SetFloat( "scrollU", TIME_MGR->GetTotalDeltaSec() );

	UINT iPass;
	cXMesh_Static::sStaticMeshEffect->Begin( &iPass, 0 );
	for( UINT i = 0 ; i < iPass ; i++ ){
		cXMesh_Static::sStaticMeshEffect->BeginPass(i);

		Device->SetFVF( QUAD_VERTEX::FVF );
		Device->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST, 
			0, 
			4, 
			2, 
			this->quadPlaneIndex, 
			D3DFMT_INDEX16, 
			this->quadVertex, 
			sizeof( QUAD_VERTEX ) );


		cXMesh_Static::sStaticMeshEffect->EndPass();
	}
	cXMesh_Static::sStaticMeshEffect->End();
}