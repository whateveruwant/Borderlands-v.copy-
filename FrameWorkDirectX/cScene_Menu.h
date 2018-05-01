#pragma once
#include "cScene.h"
#define TEXTCOLOR1 0xff4F81BD //Menu_off
#define TEXTCOLOR2 0xffFFC000 //Menu_on

//�̽��߰�(171023_UI_�ɼ�â) #define 3��
#define TEXTLINE_X 50	//UI_�ɼ�â �ؽ�Ʈ ������ġ(X)
#define TEXTLINE_Y 50	//UI_�ɼ�â �ؽ�Ʈ ������ġ(Y)
#define TEXTLINE_Y2 90	//UI_�ɼ�â �ؽ�Ʈ ������ġ(Y)

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


//�̽��߰�(171023_UI_�ɼ�â) enum 3��
// �޴� < ���� < �����
//			    Ű����/���콺 ���� < Ű���� ����
enum ACTIVEMENU //���콺�� �޴� ���� ���� �� (Ŭ��X)
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
enum EXECUTEMENU //���콺�� �޴��� Ŭ�� ���� �� (Ŭ��O)
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
enum MOTION  //����Ű ���
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

private: //�̽��߰�(171023_UI_�ɼ�â)
	ACTIVEMENU activeMenu;	//���콺�� �޴� ���� ���� �� (Ŭ��X)
	EXECUTEMENU executeMenu;//���콺�� �޴��� Ŭ�� ���� �� (Ŭ��O)
	MOTION _motion;			//����Ű ���

	string key[14];

	RECT rcMain;		//���� ū��Ʈ
	RECT rcClient[14];	//�޴� �ҷ�Ʈ
	RECT rcSound_1;		//���� ������ (�� ��)
	RECT rcSound_2;		//���� ������ (�̵���)
	RECT rcBGM_1;		//BGM ������ (�� ��)
	RECT rcBGM_2;		//BGM ������ (�̵���)
	RECT rcMou_1;		//���콺���� ������ (�� ��)
	RECT rcMou_2;		//���콺���� ������ (�̵���)



public:
	cScene_Menu();
	~cScene_Menu();

	virtual HRESULT Scene_Init();
	virtual void Scene_Release();
	virtual void Scene_Update(float timeDelta);
	virtual void Scene_Render1();
	virtual void Scene_RenderSprite();


	//�̽��߰�(171023_UI_�ɼ�â)
	virtual void helpMenu(); //�ɼ�â ����
	virtual void SmartKey(); //����Ű ����
	void SetupSounds();
	void EffectRange(cPartcleEmitter * className, int MinEmissionRange_x, int MaxEmissionRange_x, int MinEmissionRange_y, int MaxEmissionRange_y, int MinEmissionRange_z, int MaxEmissionRange_z);
};

