#pragma once
#include "cTerrain.h"
#include "cPartcleEmitter.h"
#include "cQuad.h"
struct sAstarTile {
	// 02
	// 13
	D3DXVECTOR3		centerPos;
	D3DXVECTOR3		centerNormal;
	D3DXVECTOR3		slant;
	D3DXVECTOR3		normal[4];
	D3DXVECTOR3		pos[4];
	bool			canWalk;
	bool			savecanWalk;
	float			height;
};

class cMap
{
private:
	cTerrain*			mDungeon;
	cTerrain*			mTown;
	cTerrain*			mTerrain;

	cPartcleEmitter*	weatherEffect;
	sAstarTile*			mATile;

	/////////////////////////////////////////////////////// 물효과
	cQuad*				pWater;
	cTransform*			pWaterTrans;
	LPDIRECT3DTEXTURE9  pDistortNormal;
	/////////////////////////////////////////////////////// 물효과

	bool				isTown;

public:
	cMap();
	~cMap();

	void Init();
	void InitForAstarTile();
	void InitForWeather();
	void InitForWater();

	void Update(float timeDelta);

	void Release();
	void ReleaseForWater();

	void Render(cCamera* pCam, cLight_Direction* pDirectLight, cCamera* pDirectionLightCamera);
	void RenderForWater(cCamera* pCam);
	void RenderForAstarTile();

	//이펙트가 떨어지는 범위설정 함수
	void EffectRange(
		cPartcleEmitter* className,
		int MinEmissionRange_x,
		int MaxEmissionRange_x,
		int MinEmissionRange_y,
		int MaxEmissionRange_y,
		int MinEmissionRange_z,
		int MaxEmissionRange_z
	);

	//////////////////////////////////////////////////////////////////////////////////////////////			<< GET,SET Functions
	inline cTerrain* GetpTerrain() {
		return mTerrain;
	}
	inline sAstarTile* GetpAstarTile() {
		return mATile;
	}

	inline void SetAstarTile(int _x, int _z, bool is_ob)
	{
		if (!mATile[_z*((int)sqrt(mTerrain->GetTotalCellNum()) / 4) + _x].savecanWalk)
			mATile[_z*((int)sqrt(mTerrain->GetTotalCellNum()) / 4) + _x].canWalk = is_ob;
	}
	void ChageMap(bool _where) {
		if (_where) {
			isTown = true;
			mTerrain = mTown;
			InitForAstarTile();
		}
		else {
			isTown = false;
			mTerrain = mDungeon;
			InitForAstarTile();
		}
	}
	bool GetIsTown() {
		return isTown;
	}
};

