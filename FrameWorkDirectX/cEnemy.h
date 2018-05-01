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

	////������//////////////////////////////////////////////////////////////////////////////////

	cTerrain*				pMapInfo;
	Character*				pPlayer;
	cTransform*				pPlayerTransform;
	cBoundBox*				pPlayerBoundBox;
	cBoundSphere*			pPlayerSphere;

	////////////////////////////////////////////////////////////////////////////////////////////

	cBoundSphere* appreciateRangeSphere;		//ĳ���� �ν� �ڽ�
	cBoundSphere* attackRangeSphere;			//���� ���ݹ��� �ڽ�

												//���ʹ� ///////////////////////////////////////////////////////////////////////////////////

	LPDIRECT3DTEXTURE9		monsterHpText;		//����hp�ؽ�ó
	LPDIRECT3DTEXTURE9		monsterHPendText;
	eEnemyState				savecurrentState;	//���ʹ� ����

	float mRunSpeed;							//�ٱ� 
	bool monsterHit;							//���� ���ݹ��� �ڽ��浹 
	bool lookPlayer;							//�÷��̾� �ν�
	bool checkOneAttack;						//���ݽ� �ѹ�Ÿ�ݸ� �����ϰ�

												//�Ÿ���� test��/////////////////////////////////////////////////////////////////////////////
	D3DXVECTOR3 playerEnemyCalPos;
	float resultLength;
	float monsterHeight;

public:
	string					mFilePath;			//�޾ƿ� x����
	eEnemyState				mCurrentState;		//���ʹ� ����

	bool compulsion;							//�ִϸ��̼� ���� 
	bool isLive;								//����ִ��� ����
	bool isFontSee;								//��Ʈ����

	int	monsterHP;								//ü��
	int monsterMaxHP;
	int isAttackted;							//���� ����

	bool monsterLAttack;						//���� �޼հ���
	bool isOneAttack;							//�ѹ� ���� �ߴ���?

	bool activeEnd;
	float activeEndPosY;

	float respawnmaxtime;						//������Ÿ��
	float respawntime;
	bool dropItem;								//������ ���
	bool headshotCheck;							//��弦�� �¾Ҵ���?

	int monsterDmg;								//���� ������
	int monsterEXP;								//����ġ

												//���ʹ� �ڽ� Ʈ������//////////////////////////////////////////////////////////////////////
	cTransform* boneHead;						//�밡�� �� Ʈ������
	cBoundBox*	monsterbox;						//������ �ڽ� = ���� �켱�浹 �ڽ�
	cBoundBox*	headbox;						//�밡�� �ڽ�
	cBoundBox*	bodybox;						//���� �ڽ�

												//skag, Droid�����ִ� ������///////////////////////////////////////////////////////////////
	cTransform* LboneHand;				//�޼� ����
	cBoundBox* Lhandbox;				//�޼� �ڽ�
	cBoundBox* Llegbox;//droid �޹�
	cBoundBox* Rlegbox;//droid ������


public:
	cEnemy();
	~cEnemy();

	void SetendXZ(int* _x, int* _z)
	{
		endX = _x;
		endZ = _z;
	}
	///////////////////////////////////////////////////////////////////		<< ��ȣ�����ϱ����� Set
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
	virtual void RespawnEnemy();//������
	virtual void ChangeState(eEnemyState currentState) {}
	virtual void ActionByState(float timeDelta) {}
	
	///////////////////////////////////////////////////////////////////		<< �������� �ʿ��� �Լ�
	void SeePlayer();
	void moveDirPlayer(float timeDelta);

	///////////////////////////////////////////////////////////////////		<< �⺻ Get,Set
	cTransform* GetTransform() { return pTransform; }
	bool CheckAppreciate();
	bool CheckDirect();
};

