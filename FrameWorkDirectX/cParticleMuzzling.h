#pragma once

#include <vector>
#include "cTransform.h"

typedef std::vector<D3DXCOLOR>			VEC_COLOR;
typedef std::vector<float>				VEC_SCALE;

//
// ���� �ϳ������� ����
//
class cParticleMuzzling
{
public:
	cTransform*		m_Transform;		//��ƼŬ�� ��ġ��
	D3DXVECTOR3		originPos;

private:
	bool			m_bLive;			//Ȱ��ȭ ����
	float			m_fTotalLiveTime;	//�� Ȱ��ȭ �ð�
	float			m_fDeltaLiveTime;	//Ȱ��ȭ ���� �ð�
	float			m_fNomalizeLiveTime;//Ȱ��ȭ ���� �ð�( 0 ~ 1 )


	D3DXVECTOR3		m_Velocity;			//��ƼŬ�� �ӵ�����
	D3DXVECTOR3		m_Accelation;		//�ʴ� �����ϴ� ���Ӻ���	
	float			m_fScale;			//�⺻ �����ϰ�

public:
	cParticleMuzzling(void);
	~cParticleMuzzling(void);

	void Start(
		float liveTime,				//���̺� Ÿ��
		const D3DXVECTOR3* pos,		//���� ��ġ
		const D3DXVECTOR3* velo,	//���� �ӵ�
		const D3DXVECTOR3* accel,	//���� ��
		float scale,				//�⺻ ������
		cTransform*	trans
	);

	void Update(float timeDelta);
	void bulletUpdate(float timeDelta);
	//�ڽ��� ���� ������ �������� LPPARTICLE_VERTEX �� ���� �־��ش�.
	void GetParticleVertex(
		LPPARTICLE_VERTEX pOut,
		const VEC_COLOR& colors,
		const VEC_SCALE& scales);


	bool isLive() {
		return m_bLive;
	}


	cTransform* GetpTransform() {
		return m_Transform;
	}
	D3DXVECTOR3 GetOriginPos() {
		return originPos;
	}
	float GetDistance() {
		D3DXVECTOR3 dir = m_Transform->GetWorldPosition() - originPos;
		float distSq = D3DXVec3Length(&dir);
		return distSq;
	}
};

