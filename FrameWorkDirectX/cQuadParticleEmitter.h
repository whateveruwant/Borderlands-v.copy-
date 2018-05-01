#pragma once
#include "cbaseobject.h"
#include "cParticleQuad.h"

class cQuadParticleEmitter :public cBaseObject
{
private:

	DWORD					m_PaticleNum;				//입자의 총량
	cParticleQuad*			m_pPaticles;				//파티클 배열
	cParticleQuad**			m_pSortArray;				//파티클 소트용 Array

	LPPARTICLEQUAD_VERTEX	m_ParticleVerticles;		//파티클 버텍스
	DWORD*					m_ParticleIndex;			//파티클 인덱스

	float					m_fEmissionPerSec;			//초당 발생량
	DWORD					m_dwParticleCount;			//활성화 순번

	VEC_COLOR				m_Colors;					//파티클 컬러 배열
	VEC_SCALE				m_Scales;					//파티클 사이즈 배열

	cTransform*				m_pCameraTransform;			//카메라 트랜스폼
	bool					m_bCameraSort;				//카메라 거리별 소트할꺼니?....


	//파티클 라이브 타임 최대 최소
	float				m_fStartLiveTimeMin;
	float				m_fStartLiveTimeMax;

	//파티클 시작 속도 최대 최소
	D3DXVECTOR3			m_StartVelocityMin;			
	D3DXVECTOR3			m_StartVelocityMax;

	//파티클 시작 가속 최대 최소
	D3DXVECTOR3			m_StartAccelateMin;
	D3DXVECTOR3			m_StartAccelateMax;

	//파티클 시작할때 회전 시작값 최대 최소
	D3DXVECTOR3			m_StartSetRotateMin;
	D3DXVECTOR3			m_StartSetRotateMax;

	//파티클 시작 회전 최대 최소
	D3DXVECTOR3			m_StartRotateMin;
	D3DXVECTOR3			m_StartRotateMax;
	
	//파티클 회전 가속 최대 최소
	D3DXVECTOR3			m_StartRotateAccelateMin;
	D3DXVECTOR3			m_StartRotateAccelateMax;

	//파티클 시작 사이즈 최대 최소
	float				m_fStartScaleMin;
	float				m_fStartScaleMax;

	bool				m_bEmission;				//현재 생성중이니?
	float				m_fEmisionIntervalTime;		//생성 간격 시간
	float				m_fEmisionDeltaTime;		//하나 생성하구 지난시간

	//particle Texture 
	LPDIRECT3DTEXTURE9		m_pTex;

	bool				m_bLocal;



public:
	cQuadParticleEmitter(void);
	~cQuadParticleEmitter(void);


	HRESULT Init( 
		DWORD partcleNum,					//총 파티클 량
		float emission,						//초당 발생량
		float liveTimeMin,
		float liveTimeMax,
		const D3DXVECTOR3& velocityMin,
		const D3DXVECTOR3& velocityMax,
		const D3DXVECTOR3& accelMin,
		const D3DXVECTOR3& accelMax,
		const D3DXVECTOR3& startRotateMin,		//시작할때 회전 최소값 
		const D3DXVECTOR3& startRotateMax,		//시작할때 회전 최대값
		const D3DXVECTOR3& rotateMin,			//움직임에대한 회전 최소량
		const D3DXVECTOR3& rotateMax,			//움직임에대한 회전 최대량
		const D3DXVECTOR3& rotateAccelMin,		//움직임 가속에대한 회전 최소량
		const D3DXVECTOR3& rotateAccelMax,		//움직임 가속에대한 회전 최대량
		const VEC_COLOR& colors,
		const VEC_SCALE& scales, 
		float scaleMin,
		float scaleMax,
		LPDIRECT3DTEXTURE9 pTex,
		bool bLocal = false
		);
	
	void Release();

	virtual void BaseObjectUpdate( float timeDelta ) override;			//BaseObject 가 Update 때 실행....
	virtual void BaseObjectRender() override;


	void StartEmission();			//파티클 생성 시작
	void StopEmission();			//파티클 생성 중지

	//카메라 소팅을 할꺼니?
	void SetCameraSort( cTransform* pCameraTrans, bool bSort );

	
	void SetRenderLocal( bool bLocal ){
		m_bLocal = bLocal;
	}



	void StartOneParticle();		//파티클 하나 생성

private:

};

