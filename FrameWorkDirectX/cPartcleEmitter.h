#pragma once

#include "cBaseObject.h"
#include "cParticle.h"


enum PATICLE_EMISSION_TYPE{
	PZERO,				//한점에서 방사
	SPHERE,				//구에서 방사
	SPHERE_OUTLINE,		//구에서 방사하는데 외부표면에서만....
	BOX					//박스에서 방사
};

//
// 파티클을 재생시켜주는 놈
//
class cPartcleEmitter : public cBaseObject
{
private:
	DWORD				m_PaticleNum;				//입자의 총량
	cParticle*			m_pPaticles;				//파티클 배열
	LPPARTICLE_VERTEX	m_ParticleVerticles;		//파티클 버텍스
	float				m_fEmissionPerSec;			//초당 발생량

	DWORD				m_dwParticleCount;			//활성화 순번
	
	VEC_COLOR			m_Colors;					//파티클 컬러 배열
	VEC_SCALE			m_Scales;					//파티클 사이즈 배열

	//파티클 라이브 타임 최대 최소
	float				m_fStartLiveTimeMin;
	float				m_fStartLiveTimeMax;
	
	//파티클 시작 속도 최대 최소
	D3DXVECTOR3			m_StartVelocityMin;			
	D3DXVECTOR3			m_StartVelocityMax;

	//파티클 시작 가속 최대 최소
	D3DXVECTOR3			m_StartAccelateMin;
	D3DXVECTOR3			m_StartAccelateMax;

	//파티클 시작 사이즈 최대 최소
	float				m_fStartScaleMin;
	float				m_fStartScaleMax;

	
	bool				m_bEmission;				//현재 생성중이니?
	float				m_fEmisionIntervalTime;		//생성 간격 시간
	float				m_fEmisionDeltaTime;		//하나 생성하구 지난시간

	//particle Texture 
	LPDIRECT3DTEXTURE9	m_pTex;

	bool				m_bLocal;					//로컬이니?


public:
	//파티클 생성타입
	PATICLE_EMISSION_TYPE	EmissionType;		//파티클 생성타입

	//파티클 생성범위
	float					MinEmissionRangeX;		//X 생성범위
	float					MaxEmissionRangeX;		
	float					MinEmissionRangeY;		//Y 생성범위
	float					MaxEmissionRangeY;
	float					MinEmissionRangeZ;		//Z 생성범위
	float					MaxEmissionRangeZ;
	float					SphereEmissionRange;		//구생성시 생성 반지름





public:
	cPartcleEmitter(void);
	~cPartcleEmitter(void);

	void Init( 
		DWORD partcleNum,					//총 파티클 량
		float emission,						//초당 발생량
		float liveTimeMin,					//파티클하나의 시간
		float liveTimeMax,
		const D3DXVECTOR3& velocityMin,		//파티클 시작 속도
		const D3DXVECTOR3& velocityMax,
		const D3DXVECTOR3& accelMin,		//파티클 가속
		const D3DXVECTOR3& accelMax,
		const VEC_COLOR& colors,			//파티클 컬러 벡터배열
		const VEC_SCALE& scales,			//파티클 스케일 컬러배열
		float scaleMin,						//파티클 스케일
		float scaleMax,
		LPDIRECT3DTEXTURE9 pPaticleTexture,	//파티클 Texture
		bool bLocal = false 				//이게 true 면 파티클의 움직임이 나의Transform Local 기준으로 움직인다.
		);
	
	void Release();

	virtual void BaseObjectUpdate( float timeDelta ) override;			//BaseObject 가 Update 때 실행....
	virtual void BaseObjectRender() override;

	void StartEmission();			//파티클 생성 시작
	void StopEmission();			//파티클 생성 중지

	//사방 팔방으로 입자 퍼트린다.
	void Burst( int num, float minSpeed, float maxSpeed, float maxLife, float minLife );

	void ParticleBurst(int num, float minX, float maxX, float minY, float maxY, float minZ, float maxZ,
		float minSpeed, float maxSpeed, float maxLife, float minLife);

	void SetRenderLocal( bool bLocal ){
		m_bLocal = bLocal;
	}

	void StartOneParticle();		//파티클 하나 생성
	void StartOneParticleMuzle();		//파티클 하나 생성
	//파티클 하나 생성 ( 방향 넣어서 )
	void StartOneParticle(D3DXVECTOR3 dir, float life);

};

