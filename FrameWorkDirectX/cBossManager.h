#pragma once
#include "cBaseObject.h"
#include "cTerrain.h"
#include "Character.h"
#include "cAstar.h"
enum eBossBodyState
{
	eBossBodyState_spawn,		//���� ����
	eBossBodyState_Idle,		//���
	eBossBodyState_Run,			//�������� ������ ���°�
	eBossBodyState_Attack,		//���� ����
	eBossBodyState_Hitted,		//�÷��̾� ���� ����
	eBossBodyState_Angry,		//���� �г����
	eBossBodyState_Die			//����
};
class cBossManager : public cBaseObject
{
protected:
	////////////////////////////////////////////// << ��ȣ������

	cMap*					pMap;
	Character*				pPlayer;

	//////////////////////////////////////////////

	cBoundSphere*			attackRangeSphere;	//���� ���ݹ��� �ڽ�

	//////////////////////////////////////////////

	string				mFilePath;

	int	HP;										//ü��
	int MaxHP;									//ü��
	float speed;								//���� �ӵ� (��/�� ��ġ�� ���� �ӵ���ȭ���� �߰�)
	int monsterDmg;								//���� ������
	int monsterEXP;								//����ġ
	bool isLive;								//����ִ��� ����
	bool headshotCheck;							//��弦�� �¾Ҵ���?

public:
	int isAttackted;							//���� ����

	cBossManager();
	~cBossManager();

	//////////////////////////////////////////////////////////////////		<< ��ȣ�����ϱ����� Set
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

	///////////////////////////////////////////////////////////////////		<< �������� �ʿ��� �Լ�
	void SeePlayer();
	void moveDirPlayer(float timeDelta);

};