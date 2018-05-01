#pragma once
#include "cBaseObject.h"
#include "cTerrain.h"
#include "Character.h"
#include "cAstar.h"
enum eBossBodyState
{
	eBossBodyState_spawn,		//스폰 상태
	eBossBodyState_Idle,		//대기
	eBossBodyState_Run,			//수영으로 빠르게 오는것
	eBossBodyState_Attack,		//직접 공격
	eBossBodyState_Hitted,		//플레이어 한테 맞음
	eBossBodyState_Angry,		//보스 분노상태
	eBossBodyState_Die			//다이
};
class cBossManager : public cBaseObject
{
protected:
	////////////////////////////////////////////// << 상호참조용

	cMap*					pMap;
	Character*				pPlayer;

	//////////////////////////////////////////////

	cBoundSphere*			attackRangeSphere;	//몬스터 공격범위 박스

	//////////////////////////////////////////////

	string				mFilePath;

	int	HP;										//체력
	int MaxHP;									//체력
	float speed;								//보스 속도 (물/땅 위치에 따른 속도변화위해 추가)
	int monsterDmg;								//몬스터 데미지
	int monsterEXP;								//경험치
	bool isLive;								//살아있는지 유무
	bool headshotCheck;							//헤드샷을 맞았느냐?

public:
	int isAttackted;							//공격 당함

	cBossManager();
	~cBossManager();

	//////////////////////////////////////////////////////////////////		<< 상호참조하기위해 Set
	void SetpPlayer(Character* pChar) {
		pPlayer = pChar;
	}
	void SetpMap(cMap* pMap) {
		this->pMap = pMap;
	}

	//////////////////////////////////////////////////////////////////
	virtual void Init(D3DXVECTOR2 xz);
	virtual void LoadMesh() {}
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void RenderShadow();

	virtual void ActionByState(float timeDelta) {}

	///////////////////////////////////////////////////////////////////		<< 공통으로 필요한 함수
	void SeePlayer();
	void moveDirPlayer(float timeDelta);

};