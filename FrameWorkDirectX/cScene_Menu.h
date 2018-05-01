#pragma once
#include "cScene.h"
#define TEXTCOLOR1 0xff4F81BD //Menu_off
#define TEXTCOLOR2 0xffFFC000 //Menu_on

//이슬추가(171023_UI_옵션창) #define 3개
#define TEXTLINE_X 50	//UI_옵션창 텍스트 시작위치(X)
#define TEXTLINE_Y 50	//UI_옵션창 텍스트 시작위치(Y)
#define TEXTLINE_Y2 90	//UI_옵션창 텍스트 시작위치(Y)

class cXMesh_Skinned;
class cXMesh_Static;
class cSkinnedAnimation;
class cBoundBox;
class cLight;
class cBaseObject;
class cCamera;
class cTerrain;
class cSkinnedAnimation;
class cQuad;
class cGizmoManager;


//이슬추가(171023_UI_옵션창) enum 3개
// 메뉴 < 세팅 < 오디오
//			    키보드/마우스 세팅 < 키보드 세팅
enum ACTIVEMENU //마우스가 메뉴 위에 있을 때 (클릭X)
{
	ACTIVEMENU_NONE,
	ACTIVEMENU_MENU_RESTART,
	ACTIVEMENU_MENU_NEWGAME,
	ACTIVEMENU_MENU_FINDGAME,
	ACTIVEMENU_MENU_CHARCHOICE,
	ACTIVEMENU_MENU_SET,
	ACTIVEMENU_MENU_EXIT,

	ACTIVEMENU_MENU_SET_AUDIO,
	ACTIVEMENU_MENU_SET_KEYMOU,
	ACTIVEMENU_MENU_SET_PREMENU,

	ACTIVEMENU_MENU_SET_AUDIO_BGM,
	ACTIVEMENU_MENU_SET_AUDIO_EFFECT,
	ACTIVEMENU_MENU_SET_AUDIO_PREMENU,

	ACTIVEMENU_MENU_SET_KEYMOU_MOUSE,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYRESET,
	ACTIVEMENU_MENU_SET_KEYMOU_PREMENU,

	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_JUMP,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_RUN,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SEAT,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_FIRE,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREGUN,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_POSTGUN,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_LOADGUN,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_NEARATTACK,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_GRENADE,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT1,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT2,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT3,
	ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREMENU
};
enum EXECUTEMENU //마우스가 메뉴를 클릭 했을 때 (클릭O)
{
	EXECUTEMENU_NONE,
	EXECUTEMENU_MENU,

	EXECUTEMENU_MENU_RESTART,
	EXECUTEMENU_MENU_NEWGAME,
	EXECUTEMENU_MENU_FINDGAME,
	EXECUTEMENU_MENU_CHARCHOICE,
	EXECUTEMENU_MENU_SET,
	EXECUTEMENU_MENU_EXIT,

	EXECUTEMENU_MENU_SET_AUDIO,
	EXECUTEMENU_MENU_SET_KEYMOU,
	EXECUTEMENU_MENU_SET_PREMENU,

	EXECUTEMENU_MENU_SET_AUDIO_BGM,
	EXECUTEMENU_MENU_SET_AUDIO_EFFECT,
	EXECUTEMENU_MENU_SET_AUDIO_PREMENU,

	EXECUTEMENU_MENU_SET_KEYMOU_MOUSE,
	EXECUTEMENU_MENU_SET_KEYMOU_KEYSET,
	EXECUTEMENU_MENU_SET_KEYMOU_KEYRESET,
	EXECUTEMENU_MENU_SET_KEYMOU_PREMENU,
};
enum MOTION  //단축키 모션
{
	MOTION_NONE,
	MOTION_JUMP,
	MOTION_RUN,
	MOTION_SITDOWN,
	MOTION_FIRE,
	MOTION_PREVWEAPON,
	MOTION_NEXTWEAPON,
	MOTION_RELOAD,
	MOTION_NEARATTACK,
	MOTION_DRAWBOOM,
	MOTION_WEAPON1st,
	MOTION_WEAPON2st,
	MOTION_WEAPON3st
};


class cScene_Menu : public cScene
{
private:
	cBaseObject*			pBaseObj;
	cPartcleEmitter*	weatherEffect;

	cTerrain*		m_pTerrain;
	cTransform*		pTransform;
	cGizmoManager*  pGiz;

	string					fileName;
	string					fileName2;
	string					fileName3;
	D3DXVECTOR3 worldPos;

private: //이슬추가(171023_UI_옵션창)
	ACTIVEMENU activeMenu;	//마우스가 메뉴 위에 있을 때 (클릭X)
	EXECUTEMENU executeMenu;//마우스가 메뉴를 클릭 했을 때 (클릭O)
	MOTION _motion;			//단축키 모션

	string key[14];

	RECT rcMain;		//가장 큰렉트
	RECT rcClient[14];	//메뉴 소렉트
	RECT rcSound_1;		//사운드 조절바 (긴 것)
	RECT rcSound_2;		//사운드 조절바 (이동바)
	RECT rcBGM_1;		//BGM 조절바 (긴 것)
	RECT rcBGM_2;		//BGM 조절바 (이동바)
	RECT rcMou_1;		//마우스감도 조절바 (긴 것)
	RECT rcMou_2;		//마우스감도 조절바 (이동바)



public:
	cScene_Menu();
	~cScene_Menu();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render1();
	virtual void Scene_RenderSprite();


	//이슬추가(171023_UI_옵션창)
	virtual void helpMenu(); //옵션창 선택
	virtual void SmartKey(); //단축키 설정
	void SetupSounds();
	void EffectRange(cPartcleEmitter * className, int MinEmissionRange_x, int MaxEmissionRange_x, int MinEmissionRange_y, int MaxEmissionRange_y, int MinEmissionRange_z, int MaxEmissionRange_z);
};

