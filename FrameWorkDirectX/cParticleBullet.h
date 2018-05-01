#pragma once
#include"cTransform.h"
#include "cParticle.h"
#include"cBoundBox.h"

class cParticleBullet
{
public:
	cTransform*					mTransform;
	float randAngle;
	float randAngleX;
	float randAngleY;


	cBoundBox*					m_box;
	float speed2;
	float range;

private:
	vector<cParticle*>			m_pPaticles;				//파티클 배열
	vector<LPPARTICLE_VERTEX>	m_ParticleVerticles;		//파티클 버텍스
	D3DXVECTOR3 terrainpos;

	VEC_COLOR			m_Colors;					//파티클 컬러 배열
	VEC_SCALE			m_Scales;					//파티클 사이즈 배열

	float speed;

	LPDIRECT3DTEXTURE9	m_pTex;

public:
	cParticleBullet();
	~cParticleBullet();

	void Init(
		float _speed,
		const VEC_COLOR& colors,					//파티클 컬러 벡터배열
		const VEC_SCALE& scales,					//파티클 스케일 컬러배열
		LPDIRECT3DTEXTURE9 pPaticleTexture			//파티클 Texture
	);
	void StartOneParticle();

	void Update(float timeDelta);
	void UpdateForPlayer(float timeDelta);
	void UpdateForEnemy(float timeDelta);
	void UpdateForBoss(float timeDelta);
	void Render();

	vector<cParticle*> GetPaticles() {
		return m_pPaticles;
	}
	void DeleteOneParticle(int index) {
		m_pPaticles.erase(m_pPaticles.begin() + index);
		m_ParticleVerticles.erase(m_ParticleVerticles.begin() + index);
	}
	cTransform* GetpTransform() {
		return mTransform;
	}
	//텍스쳐 바꾸는 함수 작성
};