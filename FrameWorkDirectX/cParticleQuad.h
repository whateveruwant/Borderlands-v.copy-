#pragma once

#include <vector>
#include "cTransform.h"

//파티클 정점 구조체
typedef struct tagPARTICLEQUAD_VERTEX{
	D3DXVECTOR3 pos;			//정점 위치
	DWORD color;				//정점 컬러
	D3DXVECTOR2 uv;				//정점 UV
	enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };
}PARTICLEQUAD_VERTEX, *LPPARTICLEQUAD_VERTEX;

typedef std::vector<D3DXCOLOR>			VEC_COLOR;
typedef std::vector<float>				VEC_SCALE;

class cParticleQuad
{
public:
	cTransform		m_Transform;		//파티클의 위치값

private:
	bool			m_bLive;			//활성화 여부
	
	float			m_fTotalLiveTime;	//총 활성화 시간
	float			m_fDeltaLiveTime;	//활성화 지난 시간
	float			m_fNomalizeLiveTime;//활성화 비율 시간( 0 ~ 1 )

	
	D3DXVECTOR3		m_Velocity;			//파티클의 속도벡터
	D3DXVECTOR3		m_Accelation;		//초당 증가하는 가속벡터
	D3DXVECTOR3		m_Rotate;			//초당 회전 벡터
	D3DXVECTOR3		m_RotateAccel;		//초당 증가하는 회전벡터

	float			m_fScale;			//기본 스케일값

public:
	cParticleQuad(void);
	~cParticleQuad(void);

	void Start( 
		float liveTime,				//라이브 타임
		const D3DXVECTOR3* pos,		//시작 위치
		const D3DXVECTOR3* velo,		//시작 속도
		const D3DXVECTOR3* accel,	//가속 값
		const D3DXVECTOR3* rotate,	//초당 회전 값
		const D3DXVECTOR3* rotateAccel,	//초당 회전 증가값
		float scale					//기본 스케일
		);

	void Update( float timeDelta );	

	//자신의 정점 정보를 바탕으로 LPPARTICLEQUAD_VERTEX 의 값을 넣어준다.
	void GetParticleVertex( 
		LPPARTICLEQUAD_VERTEX pOut, 
		DWORD* pIndex,
		const VEC_COLOR& colors,
		const VEC_SCALE& scales,
		DWORD dwParticleNum );

	bool isLive(){
		return m_bLive;
	}

	
};

