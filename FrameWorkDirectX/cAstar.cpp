#include "stdafx.h"
#include "cAstar.h"


cAstar::cAstar()
{
	box = new cBoundBox();
	box->SetBound(&D3DXVECTOR3(0, 0, 0), &D3DXVECTOR3(2, 2, 2));
}


cAstar::~cAstar()
{
}

void cAstar::setastar(int _startX, int _startY, int _EndX, int _EndY)
{
	memcpy(&tmp_tile, maptile->GetpAstarTile(), sizeof(sAstarTile)*MAXTILENUM*MAXTILENUM);

	if (tmp_tile[_EndY*MAXTILENUM + _EndX].canWalk)return;
	tmp_tile[_EndY*MAXTILENUM+_EndX].canWalk = false;

	_closeList.clear();
	_openList.clear();
	aStarlist* __aStarlist = new aStarlist;
	__aStarlist->x = _startX;
	__aStarlist->z = _startY;
	__aStarlist->G = 0;//처음값이니까 g는 0
	__aStarlist->H = abs(_startX - _EndX) + abs(_startY - _EndY);
	__aStarlist->F = __aStarlist->G + __aStarlist->H;
	__aStarlist->parent = NULL;
	_closeList.push_back(__aStarlist);
	int lastindex = 0;
	while (1)
	{
		if (_closeList[lastindex]->x == _EndX&&_closeList[lastindex]->z == _EndY)
		{
			v_walk.clear();
			aStarlist* close = _closeList[lastindex];
			astarWalk(close);
			break;
		}
		if (_closeList[lastindex]->x != 0 && _closeList[lastindex]->z != 0)//왼쪽위
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].canWalk &&
				!tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].height&&
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				if (!tmp_tile[(_closeList[lastindex]->z - 1) * MAXTILENUM + _closeList[lastindex]->x - 1].canWalk ||
					tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
					tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x - 1].height)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[(_closeList[lastindex]->z - 1) * MAXTILENUM + _closeList[lastindex]->x - 1].canWalk = true;
					_aStarlist->x = _closeList[lastindex]->x - 1;
					_aStarlist->z = _closeList[lastindex]->z - 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}

		if (_closeList[lastindex]->x != 0 && _closeList[lastindex]->z != MAXTILENUM - 1)//왼쪽아래
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].canWalk &&
				!tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].height&&
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				if (!tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x - 1].canWalk ||
					tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
					tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x - 1].height)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[(_closeList[lastindex]->z + 1) * MAXTILENUM + _closeList[lastindex]->x - 1].canWalk = true;
					_aStarlist->x = _closeList[lastindex]->x - 1;
					_aStarlist->z = _closeList[lastindex]->z + 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}

		if (_closeList[lastindex]->x != MAXTILENUM - 1 && _closeList[lastindex]->z != MAXTILENUM - 1)//오른쪽아래
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].canWalk &&
				!tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].height&&
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				if (!tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x + 1].canWalk ||
					tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
					tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x + 1].height)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[(_closeList[lastindex]->z + 1) * MAXTILENUM + _closeList[lastindex]->x + 1].canWalk = true;
					_aStarlist->x = _closeList[lastindex]->x + 1;
					_aStarlist->z = _closeList[lastindex]->z + 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}
		if (_closeList[lastindex]->x != MAXTILENUM - 1 && _closeList[lastindex]->z != 0)//오른쪽위
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].canWalk &&
				!tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].height&&
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				if (!tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x + 1].canWalk ||
					tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
					tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x + 1].height)
				{
					aStarlist* _aStarlist = new aStarlist;
					tmp_tile[(_closeList[lastindex]->z - 1) * MAXTILENUM + _closeList[lastindex]->x + 1].canWalk = true;
					_aStarlist->x = _closeList[lastindex]->x + 1;
					_aStarlist->z = _closeList[lastindex]->z - 1;
					_aStarlist->G = _closeList[lastindex]->G + 10;
					_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
					_aStarlist->F = _aStarlist->G + _aStarlist->H;
					_aStarlist->parent = _closeList[lastindex];
					_openList.push_back(_aStarlist);
				}
			}
		}
		if (_closeList[lastindex]->x != 0)
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x - 1].height)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->z * MAXTILENUM + _closeList[lastindex]->x - 1].canWalk = true;
				_aStarlist->x = _closeList[lastindex]->x - 1;
				_aStarlist->z = _closeList[lastindex]->z;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->x != MAXTILENUM - 1)
		{
			if (!tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x + 1].height)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[_closeList[lastindex]->z * MAXTILENUM + _closeList[lastindex]->x + 1].canWalk = true;
				_aStarlist->x = _closeList[lastindex]->x + 1;
				_aStarlist->z = _closeList[lastindex]->z;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->z != 0)
		{
			if (!tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z - 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[(_closeList[lastindex]->z - 1) * MAXTILENUM + _closeList[lastindex]->x].canWalk = true;

				_aStarlist->x = _closeList[lastindex]->x;
				_aStarlist->z = _closeList[lastindex]->z - 1;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}
		if (_closeList[lastindex]->z != MAXTILENUM - 1)
		{
			if (!tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].canWalk ||
				tmp_tile[_closeList[lastindex]->z*MAXTILENUM + _closeList[lastindex]->x].height>
				tmp_tile[(_closeList[lastindex]->z + 1)*MAXTILENUM + _closeList[lastindex]->x].height)
			{
				aStarlist* _aStarlist = new aStarlist;
				tmp_tile[(_closeList[lastindex]->z + 1) * MAXTILENUM + _closeList[lastindex]->x].canWalk = true;

				_aStarlist->x = _closeList[lastindex]->x;
				_aStarlist->z = _closeList[lastindex]->z + 1;
				_aStarlist->G = _closeList[lastindex]->G + 10;
				_aStarlist->H = (abs(_aStarlist->x - _EndX) + abs(_aStarlist->z - _EndY)) * 10;
				_aStarlist->F = _aStarlist->G + _aStarlist->H;
				_aStarlist->parent = _closeList[lastindex];
				_openList.push_back(_aStarlist);
			}
		}

		if (_openList.size() == 0)//가는길이 다막혀있다면
		{
			v_walk.clear();
			break;
		}

		if (_openList.size() > 1000)
		{
			v_walk.clear();
			break;
		}

		int deletfloor = 0;//오픈리스트 중어떤것이 제일 작은건지 판단하고 그배열값을 넣어줄곳임
		int min = 500000;//제일큰 f값을 가지도록 해줌
		for (int i = 0; i < _openList.size(); i++)
		{
			if (_openList[i]->F < min)
			{
				min = _openList[i]->F;
				deletfloor = i;
			}
		}

		_closeList.push_back(_openList[deletfloor]);
		_openList.erase(_openList.begin() + deletfloor);

		lastindex++;//추가될때마다 마지막 을알기위해?
	}
	SetWalkRay();
}

void cAstar::astarWalk(aStarlist * _aStarlist)
{
	if (_aStarlist->parent == NULL)
		return;

	walk _walk;
	_walk.x = _aStarlist->x;
	_walk.z = _aStarlist->z;
	_walk.trans = new cTransform();
	_walk.trans->SetWorldPosition(*maptile->GetpAstarTile()[_walk.z*MAXTILENUM + _walk.x].pos);
	v_walk.push_back(_walk);

	astarWalk(_aStarlist->parent);
}

void cAstar::SetWalkRay()
{
	return;
	int count = 0;
	for (int i = 0; i < v_walk.size(); i++)
	{
		//for (int j= i+1;j< v_walk.size();)
		for (int j = v_walk.size() - 1; j> i;)
		{
			if (v_walk[i].trans->GetWorldPosition().y + 30 <= v_walk[j].trans->GetWorldPosition().y)
			{
				j--;
				continue;
			}
			v_walk[i].trans->LookPosition(v_walk[j].trans->GetWorldPosition());
			Ray ry;
			ry.direction = v_walk[i].trans->GetForward();
			ry.origin = v_walk[i].trans->GetWorldPosition();
			D3DXVECTOR3 po(0, 0, 0);
			bool tf = maptile->GetpTerrain()->IsIntersectRay(&po, &ry);

			D3DXVECTOR3 le = po - v_walk[i].trans->GetWorldPosition();
			float dd = D3DXVec3Length(&le);
			D3DXVECTOR3 le2 = v_walk[j].trans->GetWorldPosition() - v_walk[i].trans->GetWorldPosition();
			float dd2 = D3DXVec3Length(&le2);

			if (dd2<dd)
			{
				j--;
			}
			else
			{
				if (j - 1 == i)break;

				count = 0;
				for (int z = j - 1; z > i; z--)
				{
					v_walk.erase(v_walk.begin() + z);
					count++;
				}
				j -= count;

			}

		}
	}
}
