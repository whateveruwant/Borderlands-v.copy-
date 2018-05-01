#pragma once
#include "cSingletonBase.h"
#include "cQuadParticleEmitter.h"
#include "cPartcleEmitter.h"
#include "cParticleMuzzleEmitter.h"
#include "cCamera.h"

struct sUseParticleEmitter {
	cPartcleEmitter*	mParticleEmitter;
	bool				isActive;
	float				activeTime;
};
struct sUseQuadParticleEmitter {
	cQuadParticleEmitter*	mQuadParticleEmitter;
	bool					isActive;
	float					activeTime;
};
struct sUseParticleMuzzleEmitter {
	cParticleMuzzlingEitter*	mParticleMuzzleEmitter;
	bool						isActive;
	float						activeTime;
};
struct sDamageText {
	string					mDamage;
	float					mTime;
	D3DXCOLOR				mColor;
	D3DXVECTOR3				mPosition;
	float					slate;
	float					start;
};

class cEffectMGR : public cSingletonBase<cEffectMGR>
{
private:
	map<string, sUseParticleEmitter>			mParticleEmitters;
	map<string, sUseQuadParticleEmitter>		mQuadParticleEmitters;
	map<string, sUseParticleMuzzleEmitter>		mParticleMuzzleEmitters;

	cCamera*									pCamera;

	vector<sDamageText>							vDamages;

public:
	cEffectMGR();
	~cEffectMGR();

	void Init();
	void Release();
	void Update(float timeDelta);
	void Render();
	void RenderSprite();
	void RenderFont();

	void SetCamera(cCamera* p) {
		pCamera = p;
	}

	void AddDamageFont(sDamageText font);
	void AddParticleMuzzleEmitter(string key, cParticleMuzzlingEitter* pParticle);
	void AddQuadParticleEmitter(string key, cQuadParticleEmitter* pParticle);
	void AddParticleEmitter(string key, cPartcleEmitter* pParticle);

	void StartParticleMuzzleEmitter(string key, float time, cTransform* pTrans);
	void StartQuadParticleEmitter(string key, float time, cTransform* pTrans);
	void StartParticleEmitter(string key, float time, cTransform* pTrans);
};
#define EFFECT_MGR cEffectMGR::GetInstance()