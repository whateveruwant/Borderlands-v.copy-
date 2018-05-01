#include "stdafx.h"
#include "cEnemy.h"
#include "cTransform.h"
#include "cBoundBox.h"
#include "cBoundSphere.h"
#include "cSkinnedAnimation.h"


cEnemy::cEnemy()
{
}


cEnemy::~cEnemy()
{
}


void cEnemy::Init(D3DXVECTOR2 xz)
{
	//초기화
	dropItem = false;
	respawntime = 0;
	respawnmaxtime = ENEMY_RESPAWNTIME;
	float tmpY = pMapInfo->GetHeight(xz.x, xz.y);
	this->pTransform->SetWorldPosition(xz.x, tmpY, xz.y);
	saveX = StartX = saveZ = StartZ = 0;
	isOneAttack = false;//한번떄리는지
	activeEnd = false;//행동 끝날때
	activeEndPosY = -0.15f;//y값으로 떨어지는 속도+델타값
	isFontSee = false;//폰트 폴수없음
	isLive = true;
	headshotCheck = false;
	compulsion = false;

	monsterHpText = RESOURCE_TEXTURE->GetResource("../Resources/ui/monsterHP.png");
	monsterHPendText = RESOURCE_TEXTURE->GetResource("../Resources/ui/monsterHPend.png");
}

void cEnemy::Update(float timeDelta)
{
	cBaseObject::Update(timeDelta);
	PHYSICS_MGR->IsBlocking(pPlayerTransform, pPlayerBoundBox, pTransform, monsterbox, 0.1f);
	isFontSee = false;//폰트 false해줘야함	
	respawntime += timeDelta;
}

void cEnemy::Release()
{
}

void cEnemy::Render()
{
	cBaseObject::Render();
	for (int i = 0; i < v_walk.size(); i++) {
		if (!isLive) continue;
		astar->box->RenderGizmo(v_walk[i].trans);
	}
}

void cEnemy::RenderSprite()
{
}

void cEnemy::RenderFont()
{
}

void cEnemy::SeePlayer()
{
	D3DXVECTOR3 tmp = { pPlayerTransform->GetWorldPosition().x, this->pTransform->GetWorldPosition().y,  pPlayerTransform->GetWorldPosition().z };
	pTransform->LookPosition(tmp, 5.0f*ONE_RAD);
}

void cEnemy::moveDirPlayer(float timeDelta)
{
	if (CheckDirect()) {
		////거리를 구한다.
		//LOG_MGR->AddLog("야호 바로앞에있네");
		pMapInfo->GetIndex(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z, &StartX, &StartZ);
		D3DXVECTOR3 dirToTarget = pPlayer->GetpPlayerXZTransform()->GetWorldPosition() - this->pTransform->GetWorldPosition();
		float dist = D3DXVec3Length(&dirToTarget);
		//이동량
		float deltaMove = mRunSpeed * timeDelta;
		float t = Clamp01(deltaMove / dist);
		D3DXVECTOR3 pos = VecLerp(this->pTransform->GetWorldPosition(), pPlayer->GetpPlayerXZTransform()->GetWorldPosition(), t);
		//높이 얻는다.
		pos.y = this->_cMap->GetpTerrain()->GetHeight(pos.x, pos.z);
		this->pTransform->SetWorldPosition(pos);
		//중력처리
		D3DXVECTOR3 sliding;
		_cMap->GetpTerrain()->GetSlant(&sliding, 9.8f, pos.x, pos.z);
		this->pTransform->MovePositionWorld(sliding * timeDelta);
		return;
	}
	//LOG_MGR->AddLog("에이 돌아가야지~");
	

	pMapInfo->GetIndex(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z, &StartX, &StartZ);
	_cMap->SetAstarTile(StartX, StartZ, false);
	_cMap->SetAstarTile(StartX - 1, StartZ - 1, false);
	_cMap->SetAstarTile(StartX - 1, StartZ, false);
	_cMap->SetAstarTile(StartX - 1, StartZ + 1, false);
	_cMap->SetAstarTile(StartX, StartZ - 1, false);
	_cMap->SetAstarTile(StartX, StartZ + 1, false);
	_cMap->SetAstarTile(StartX + 1, StartZ - 1, false);
	_cMap->SetAstarTile(StartX + 1, StartZ, false);
	_cMap->SetAstarTile(StartX + 1, StartZ + 1, false);

	if (v_walk.size() == 0 || saveX != *endX || saveZ != *endZ)
	{
		saveX = *endX;
		saveZ = *endZ;
		v_walk.clear();
		astar->setastar(StartX, StartZ, *endX, *endZ);
		v_walk = astar->getwalk();
	}

	if (v_walk.size()> 0)
	{
		D3DXVECTOR3 POS = _cMap->GetpAstarTile()[v_walk.back().z*MAXTILENUM + v_walk.back().x].centerPos;
		pTransform->LookPosition(POS, 5 * ONE_RAD);

		POS = pTransform->GetForward();
		POS *= mRunSpeed * timeDelta;

		pTransform->MovePositionWorld(POS);
		{
			pTransform->CorrectionUp(_cMap->GetpAstarTile()[v_walk.back().z*MAXTILENUM + v_walk.back().x].centerNormal, 2 * ONE_RAD);
		}
		//pTransform->SetAxisUp(_cMap->GetpAstarTile()[v_walk.back().z*MAXTILENUM + v_walk.back().x].centerNormal);



		if (v_walk.back().x == StartX || v_walk.back().z == StartZ)
		{
			v_walk.pop_back();
		}

		if (v_walk.size()>0 && _cMap->GetpAstarTile()[v_walk.back().z*MAXTILENUM + v_walk.back().x].canWalk)
		{
			v_walk.clear();
		}
	}
	pTransform->SetWorldPosition(pTransform->GetWorldPosition().x, this->pMapInfo->GetHeight(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z), pTransform->GetWorldPosition().z);

	pMapInfo->GetIndex(pTransform->GetWorldPosition().x, pTransform->GetWorldPosition().z, &StartX, &StartZ);

	_cMap->SetAstarTile(StartX, StartZ, true);
	_cMap->SetAstarTile(StartX - 1, StartZ - 1, true);
	_cMap->SetAstarTile(StartX - 1, StartZ, true);
	_cMap->SetAstarTile(StartX - 1, StartZ + 1, true);
	_cMap->SetAstarTile(StartX, StartZ - 1, true);
	_cMap->SetAstarTile(StartX, StartZ + 1, true);
	_cMap->SetAstarTile(StartX + 1, StartZ - 1, true);
	_cMap->SetAstarTile(StartX + 1, StartZ, true);
	_cMap->SetAstarTile(StartX + 1, StartZ + 1, true);

	////거리를 구한다.

	//D3DXVECTOR3 dirToTarget = pPlayerTransform->GetWorldPosition() - this->pTransform->GetWorldPosition();
	//float dist = D3DXVec3Length(&dirToTarget);
	//
	////이동량
	//float deltaMove = mRunSpeed * timeDelta;
	//float t = Clamp01(deltaMove / dist);
	//
	//D3DXVECTOR3 pos = VecLerp(this->pTransform->GetWorldPosition(), pPlayerTransform->GetWorldPosition(), t);
	//
	////높이 얻는다.
	//pos.y = this->pMapInfo->GetHeight(pos.x, pos.z);
	//this->pTransform->SetWorldPosition(pos);
	//
	////중력처리
	//D3DXVECTOR3 sliding;
	//pMapInfo->GetSlant(&sliding, 9.8f, pos.x, pos.z);
	//
	//this->pTransform->MovePositionWorld(sliding * timeDelta);
}

bool cEnemy::CheckAppreciate()
{
	if (!PHYSICS_MGR->IsOverlap(this->pPlayerTransform, this->pPlayerSphere, this->pTransform, this->appreciateRangeSphere)) {
		return false;
	}

	D3DXVECTOR3 enemyPos = pTransform->GetWorldPosition() + D3DXVECTOR3(0, monsterHeight, 0);
	D3DXVECTOR3 playerPos;
	playerPos = pPlayerTransform->GetWorldPosition() + D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 dir;

	//적->플레이어 검사
	dir = playerPos - enemyPos;
	//터레인 충돌검사
	D3DXVECTOR3 pHits = { 0,0,0 };
	Ray tmpRay;
	tmpRay.origin = enemyPos;
	D3DXVec3Normalize(&tmpRay.direction, &dir);
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		D3DXVECTOR3 terrainAndEnemy = pHits - enemyPos;
		if (D3DXVec3Length(&dir) >= D3DXVec3Length(&terrainAndEnemy)) {
			return false;
		}
	}

	//플레이어->적 검사
	enemyPos = pPlayerTransform->GetWorldPosition() + D3DXVECTOR3(0, 10, 0);
	playerPos = pTransform->GetWorldPosition() + D3DXVECTOR3(0, monsterHeight, 0);
	dir = playerPos - enemyPos;
	//터레인 충돌검사
	tmpRay.origin = enemyPos;
	D3DXVec3Normalize(&tmpRay.direction, &dir);
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		D3DXVECTOR3 terrainAndEnemy = pHits - enemyPos;
		if (D3DXVec3Length(&dir) >= D3DXVec3Length(&terrainAndEnemy)) {
			return false;
		}
	}

	return true;
}

bool cEnemy::CheckDirect()
{
	D3DXVECTOR3 enemyPos = pTransform->GetWorldPosition() + D3DXVECTOR3(0, monsterHeight, 0);
	D3DXVECTOR3 playerPos;
	playerPos = pPlayerTransform->GetWorldPosition() + D3DXVECTOR3(0, 0, 0);
	D3DXVECTOR3 dir;

	//적->플레이어 검사
	dir = playerPos - enemyPos;
	//터레인 충돌검사
	D3DXVECTOR3 pHits = { 0,0,0 };
	Ray tmpRay;
	tmpRay.origin = enemyPos;
	D3DXVec3Normalize(&tmpRay.direction, &dir);
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		D3DXVECTOR3 terrainAndEnemy = pHits - enemyPos;
		if (D3DXVec3Length(&dir) >= D3DXVec3Length(&terrainAndEnemy)) {
			return false;
		}
	}

	//플레이어->적 검사
	enemyPos = pPlayerTransform->GetWorldPosition() + D3DXVECTOR3(0, 10, 0);
	playerPos = pTransform->GetWorldPosition() + D3DXVECTOR3(0, monsterHeight, 0);
	dir = playerPos - enemyPos;
	//터레인 충돌검사
	tmpRay.origin = enemyPos;
	D3DXVec3Normalize(&tmpRay.direction, &dir);
	if (pMapInfo->IsIntersectRay(&pHits, &tmpRay)) {
		D3DXVECTOR3 terrainAndEnemy = pHits - enemyPos;
		if (D3DXVec3Length(&dir) >= D3DXVec3Length(&terrainAndEnemy)) {
			return false;
		}
	}

	return true;
}


void cEnemy::RespawnEnemy()
{
	if (mFilePath == "../Resources/Meshes/enemy/Skag/Skag(final).X")//skag
	{
		cEnemy::SeePlayer();
		ChangeState(eEnemyState_Idle);
		respawnmaxtime = ENEMY_RESPAWNTIME;
		mRunSpeed = SKAG_SPEED;			
		monsterHP = SKAG_HP;
		monsterMaxHP = monsterHP;
		monsterDmg = SKAG_DMG;
		checkOneAttack = false;		
		activeEnd = false;
		isOneAttack = false;
		activeEndPosY = -0.15f;
		isFontSee = false;
		isLive = true;
		dropItem = false;
		compulsion = false;
	}
	else if ("../Resources/Meshes/enemy/Bandit/Bandit.X")//bandit
	{
		cEnemy::SeePlayer();
		ChangeState(eEnemyState_Idle);
		respawnmaxtime = ENEMY_RESPAWNTIME;
		mRunSpeed = BANDIT_SPEED;			
		monsterHP = BANDIT_HP;
		monsterMaxHP = monsterHP;
		monsterDmg = BANDIT_DMG;
		checkOneAttack = false;		
		activeEnd = false;
		isOneAttack = false;	
		activeEndPosY = -0.15f;
		isFontSee = false;
		isLive = true;
		dropItem = false;
		compulsion = false;
	}
	else if ("../Resources/Meshes/enemy/BattleDroid/BattleDroid.X")//battledroid
	{
		ChangeState(eEnemyState_Idle);
		respawnmaxtime = ENEMY_RESPAWNTIME;
		mRunSpeed = BATTLEDROID_SPEED;		
		monsterHP = BATTLEDROID_HP;
		monsterMaxHP = monsterHP;
		monsterDmg = BATTLEDROID_DMG;
		checkOneAttack = false;		
		activeEnd = false;
		isOneAttack = false;	
		activeEndPosY = -0.15f;
		isFontSee = false;
		isLive = true;
		dropItem = false;
		compulsion = false;
	}
}