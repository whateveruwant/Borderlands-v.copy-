#pragma once
typedef struct {
	char JUMP;
	char RUN;
	char SITDOWN;
	char FIRE;
	char PREVWEAPON;
	char NEXTWEAPON;
	char RELOAD;
	char ATTACK;		//근접공격
	char DRAWBOOM;
	char WEAPON1st;
	char WEAPON2st;
	char WEAPON3st;

	char UP;
	char DOWN;
	char LEFT;
	char RIGHT;
	char TAKE;
	char ZOOM;
} gameNaming;
union sGameKeySetting {
	char arr[18];
	gameNaming name;
};

struct sGameElseSetting {
	float effect;
	float background;
	float mouseSpeed;
};
struct sPlayerItems {
	int*	hp;
	int*	maxhp;
	int*	shield;
	int*	maxshield;
	int*	shotgunBullet;
	int*	rifleBullet;
	int*	sniperBullet;
	int*	money;
};