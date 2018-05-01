#pragma once

#include "cSingletonBase.h"

#define MAX_KEY_NUM      256

class cKeyMgr : public cSingletonBase<cKeyMgr>
{
private:
	bool         m_bDownCheck[MAX_KEY_NUM];
	bool         m_bUpCheck[MAX_KEY_NUM];

public:
	float _mouseSpeed;

private:
	cKeyMgr(void);

public:
	friend cKeyMgr* cSingletonBase<cKeyMgr>::GetInstance();
	~cKeyMgr(void);

	HRESULT Init(void);
	void Release(void);

	bool IsStayDown(int nVKey);
	bool IsOnceDown(int nVKey);
	bool IsOnceUp(int nVKey);

	bool wheelUP();
	bool wheelDOWN();
};

#define KEY_MGR cKeyMgr::GetInstance()