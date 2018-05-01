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
	vector<cParticle*>			m_pPaticles;				//��ƼŬ �迭
	vector<LPPARTICLE_VERTEX>	m_ParticleVerticles;		//��ƼŬ ���ؽ�
	D3DXVECTOR3 terrainpos;

	VEC_COLOR			m_Colors;					//��ƼŬ �÷� �迭
	VEC_SCALE			m_Scales;					//��ƼŬ ������ �迭

	float speed;

	LPDIRECT3DTEXTURE9	m_pTex;

public:
	cParticleBullet();
	~cParticleBullet();

	void Init(
		float _speed,
		const VEC_COLOR& colors,					//��ƼŬ �÷� ���͹迭
		const VEC_SCALE& scales,					//��ƼŬ ������ �÷��迭
		LPDIRECT3DTEXTURE9 pPaticleTexture			//��ƼŬ Texture
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
	//�ؽ��� �ٲٴ� �Լ� �ۼ�
};