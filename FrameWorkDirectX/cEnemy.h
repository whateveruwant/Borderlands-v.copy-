#pragma once
#include "cBaseObject.h"
#include "cTerrain.h"
#include "cAstar.h"
#include "Character.h"

#define ENEMYMAXNUM 100
#define ENEMY_RESPAWNTIME 100

//skag
#define SKAG_DMG RandomIntRange(10,15)
#define SKAG_SPEED 50.0f
#define SKAG_HP 100

//bandit
#define BANDIT_DMG RandomIntRange(10,15)
#define BANDIT_SPEED 30.0f
#define BANDIT_HP 100

//battledroid
#define BATTLEDROID_DMG RandomIntRange(15,20)
#define BATTLEDROID_SPEED 40.0f
#define BATTLEDROID_HP 200

enum eEnemyState
{
	eEnemyState_Idle,
	eEnemyState_Follow,
	eEnemyState_Run,
	eEnemyState_Attack,
	eEnemyState_Hitted,
	eEnemyState_Die,
};

struct sEnemySaveData {
	bool			isSet;
	char			sfilePath[100];
	D3DXVECTOR3		position;
};

class cEnemy : public cBaseObject
{
protected:
	//A*///////////////////////////////////////////////////////////////////////////////////////
	cAstar*					astar;
	cMap* _cMap;
	vector<walk>v_walk;
	int StartX, StartZ;
	int* endX;
	int* endZ;
	int saveX, saveZ;

	////맵정보//////////////////////////////////////////////////////////////////////////////////

	cTerrain*				pMapInfo;
	Character*				pPlayer;
	cTransform*				pPlayerTransform;
	cBoundBox*				pPlayerBoundBox;
	cBoundSphere*			pPlayerSphere;

	////////////////////////////////////////////////////////////////////////////////////////////

	cBoundSphere* appreciateRangeSphere;		//캐릭터 인식 박스
	cBoundSphere* attackRangeSphere;			//몬스터 공격범위 박스

												//에너미 ///////////////////////////////////////////////////////////////////////////////////

	LPDIRECT3DTEXTURE9		monsterHpText;		//몬스터hp텍스처
	LPDIRECT3DTEXTURE9		monsterHPendText;
	eEnemyState				savecurrentState;	//에너미 상태

	float mRunSpeed;							//뛰기 
	bool monsterHit;							//몬스터 공격범위 박스충돌 
	bool lookPlayer;							//플레이어 인식
	bool checkOneAttack;						//공격시 한번타격만 가능하게

												//거리재기 test용/////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 playerEnemyCalPos;
	float resultLength;
	float monsterHeight;

public:
	string					mFilePath;			//받아올 x파일
	eEnemyState				mCurrentState;		//에너미 상태

	bool compulsion;							//애니메이션 조정 
	bool isLive;								//살아있는지 유무
	bool isFontSee;								//폰트보기

	int	monsterHP;								//체력
	int monsterMaxHP;
	int isAttackted;							//공격 당함

	bool monsterLAttack;						//몬스터 왼손공격
	bool isOneAttack;							//한번 공격 했는지?

	bool activeEnd;
	float activeEndPosY;

	float respawnmaxtime;						//리스폰타임
	float respawntime;
	bool dropItem;								//아이템 드랍
	bool headshotCheck;							//헤드샷을 맞았느냐?

	int monsterDmg;								//몬스터 데미지
	int monsterEXP;								//경험치

												//에너미 박스 트랜스폼//////////////////////////////////////////////////////////////////////
	cTransform* boneHead;						//대가리 본 트랜스폼
	cBoundBox*	monsterbox;						//몬스터의 박스 = 몬스터 우선충돌 박스
	cBoundBox*	headbox;						//대가리 박스
	cBoundBox*	bodybox;						//몸통 박스

												//skag, Droid에만있는 본정보///////////////////////////////////////////////////////////////
	cTransform* LboneHand;				//왼손 정보
	cBoundBox* Lhandbox;				//왼손 박스
	cBoundBox* Llegbox;//droid 왼발
	cBoundBox* Rlegbox;//droid 오른발


public:
	cEnemy();
	~cEnemy();

	void SetendXZ(int* _x, int* _z)
	{
		endX = _x;
		endZ = _z;
	}
	///////////////////////////////////////////////////////////////////		<< 상호참조하기위해 Set
	void SetpPlayer(Character* pChar) {
		pPlayer = pChar;
		pPlayerTransform = pPlayer->GetpPlayerXZTransform();
		pPlayerBoundBox = pPlayer->GetpPlayerBoundBox();
		pPlayerSphere = pPlayer->GetpPlayerSphere();
	}
	void SetMap(cMap* __cMap) {
		astar = new cAstar();
		astar->SetMap(__cMap);
		_cMap = __cMap;
		pMapInfo = __cMap->GetpTerrain();
	}
	void SetTerrain(cTerrain* terrain) {
		pMapInfo = terrain;
	}

	//////////////////////////////////////////////////////////////////
	virtual void Init(D3DXVECTOR2 xz);
	virtual void Update(float timeDelta);
	virtual void Release();
	virtual void Render();
	virtual void RenderSprite();
	virtual void RenderFont();
	virtual void RespawnEnemy();//리스폰
	virtual void ChangeState(eEnemyState currentState) {}
	virtual void ActionByState(float timeDelta) {}
	
	///////////////////////////////////////////////////////////////////		<< 공통으로 필요한 함수
	void SeePlayer();
	void moveDirPlayer(float timeDelta);

	///////////////////////////////////////////////////////////////////		<< 기본 Get,Set
	cTransform* GetTransform() { return pTransform; }
	bool CheckAppreciate();
	bool CheckDirect();
};

