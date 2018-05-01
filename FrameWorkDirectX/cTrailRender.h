#pragma once

#include "cCamera.h"

class cTrailRender
{
private:


	//Trail Line 구조체
	typedef struct tagTrail{
		D3DXVECTOR3 p0;
		D3DXVECTOR3 p1;
		float x;				//해당 Trail 의 UV X
		float leftTime;
		bool bLive;

		tagTrail(){
			bLive = false;
		}

	}Trail, *LPTrail;

	//Trail Vertex
	typedef struct tagTrailVertex{
		D3DXVECTOR3 pos;
		DWORD		color;
		D3DXVECTOR2 uv;
		
		enum{ FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };

		tagTrailVertex(){

			color = 0xff00ffff;
		}
	}TrailVertex, *LPTrailVertex;

	//Trail Index 구조체 ( 이거 하나가 삼각형 하나다 )
	typedef struct tagTrailIndex{
		DWORD	i0;
		DWORD	i1;
		DWORD	i2;

		tagTrailIndex()
			: i0(0), i1(0), i2(0)
		{
		}

		tagTrailIndex( DWORD _0, DWORD _1, DWORD _2 )
			: i0(_0), i1(_1), i2(_2)
		{
		}

	}TrailIndex, *LPTrailIndex;

public:
	cTransform	Transform;		//Trail Transform  ( 예를 움직이면 라인이 생긴다. )

private:
	cTransform	m_LeftTransform;	//Trail Left Child Transform
	cTransform	m_RightTransform;	//Trail Right Child Transform

	D3DXVECTOR3	m_PrevLeftPosition;		//Left 이전 프레임 위치 ( 위치의 변화를 파악하기 위해 )
	D3DXVECTOR3 m_PrevRightPosition;	//Right 이전 프레임 위치 ( 위치의 변화를 파악하기 위해 )

	int				m_nTrailMaxNum;		//Trail Line 구조체 최대 수		
	LPTrail			m_pTrails;			//Trail Line 구조체 환영큐로 사용할꺼다
	LPTrailVertex	m_pTrailVertices;	//정점 구조체 배열
	LPTrailIndex	m_pTrailIndices;	//정점 인덱스 배열

	bool		b_LineGen;				//라인 생성 여부 

	int			m_nHeadIndex;			//헤드 위치 ( 생성위치 ) 
	int			m_nTailIndex;			//꼬리 위치

	// |  |  |  |  |  |  |  |  
    // ↑					  ↑
	// TailIndex			  HeadIndex ( 추가될 위치 실제로 그려지는 것은 Head 의 이전 부터 그려진다 )

	float		m_fLineTime;			//하나의 라인이 살아있는 시간
	
	LPDIRECT3DTEXTURE9		m_pTrailTexture;		//일반 Texture 
	LPDIRECT3DTEXTURE9		m_pDistortTexture;		//외곡에 사용되는 Texture

	//Distort Effect
	LPD3DXEFFECT			m_pDistortEffect;

	LPDIRECT3DTEXTURE9		m_pGrabTexture;

	D3DXCOLOR				m_TrailColor;			//선색

public:
	cTrailRender(void);
	~cTrailRender(void);

	void		Init( float trailLiveTime, float width, LPDIRECT3DTEXTURE9 pTrailTex, D3DXCOLOR color, LPDIRECT3DTEXTURE9 pDistortTex );
	void		Release();
	void		Update(float timeDelta);
	void		Render();
	void		RenderDistort( cCamera* pCam );


	void		TrailGenStart();
	void		TrailGenStop();
};

