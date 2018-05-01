#include "stdafx.h"
#include "cEffectMGR.h"
#include "cCamera.h"
#include "cTransform.h"


cEffectMGR::cEffectMGR()
{
}


cEffectMGR::~cEffectMGR()
{
}

void cEffectMGR::Init()
{
}

void cEffectMGR::Release()
{
	mParticleEmitters.clear();
	mQuadParticleEmitters.clear();
	mParticleMuzzleEmitters.clear();
}

void cEffectMGR::Update(float timeDelta)
{
	for (map<string, sUseParticleEmitter>::iterator i = mParticleEmitters.begin(); i != mParticleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mParticleEmitter->Update(timeDelta);
			i->second.activeTime -= timeDelta;
			if (i->second.activeTime <= 0) {
				i->second.activeTime = 0;
				i->second.isActive = false;
				i->second.mParticleEmitter->StopEmission();
			}
		}
	}
	for (map<string, sUseQuadParticleEmitter>::iterator i = mQuadParticleEmitters.begin(); i != mQuadParticleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mQuadParticleEmitter->Update(timeDelta);
			i->second.activeTime -= timeDelta;
			if (i->second.activeTime <= 0) {
				i->second.activeTime = 0;
				i->second.isActive = false;
				i->second.mQuadParticleEmitter->StopEmission();
			}
		}
	}
	for (map<string, sUseParticleMuzzleEmitter>::iterator i = mParticleMuzzleEmitters.begin(); i != mParticleMuzzleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mParticleMuzzleEmitter->Update(timeDelta);
			i->second.activeTime -= timeDelta;
			if (i->second.activeTime <= 0) {
				i->second.activeTime = 0;
				i->second.isActive = false;
				i->second.mParticleMuzzleEmitter->StopEmission();
			}
		}
	}

	for (int i =0 ; i < vDamages.size();) {
		vDamages[i].mTime -= timeDelta;
		float tmp = vDamages[i].mColor.a * 255 - (float)100 * timeDelta;
		vDamages[i].mColor.a = tmp / 255;
		vDamages[i].mPosition.x += timeDelta;
		vDamages[i].slate -= timeDelta * 0.5f;
		vDamages[i].mPosition.y += vDamages[i].slate;
		if (vDamages[i].mTime <= 0 || vDamages[i].mPosition.y < 0) {
			vDamages.erase(vDamages.begin() + i);
		}
		else ++i;
	}
}

void cEffectMGR::Render()
{
	for (map<string, sUseParticleEmitter>::iterator i = mParticleEmitters.begin(); i != mParticleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mParticleEmitter->Render();
		}
	}
	for (map<string, sUseQuadParticleEmitter>::iterator i = mQuadParticleEmitters.begin(); i != mQuadParticleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mQuadParticleEmitter->Render();
		}
	}
	for (map<string, sUseParticleMuzzleEmitter>::iterator i = mParticleMuzzleEmitters.begin(); i != mParticleMuzzleEmitters.end(); i++) {
		if (i->second.isActive) {
			i->second.mParticleMuzzleEmitter->Render();
		}
	}
}

void cEffectMGR::RenderSprite()
{
}

void cEffectMGR::RenderFont()
{
	for (int i = 0; i < vDamages.size(); i++) {
		D3DXVECTOR2	screenPos;
		pCamera->GetWorldPosToScreenPos(&screenPos, &vDamages[i].mPosition);
		DXFONT_MGR->PrintStyleTextShadow("damage", vDamages[i].mDamage, screenPos.x, screenPos.y, vDamages[i].mColor);
	}
}

void cEffectMGR::AddDamageFont(sDamageText font)
{
	vDamages.push_back(font);
}

void cEffectMGR::AddParticleMuzzleEmitter(string key, cParticleMuzzlingEitter * pParticle)
{
	mParticleMuzzleEmitters[key].mParticleMuzzleEmitter = pParticle;
	mParticleMuzzleEmitters[key].isActive = false;
	mParticleMuzzleEmitters[key].activeTime = 0.0f;
	this->pCamera->AddChild(mParticleMuzzleEmitters[key].mParticleMuzzleEmitter->pTransform);
}

void cEffectMGR::AddQuadParticleEmitter(string key, cQuadParticleEmitter * pParticle)
{
	mQuadParticleEmitters[key].mQuadParticleEmitter = pParticle;
	mQuadParticleEmitters[key].isActive = false;
	mQuadParticleEmitters[key].activeTime = 0.0f;
	this->pCamera->AddChild(mQuadParticleEmitters[key].mQuadParticleEmitter->pTransform);
}

void cEffectMGR::AddParticleEmitter(string key, cPartcleEmitter * pParticle)
{
	mParticleEmitters[key].mParticleEmitter = pParticle;
	mParticleEmitters[key].isActive = false;
	mParticleEmitters[key].activeTime = 0.0f;
	this->pCamera->AddChild(mParticleEmitters[key].mParticleEmitter->pTransform);
}

void cEffectMGR::StartParticleMuzzleEmitter(string key, float time, cTransform * pTrans)
{
	mParticleMuzzleEmitters[key].mParticleMuzzleEmitter->StartEmission();
	mParticleMuzzleEmitters[key].isActive = true;
	mParticleMuzzleEmitters[key].activeTime = time;
	mParticleMuzzleEmitters[key].mParticleMuzzleEmitter->pTransform = pTrans;
}

void cEffectMGR::StartQuadParticleEmitter(string key, float time, cTransform* pTrans)
{
	mQuadParticleEmitters[key].mQuadParticleEmitter->StartOneParticle();
	mQuadParticleEmitters[key].isActive = true;
	mQuadParticleEmitters[key].activeTime = time;
	mQuadParticleEmitters[key].mQuadParticleEmitter->pTransform = pTrans;
}

void cEffectMGR::StartParticleEmitter(string key, float time, cTransform* pTrans)
{
	mParticleEmitters[key].mParticleEmitter->StartEmission();
	mParticleEmitters[key].isActive = true;
	mParticleEmitters[key].activeTime = time;
	mParticleEmitters[key].mParticleEmitter->pTransform = pTrans;
}
