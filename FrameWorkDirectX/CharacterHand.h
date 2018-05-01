#pragma once
#include "cBaseObject.h"

class CharacterHand :public cBaseObject
{
private:

public:
	CharacterHand();
	~CharacterHand();

	void Init(string filename, float scaling);
};

