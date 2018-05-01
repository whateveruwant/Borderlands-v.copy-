#include "stdafx.h"
#include "cScene_Menu.h"
#include "cCamera.h"
#include "cXMesh_Skinned.h"
#include "cSkinnedAnimation.h"
#include "cLight.h"
#include "cLight_Direction.h"
#include "cLight_Point.h"
#include "cXMesh_Static.h"
#include "cBaseObject.h"
#include "cTransform.h"
#include "cCamera.h"
#include "cTerrain.h"
#include "cSkinnedAnimation.h"
#include "cQuad.h"
#include "cTrailRender.h"
#include "cGizmoManager.h"


cScene_Menu::cScene_Menu()
{
}


cScene_Menu::~cScene_Menu()
{
}

HRESULT cScene_Menu::Scene_Init()
{
	//�̽��߰�(171023_UI_�ɼ�â)            
	activeMenu = ACTIVEMENU_NONE;
	executeMenu = EXECUTEMENU_MENU;

	//volume = 0;

	//UI_�ɼ�â ���� ū��Ʈ
	rcMain = { 0,0,WINSIZE_X / 2, WINSIZE_Y };
	//�޴� �ҷ�Ʈ
	rcClient[14];
	for (int i = 1; i < 13; i++)
	{
		rcClient[1] = { TEXTLINE_X, TEXTLINE_Y2, TEXTLINE_X + 350, TEXTLINE_Y2 + 42 };
		rcClient[i + 1] = { TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y*(i), TEXTLINE_X + 350,  TEXTLINE_Y2 + TEXTLINE_Y*(i)+42 };
	}
	//������ ��Ʈ
	rcSound_1 = { TEXTLINE_X + 400, TEXTLINE_Y2 + 18,  TEXTLINE_X + 650 , TEXTLINE_Y2 + 22 };
	rcSound_2 = { TEXTLINE_X + 620, TEXTLINE_Y2 + 13,  TEXTLINE_X + 650, TEXTLINE_Y2 + 27 };
	rcBGM_1 = { TEXTLINE_X + 400 , TEXTLINE_Y2 + TEXTLINE_Y + 18,  TEXTLINE_X + 650, TEXTLINE_Y2 + TEXTLINE_Y + 22 };
	rcBGM_2 = { TEXTLINE_X + 620, TEXTLINE_Y2 + TEXTLINE_Y + 13,  TEXTLINE_X + 650, TEXTLINE_Y2 + TEXTLINE_Y + 27 };
	rcMou_1 = { TEXTLINE_X + 400, TEXTLINE_Y2 + 18,  TEXTLINE_X + 650, TEXTLINE_Y2 + 22 };
	rcMou_2 = { TEXTLINE_X + 420, TEXTLINE_Y2 + 15,  TEXTLINE_X + 450, TEXTLINE_Y2 + 25 };



	//ASDASDAS //TEST
	fileName = "../Resources/Meshes/Boss/Test/npc1.X";

	D3DXMATRIXA16 matCorrection;
	D3DXMatrixScaling(&matCorrection, 0.1f, 0.1f, 0.1f);

	cXMesh_Skinned* pSkinned = RESOURCE_SKINNEDXMESH->GetResource(fileName, &matCorrection);
	pBaseObj = new cBaseObject();
	pBaseObj->SetMesh(pSkinned);
	pBaseObj->SetActive(true);
	pBaseObj->pTransform->SetWorldPosition(30.0f, 200.0f, 0);

	m_pTerrain = new cTerrain();
	m_pTerrain->Init(
		"../Resources/Testures/MyHeight512.bmp",
		"../Resources/Terrain/snow.png",
		"../Resources/Testures/terrain2.png",
		"../Resources/Testures/terrain3.png",
		"../Resources/Testures/terrain4.png",
		"../Resources/Testures/Splat.png",
		1.0f,
		200.0f,
		3,
		100);

	//ȯ�汸����
	this->SetEnvironment("../Resources/TextureCUBE/cubetest.dds");

	//�⺻����Ʈ ����
	this->pSceneBaseDirectionLight->pTransform->RotateWorld(
		90.0f * ONE_RAD, 0, 0);

	this->pMainCamera->SetWorldPosition(0, 53.0f, 32.0f);
	this->pMainCamera->RotateWorld(0, 120.0f * ONE_RAD, 0);
	//this->pMainCamera->AddChild(pTransform);
	SetupSounds();
	KEY_MGR->_mouseSpeed = 1.0f;

	SOUND_MGR->bgmPlay("�޴������", 1.0f);
	{
		weatherEffect = new cPartcleEmitter();
		weatherEffect->SetActive(true);//Ȱ��ȭ ��Ȱ��ȭ
		weatherEffect->pTransform->RotateLocal(180.0f * ONE_RAD, 0, 0);//�������� ����

		VEC_COLOR colors;//����
		colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		colors.push_back(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

		VEC_SCALE scales;//ũ��
		scales.push_back(1.0f);
		scales.push_back(3.0f);
		scales.push_back(5.0f);

		LPDIRECT3DTEXTURE9 pTex = RESOURCE_TEXTURE->GetResource("../Resources/particle/Paticle_Snow.png");

		//��
		//��ƼŬ ���� ����
		weatherEffect->Init(
			5000,						//�ִ� ��ƼŬ ��
			1000.0f,						//�ʴ� ��ƼŬ �߻� ��
			7,							//�ϳ��� ��ƼŬ ���� ������ �ּҰ�
			10,							//�ϳ��� ��ƼŬ ���� ������ �ִ밪
			D3DXVECTOR3(0, -1.0f, 0),	//��ƼŬ ���� �ӵ� �ּҰ� ( ���ñ��� )
			D3DXVECTOR3(0, -2.0f, 0),	//��ƼŬ ���� �ӵ� �ִ밪 ( ���ñ��� )
			D3DXVECTOR3(1, 3, 1),		//��ƼŬ ���� ���� �ּҰ� ( ���ñ��� )
			D3DXVECTOR3(-1, 5, -1),		//��ƼŬ ���� ���� �ִ밪 ( ���ñ��� )
			colors,						//�÷� �迭
			scales,						//������ �迭
			0.1f,						//����ũ�� �ּҰ�
			0.5f,						//�ִ밪
			pTex,						//�ؽ���
			false
		);

		//���� �������� ����
		weatherEffect->EmissionType = PATICLE_EMISSION_TYPE::BOX;
		//weatherEffect->EmissionType = PATICLE_EMISSION_TYPE::SPHERE_OUTLINE;
		weatherEffect->SphereEmissionRange = 30.0f;

		EffectRange(weatherEffect, -1000.0f, 1000.0f, 100.0f, 100.0f, -1000.0f, 1000.0f);

		weatherEffect->StartEmission();
	}
	return S_OK;
}

void cScene_Menu::Scene_Release()
{
	SAFE_DELETE(this->pBaseObj);
	weatherEffect->Release();
	SAFE_DELETE(weatherEffect);

}

void cScene_Menu::Scene_Update(float timeDelta)
{
	weatherEffect->Update(timeDelta);
	//pMainCamera->DefaultControl(timeDelta);
	pBaseObj->Update(timeDelta);

	// �ڽ��� �޽� ��ġ�� �������ش�
	for (int i = 0; i < RESOURCE_SKINNEDXMESH->GetResource(fileName)->GetAnimNum(); ++i) {
		char c_int = i + 0x30;

		if (KEY_MGR->IsOnceDown(c_int)) {
			pBaseObj->GetSkinned()->Play(RESOURCE_SKINNEDXMESH->GetResource(fileName)->GetAnimSet()[i]->GetName(), 0.3f);
		}
	}

	helpMenu(); //�ɼ�â ����
	SmartKey(); //����Ű ����
	//if (KEY_MGR->IsOnceDown('P')) {
	//	LOG_MGR->AddLog("x:%d, y:%d, z:%d", (int)pMainCamera->GetWorldPosition().x, (int)pMainCamera->GetWorldPosition().y, (int)pMainCamera->GetWorldPosition().z);
	//	LOG_MGR->AddLog("fx:%d, fy:%d, fz:%d", (int)pMainCamera->GetForward().x, (int)pMainCamera->GetForward().y, (int)pMainCamera->GetForward().z);
	//	LOG_MGR->AddLog("rx:%d, ry:%d, rz:%d", (int)pMainCamera->GetRight().x, (int)pMainCamera->GetRight().y, (int)pMainCamera->GetRight().z);
	//	LOG_MGR->AddLog("ux:%d, uy:%d, uz:%d", (int)pMainCamera->GetUp().x, (int)pMainCamera->GetUp().y, (int)pMainCamera->GetUp().z);
	//}
}

void cScene_Menu::Scene_Render1()
{
	//���̴��� ������ ����
	this->pBaseObj->Render();

	cXMesh_Skinned::SetBaseLight(this->pSceneBaseDirectionLight);
	cXMesh_Skinned::SetCamera(this->pMainCamera);

	m_pTerrain->Render(
		this->pMainCamera,
		this->pSceneBaseDirectionLight,
		this->pDirectionLightCamera);

	weatherEffect->Render();
	//������ �ִ� Animation ������غ���..
	//RESOURCE_SKINNEDXMESH->GetResource(fileName)->ShowAnimationName(0, 100);
}

//�̽��߰�(171023_UI_�ɼ�â)
void cScene_Menu::Scene_RenderSprite()
{
	if (executeMenu != EXECUTEMENU_NONE)
	{
		//for (int i = 1; i < 13; i++)
		//{
		//   SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcClient[1], &rcClient[1], 0x3500000); //�ɼǷ�Ʈ
		//   SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcClient[i + 1], &rcClient[i + 1], 0x35000000); //�ɼǷ�Ʈ
		//   SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcMain, &rcMain, 0x05000000); //���� ū ��Ʈ
		//}
	}

	switch (executeMenu)
	{
	case EXECUTEMENU_MENU:
		DXFONT_MGR->PrintStyleTextOutline("UI_ī�װ�", "���� �޴�", TEXTLINE_X, TEXTLINE_Y, 0xff4BACC6, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "�� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR1, 0xff000000);
		switch (activeMenu) //����ȯ (���콺 ���� ���� ��)
		{
		case ACTIVEMENU_MENU_RESTART: DXFONT_MGR->PrintStyleTextOutline("UI", "���", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_NEWGAME: DXFONT_MGR->PrintStyleTextOutline("UI", "�� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000);  break;
		case ACTIVEMENU_MENU_SET: DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_EXIT: DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR2, 0xff000000); break;
		default: break;
		}
		break;
	case EXECUTEMENU_MENU_SET:
		DXFONT_MGR->PrintStyleTextOutline("UI_ī�װ�", "���� �޴� / ����", TEXTLINE_X, TEXTLINE_Y, 0xff4BACC6, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "�����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "Ű���� / ���콺", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		switch (activeMenu) //����ȯ (���콺 ���� ���� ��)
		{
		case ACTIVEMENU_MENU_SET_AUDIO: DXFONT_MGR->PrintStyleTextOutline("UI", "�����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU: DXFONT_MGR->PrintStyleTextOutline("UI", "Ű���� / ���콺", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_PREMENU: DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000); break;
		default: break;
		}
		break;
	case EXECUTEMENU_MENU_SET_AUDIO:
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcSound_1, &rcSound_1, 0xffffffff);
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcSound_2, &rcSound_2, 0xffffffff);
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcBGM_1, &rcBGM_1, 0xffffffff);
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcBGM_2, &rcBGM_2, 0xffffffff);



		if (PtInRect(&rcSound_2, GetMousePos())) {
			if (KEY_MGR->IsStayDown(VK_LBUTTON))
			{
				if (464 < GetMousePos().x &&  GetMousePos().x< 690)
					rcSound_2 = { GetMousePos().x - 15, TEXTLINE_Y2 + 13, GetMousePos().x + 15,TEXTLINE_Y2 + 27 };
				//���ΰ� 250

				if (rcSound_2.left <= 450)
					SOUND_MGR->b_volume = 0.0f;
				else if (rcSound_2.left >= 674)
					SOUND_MGR->b_volume = 1.0f;
				else
				{
					//��ĭ��0.0045�� ������ (1/224)
					SOUND_MGR->b_volume = (rcSound_2.left - 450) * 0.0044;
				}
			}
			SOUND_MGR->bgmPlay("�޴������");
		}


		if (PtInRect(&rcBGM_2, GetMousePos())) {
			if (KEY_MGR->IsStayDown(VK_LBUTTON))
			{
				if (460 < GetMousePos().x &&  GetMousePos().x< 690)
					rcBGM_2 = { GetMousePos().x - 15, TEXTLINE_Y2 + TEXTLINE_Y + 13,   GetMousePos().x + 15, TEXTLINE_Y2 + TEXTLINE_Y + 27 };

				if (rcBGM_2.left <= 450)
					SOUND_MGR->s_volume = 0.0f;
				else if (rcBGM_2.left >= 674)
					SOUND_MGR->s_volume = 1.0f;
				else
					//��ĭ��0.0045�� ������ (1/224)
					SOUND_MGR->s_volume = (rcBGM_2.left - 450) * 0.0044;
			}
		}

		DXFONT_MGR->PrintStyleTextOutline("UI_ī�װ�", "���� �޴� / ���� / �����", TEXTLINE_X, TEXTLINE_Y, 0xff4BACC6, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "BMG ����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "ȿ���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		switch (activeMenu) //����ȯ (���콺 ���� ���� ��)
		{
		case ACTIVEMENU_MENU_SET_AUDIO_BGM: DXFONT_MGR->PrintStyleTextOutline("UI", "BMG ����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_AUDIO_EFFECT: DXFONT_MGR->PrintStyleTextOutline("UI", "ȿ���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_AUDIO_PREMENU: DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000); break;
		default: break;
		}
		break;
	case EXECUTEMENU_MENU_SET_KEYMOU:
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcMou_1, &rcMou_1, 0xffffffff);
		SPRITE_MGR->DrawArea(RESOURCE_TEXTURE->GetResource("../Resources/Testures/������.png"), &rcMou_2, &rcMou_2, 0xffffffff);

		if (PtInRect(&rcMou_2, GetMousePos())) {
			if (KEY_MGR->IsStayDown(VK_LBUTTON))
			{
				if (464 < GetMousePos().x &&  GetMousePos().x< 690)
					rcMou_2 = { GetMousePos().x - 15, TEXTLINE_Y2 + 13, GetMousePos().x + 15,TEXTLINE_Y2 + 27 };
				//���ΰ� 250

				//if (rcMou_2.left <= 450)
				//_mouseSpeed = 0.0f;
				if (rcMou_2.left >= 674)
					KEY_MGR->_mouseSpeed = 1.0f;
				else
				{
					//��ĭ��0.0045�� ������ (1/224)
					KEY_MGR->_mouseSpeed = (rcMou_2.left - 450) * 0.0044;
				}
			}

		}


		DXFONT_MGR->PrintStyleTextOutline("UI_ī�װ�", "���� �޴� / ���� / Ű����&���콺", TEXTLINE_X, TEXTLINE_Y, 0xff4BACC6, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���콺 ����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "Ű ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "Ű ���� �ʱ�ȭ", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR1, 0xff000000);
		switch (activeMenu) //����ȯ (���콺 ���� ���� ��)
		{
		case ACTIVEMENU_MENU_SET_KEYMOU_MOUSE: DXFONT_MGR->PrintStyleTextOutline("UI", "���콺 ����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET: DXFONT_MGR->PrintStyleTextOutline("UI", "Ű ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYRESET: DXFONT_MGR->PrintStyleTextOutline("UI", "Ű ���� �ʱ�ȭ", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_PREMENU: DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR2, 0xff000000); break;
		default: break;
		}
		break;
	case EXECUTEMENU_MENU_SET_AUDIO_BGM: //����âġ ���
		break;
	case EXECUTEMENU_MENU_SET_AUDIO_EFFECT: //����âġ ���
		break;
	case EXECUTEMENU_MENU_SET_KEYMOU_MOUSE: //����âġ ���
		break;
	case EXECUTEMENU_MENU_SET_KEYMOU_KEYSET:
		DXFONT_MGR->PrintStyleTextOutline("UI_ī�װ�", "���� �޴� / ���� / Ű ����", TEXTLINE_X, TEXTLINE_Y, 0xff4BACC6, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "�ɱ� / �¼��̵�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� �߻�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 4, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 5, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 6, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 7, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "����ź / �극��ũ", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 8, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "1�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 9, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "2�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 10, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "3�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 11, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 12, TEXTCOLOR1, 0xff000000);
		//����ƮŰ
		DXFONT_MGR->PrintStyleTextOutline("UI", key[0], TEXTLINE_X + 600, TEXTLINE_Y2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[1], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[2], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[3], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[4], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 4, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[5], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 5, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[6], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 6, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[7], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 7, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[8], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 8, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[9], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 9, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[10], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 10, TEXTCOLOR1, 0xff000000);
		DXFONT_MGR->PrintStyleTextOutline("UI", key[11], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 11, TEXTCOLOR1, 0xff000000);
		switch (activeMenu)
		{
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_JUMP: DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[0], TEXTLINE_X + 600, TEXTLINE_Y2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_RUN: DXFONT_MGR->PrintStyleTextOutline("UI", "����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[1], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SEAT: DXFONT_MGR->PrintStyleTextOutline("UI", "�ɱ� / �¼��̵�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[2], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 2, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_FIRE: DXFONT_MGR->PrintStyleTextOutline("UI", "���� �߻�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[3], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 3, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREGUN: DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 4, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[4], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 4, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_POSTGUN: DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 5, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[5], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 5, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_LOADGUN: DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 6, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[6], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 6, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_NEARATTACK: DXFONT_MGR->PrintStyleTextOutline("UI", "���� ����", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 7, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[7], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 7, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_GRENADE: DXFONT_MGR->PrintStyleTextOutline("UI", "����ź / �극��ũ", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 8, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[8], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 8, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT1: DXFONT_MGR->PrintStyleTextOutline("UI", "1�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 9, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[9], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 9, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT2: DXFONT_MGR->PrintStyleTextOutline("UI", "2�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 10, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[10], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 10, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT3: DXFONT_MGR->PrintStyleTextOutline("UI", "3�� ���� ���Լ���", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 11, TEXTCOLOR2, 0xff000000);
			DXFONT_MGR->PrintStyleTextOutline("UI", key[11], TEXTLINE_X + 600, TEXTLINE_Y2 + TEXTLINE_Y * 11, TEXTCOLOR2, 0xff000000); break;
		case ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREMENU: DXFONT_MGR->PrintStyleTextOutline("UI", "���� �޴�", TEXTLINE_X, TEXTLINE_Y2 + TEXTLINE_Y * 12, TEXTCOLOR2, 0xff000000); break;
		default: break;
		}
		break;
	default:
		break;
	}
}

void cScene_Menu::helpMenu()
{
	// �޴� < ���� < �����
	//             Ű����/���콺 ���� < Ű���� ����
	if (executeMenu == EXECUTEMENU_MENU) { //1. ��� 2. ������ 3. ����ã�� 4. ĳ���ͼ��� 5. ���� 6. ������
		if (PtInRect(&rcClient[1], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_RESTART;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				executeMenu = EXECUTEMENU_NONE;
				//SCENE_MGR->ChangeScene("����");
				SCENE_MGR->ChangeSceneWithLoading(
					"����",
					"�ε�2",
					1, 1);
			}
		}
		else if (PtInRect(&rcClient[2], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_NEWGAME;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. ���� [������]�ϴ� �ڵ� �ٷ� �ְų� �Ʒ��� ������ �ڵ�ֱ�
				//    executeMenu = EXECUTEMENU_MENU_NEWGAME;
				//   (EXECUTEMENU_MENU_NEWGAME: ���� [������] ���� �ֱ�)
			}
		}
		else if (PtInRect(&rcClient[3], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > �������� ��.
				executeMenu = EXECUTEMENU_MENU_SET;
			}
		}
		else if (PtInRect(&rcClient[4], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_EXIT;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �ɼ�â ����
				executeMenu = EXECUTEMENU_NONE;
			}
		}
		else activeMenu = ACTIVEMENU_NONE;
	}

	else if (executeMenu == EXECUTEMENU_MENU_SET) { //1. ����� 2. Ű���콺 3. �ڷ�
		if (PtInRect(&rcClient[1], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_AUDIO;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� > ����� ���� ��.
				executeMenu = EXECUTEMENU_MENU_SET_AUDIO;
			}
		}
		else if (PtInRect(&rcClient[2], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� > Ű,���콺 ���� ��.
				executeMenu = EXECUTEMENU_MENU_SET_KEYMOU;
			}
		}
		else if (PtInRect(&rcClient[3], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_PREMENU;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� ���� �ǵ��ư�.
				executeMenu = EXECUTEMENU_MENU;
			}
		}
		else activeMenu = ACTIVEMENU_NONE;
	}

	else if (executeMenu == EXECUTEMENU_MENU_SET_AUDIO) { //1. BGM 2. ȿ���� 3. �ڷ�
		if (PtInRect(&rcClient[1], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_AUDIO_BGM;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. ���� [BGM] �����ϴ� �ڵ� �ٷ� �ְų� �Ʒ��� ������ �ڵ�ֱ�
				//    executeMenu = EXECUTEMENU_MENU_SET_AUDIO_BGM;
				//   (EXECUTEMENU_MENU_SET_AUDIO_BGM: [BGM] ���� �ֱ�)
				//���� Ŭ������ �߰��սô�. //����ȯ�� X
			}
		}
		else if (PtInRect(&rcClient[2], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_AUDIO_EFFECT;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. ���� [ȿ����] �����ϴ� �ڵ� �ٷ� �ְų� �Ʒ��� ������ �ڵ�ֱ�
				//    executeMenu = EXECUTEMENU_MENU_SET_AUDIO_EFFECT;
				//   (EXECUTEMENU_MENU_SET_AUDIO_EFFECT: [ȿ����] ���� �ֱ�)
				//���� Ŭ������ �߰��սô�. //����ȯ�� X
			}
		}
		else if (PtInRect(&rcClient[3], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_AUDIO_PREMENU;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� ���� �ǵ��ư�.
				executeMenu = EXECUTEMENU_MENU_SET;
			}
		}
		else activeMenu = ACTIVEMENU_NONE;
	}

	else if (executeMenu == EXECUTEMENU_MENU_SET_KEYMOU) { //1. ī�޶� ȸ���ӵ� 2. Ű���� 3. Ű ���� 4. �ڷ�
		if (PtInRect(&rcClient[1], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_MOUSE;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. ���� [ī�޶�ȸ��] �����ϴ� �ڵ� �ٷ� �ְų� �Ʒ��� ������ �ڵ�ֱ�
				//    executeMenu = EXECUTEMENU_MENU_SET_KEYMOU_MOUSE;
				//   (EXECUTEMENU_MENU_SET_KEYMOU_MOUSE: [ī�޶�ȸ��] ���� �ֱ�)
			}
		}
		else if (PtInRect(&rcClient[2], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� > Ű,���콺 > Ű���� ���� ��
				executeMenu = EXECUTEMENU_MENU_SET_KEYMOU_KEYSET;
			}
		}
		else if (PtInRect(&rcClient[3], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYRESET;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				gGameKey.name.JUMP = VK_SPACE;
				gGameKey.name.RUN = VK_SHIFT;
				gGameKey.name.SITDOWN = 'C';
				gGameKey.name.FIRE = VK_LBUTTON;
				gGameKey.name.PREVWEAPON = 'R';
				gGameKey.name.NEXTWEAPON = 'R';
				gGameKey.name.RELOAD = 'R';
				gGameKey.name.ATTACK = 'V';
				gGameKey.name.DRAWBOOM = 'G';
				gGameKey.name.WEAPON1st = '1';
				gGameKey.name.WEAPON2st = '2';
				gGameKey.name.WEAPON3st = '3';
			}
		}
		else if (PtInRect(&rcClient[4], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_PREMENU;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� ���� �ǵ��ư�.
				executeMenu = EXECUTEMENU_MENU_SET;
			}
		}
		else activeMenu = ACTIVEMENU_NONE;
	}

	else if (executeMenu == EXECUTEMENU_MENU_SET_KEYMOU_KEYSET) {
		if (PtInRect(&rcClient[1], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_JUMP;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_JUMP;
			}
		}
		else if (PtInRect(&rcClient[2], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_RUN;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_RUN;
			}
		}
		else if (PtInRect(&rcClient[3], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SEAT;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_SITDOWN;
			}
		}
		else if (PtInRect(&rcClient[4], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_FIRE;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_FIRE;
			}
		}
		else if (PtInRect(&rcClient[5], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREGUN;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_PREVWEAPON;
			}
		}
		else if (PtInRect(&rcClient[6], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_POSTGUN;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_NEXTWEAPON;
			}
		}
		else if (PtInRect(&rcClient[7], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_LOADGUN;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_RELOAD;
			}
		}
		else if (PtInRect(&rcClient[8], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_NEARATTACK;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_NEARATTACK;
			}
		}
		else if (PtInRect(&rcClient[9], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_GRENADE;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_DRAWBOOM;
			}
		}
		else if (PtInRect(&rcClient[10], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT1;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_WEAPON1st;
			}
		}
		else if (PtInRect(&rcClient[11], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT2;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_WEAPON2st;
			}
		}
		else if (PtInRect(&rcClient[12], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT3;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				_motion = MOTION_WEAPON3st;
			}
		}
		else if (PtInRect(&rcClient[13], GetMousePos())) {
			activeMenu = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_PREMENU;
			if (KEY_MGR->IsOnceDown(VK_LBUTTON)) {
				//1. Ŭ�� ���� �߰�
				SOUND_MGR->play("�޴�Ŭ����", 1.0f);
				//2. �޴� > ���� ���� �ǵ��ư�.
				executeMenu = EXECUTEMENU_MENU_SET_KEYMOU;
			}
		}
		else activeMenu = ACTIVEMENU_NONE;

		//Ű���氪
		switch (_motion)
		{
		case MOTION_JUMP:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					//if (i == 0) continue;
					if (i == VK_LBUTTON) continue;
					gGameKey.name.JUMP = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_RUN:
			for (int i = 0; i < 256; ++i) {
				//if (i == 0) continue;
				//if (i == 0xFF) break;
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.RUN = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_SITDOWN:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.SITDOWN = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_FIRE:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.FIRE = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_PREVWEAPON:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.PREVWEAPON = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_NEXTWEAPON:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.NEXTWEAPON = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_RELOAD:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.RELOAD = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_NEARATTACK:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.ATTACK = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_DRAWBOOM:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.DRAWBOOM = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_WEAPON1st:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.WEAPON1st = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_WEAPON2st:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.WEAPON2st = (char)i;
					_motion = MOTION_NONE;
					break;
				}
			}
			break;
		case MOTION_WEAPON3st:
			for (int i = 0; i < 256; ++i) {
				if (GetAsyncKeyState(i) & 0x8000 && (GetAsyncKeyState(VK_LBUTTON) == 0x0000)) {
					if (i == VK_LBUTTON) continue;
					gGameKey.name.WEAPON3st = (char)i;
					_motion = MOTION_NONE;

					//executeMenu = EXECUTEMENU_MENU_SET_KEYMOU_KEYSET;
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}

void cScene_Menu::SmartKey()
{
	for (int i = ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_JUMP; i <= ACTIVEMENU_MENU_SET_KEYMOU_KEYSET_SLOT3; ++i)
	{
		//if (activeMenu == i)
		{
			if (gGameKey.arr[i - 17] == 0x41)
				key[i - 17] = "A";
			else if (gGameKey.arr[i - 17] == 0x42)
				key[i - 17] = "B";
			else if (gGameKey.arr[i - 17] == 0x43)
				key[i - 17] = "C";
			else if (gGameKey.arr[i - 17] == 0x44)
				key[i - 17] = "D";
			else if (gGameKey.arr[i - 17] == 0x45)
				key[i - 17] = "E";
			else if (gGameKey.arr[i - 17] == 0x46)
				key[i - 17] = "F";
			else if (gGameKey.arr[i - 17] == 0x47)
				key[i - 17] = "G";
			else if (gGameKey.arr[i - 17] == 0x48)
				key[i - 17] = "H";
			else if (gGameKey.arr[i - 17] == 0x49)
				key[i - 17] = "I";
			else if (gGameKey.arr[i - 17] == 0x4A)
				key[i - 17] = "J";
			else if (gGameKey.arr[i - 17] == 0x4B)
				key[i - 17] = "K";
			else if (gGameKey.arr[i - 17] == 0x4C)
				key[i - 17] = "L";
			else if (gGameKey.arr[i - 17] == 0x4D)
				key[i - 17] = "M";
			else if (gGameKey.arr[i - 17] == 0x4E)
				key[i - 17] = "N";
			else if (gGameKey.arr[i - 17] == 0x4F)
				key[i - 17] = "O";
			else if (gGameKey.arr[i - 17] == 0x50)
				key[i - 17] = "P";
			else if (gGameKey.arr[i - 17] == 0x51)
				key[i - 17] = "Q";
			else if (gGameKey.arr[i - 17] == 0x52)
				key[i - 17] = "R";
			else if (gGameKey.arr[i - 17] == 0x53)
				key[i - 17] = "S";
			else if (gGameKey.arr[i - 17] == 0x54)
				key[i - 17] = "T";
			else if (gGameKey.arr[i - 17] == 0x55)
				key[i - 17] = "U";
			else if (gGameKey.arr[i - 17] == 0x56)
				key[i - 17] = "V";
			else if (gGameKey.arr[i - 17] == 0x57)
				key[i - 17] = "W";
			else if (gGameKey.arr[i - 17] == 0x58)
				key[i - 17] = "X";
			else if (gGameKey.arr[i - 17] == 0x59)
				key[i - 17] = "Y";
			else if (gGameKey.arr[i - 17] == 0x5A)
				key[i - 17] = "Z";
			else if (gGameKey.arr[i - 17] == 0x20)
				key[i - 17] = "SPACE";
			else if (gGameKey.arr[i - 17] == 0x26)
				key[i - 17] = "��";
			else if (gGameKey.arr[i - 17] == 0x28)
				key[i - 17] = "�Ʒ�";
			else if (gGameKey.arr[i - 17] == 0x25)
				key[i - 17] = "����";
			else if (gGameKey.arr[i - 17] == 0x27)
				key[i - 17] = "������";
			else if (gGameKey.arr[i - 17] == 0x02)
				key[i - 17] = "���콺R";
			else if (gGameKey.arr[i - 17] == 0x01)
				key[i - 17] = "���콺L";
			else if (gGameKey.arr[i - 17] == 0x04)
				key[i - 17] = "���콺M";
			else if (gGameKey.arr[i - 17] == 0x31)
				key[i - 17] = "1";
			else if (gGameKey.arr[i - 17] == 0x32)
				key[i - 17] = "2";
			else if (gGameKey.arr[i - 17] == 0x33)
				key[i - 17] = "3";
			else if (gGameKey.arr[i - 17] == 0x34)
				key[i - 17] = "4";
			else if (gGameKey.arr[i - 17] == 0x35)
				key[i - 17] = "5";
			else if (gGameKey.arr[i - 17] == 0x36)
				key[i - 17] = "6";
			else if (gGameKey.arr[i - 17] == 0x37)
				key[i - 17] = "7";
			else if (gGameKey.arr[i - 17] == 0x38)
				key[i - 17] = "8";
			else if (gGameKey.arr[i - 17] == 0x39)
				key[i - 17] = "9";
			else if (gGameKey.arr[i - 17] == 0x30)
				key[i - 17] = "0";
			else if (gGameKey.arr[i - 17] == 0x10)
				key[i - 17] = "SHIFT";
			else if (gGameKey.arr[i - 17] == KEY_MGR->wheelUP())
				key[i - 17] = "WHEEL_UP";
			else if (gGameKey.arr[i - 17] == KEY_MGR->wheelDOWN())
				key[i - 17] = "WHEEL_DOWN";
		}
	}
}

void cScene_Menu::SetupSounds()
{
	SOUND_MGR->addSound("�޴�Ŭ����", "../Resources/Sounds/MenuButtonClick.mp3", false, false);
	SOUND_MGR->addSound("�޴������", "../Resources/Sounds/MenuSound.mp3", true, true);

}

void cScene_Menu::EffectRange(cPartcleEmitter * className, int MinEmissionRange_x, int MaxEmissionRange_x, int MinEmissionRange_y, int MaxEmissionRange_y, int MinEmissionRange_z, int MaxEmissionRange_z)
{
	className->MinEmissionRangeX = MinEmissionRange_x;	//-100.0f;
	className->MaxEmissionRangeX = MaxEmissionRange_x;	//100.0f;

	className->MinEmissionRangeY = MinEmissionRange_y;	//40.0f;
	className->MaxEmissionRangeY = MaxEmissionRange_y;	//40.0f;

	className->MinEmissionRangeZ = MinEmissionRange_z;	//-100.0f;
	className->MaxEmissionRangeZ = MaxEmissionRange_z;	//100.0f;
}
