#pragma once

#include"cMap.h"
#include"cTransform.h"
#include"cBoundBox.h"
struct aStarlist
{
	int x;
	int z;

	int F;
	int G;
	int H;

	aStarlist* parent;
};

struct walk
{
	int x, z;
	cTransform* trans;
};
#define MAXTILENUM 1024/4

class cAstar
{
public:
	cBoundBox* box;
private:
	sAstarTile tmp_tile[MAXTILENUM *MAXTILENUM];

	vector<aStarlist*> _openList;
	vector<aStarlist*> _closeList;

	vector<aStarlist*>_list;
	vector<walk>v_walk;

public:
	cMap* maptile;
	cAstar();
	~cAstar();
	void SetMap(cMap* map) { maptile = map; }

	void setastar(int _startX, int _startY, int _EndX, int _EndY);
	void astarWalk(aStarlist* _aStarlist);
	void SetWalkRay();

	void walkclear() { v_walk.clear(); }
	vector<walk>cAstar::getwalk() { return v_walk; }
};

