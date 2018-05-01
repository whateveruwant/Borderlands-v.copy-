#pragma once

#include <vector>
#include "cTransform.h"

//파티클 정점 구조체
typedef struct tagPARTICLE_VERTEX {
	D3DXVECTOR3 pos;			//정점 위치
	float size;					//파티클 사용시 정점 사이즈
	DWORD color;				//정점 컬러
	enum { FVF = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE };
}PARTICLE_VERTEX, *LPPARTICLE_VERTEX;


typedef std::vector<D3DXCOLOR>			VEC_COLOR;
typedef std::vector<float>				VEC_SCALE;

//
// 입자 하나에대한 정보
//
class cParticle
{
public:
	cTransform		m_Transform;		//파티클의 위치값
	D3DXVECTOR3		originPos;
	
private:
	bool			m_bLive;			//활성화 여부
	float			m_fTotalLiveTime;	//총 활성화 시간
	float			m_fDeltaLiveTime;	//활성화 지난 시간
	float			m_fNomalizeLiveTime;//활성화 비율 시간( 0 ~ 1 )


	D3DXVECTOR3		m_Velocity;			//파티클의 속도벡터
	D3DXVECTOR3		m_Accelation;		//초당 증가하는 가속벡터	
	float			m_fScale;			//기본 스케일값

public:
	bool			hitTerrain;
	D3DXVECTOR3		hitPos;

	cParticle(void);
	~cParticle(void);

	void Start(
		float liveTime,				//라이브 타임
		const D3DXVECTOR3* pos,		//시작 위치
		const D3DXVECTOR3* velo,	//시작 속도
		const D3DXVECTOR3* accel,	//가속 값
		float scale					//기본 스케일
	);

	void Update(float timeDelta);
	void bulletUpdate(float timeDelta);
	//자신의 정점 정보를 바탕으로 LPPARTICLE_VERTEX 의 값을 넣어준다.
	void GetParticleVertex(
		LPPARTICLE_VERTEX pOut,
		const VEC_COLOR& colors,
		const VEC_SCALE& scales);


	bool isLive() {
		return m_bLive;
	}

	cTransform GetTransform() {
		return m_Transform;
	}
	cTransform* GetpTransform() {
		return &m_Transform;
	}
	D3DXVECTOR3 GetOriginPos() {
		return originPos;
	}
	float GetDistance() {
		D3DXVECTOR3 dir = m_Transform.GetWorldPosition() - originPos;
		float distSq = D3DXVec3Length(&dir);
		return distSq;
	}
};

