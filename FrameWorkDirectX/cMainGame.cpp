#include "StdAfx.h"
#include "cMainGame.h"
#include "cImage.h"
#include "cScene_1stLoading.h"
#include "cScene_2ndLoading.h"
#include "cScene_Menu.h"
#include "cScene_Option.h"
#include "cScene_inGame.h"
cMainGame::cMainGame(void)
{
}

cMainGame::~cMainGame(void)
{
}

//����
HRESULT cMainGame::Init(void)
{
	//�����õ�
	SetRandomSeed();
	
	//����̽� ����
	CreateDevice();


	//�Ŵ��� ����
	TIME_MGR->Init();
	LOG_MGR->Init( LOG_WINDOW | LOG_FILE, g_hWnd, "Dump" );
	DXFONT_MGR->Init( Device );	
	GIZMO_MGR->Init( Device );
	SPRITE_MGR->Init( Device );
	SCENE_MGR->Init();
	SOUND_MGR->Setup();
	


	//��Ʈ��Ÿ�� �߰�
	DXFONT_MGR->AddStyle(Device, "UI", "HY�߰��", 40.0f);
	DXFONT_MGR->AddStyle(Device, "damage", "HY�߰��", 30.0f);
	//HY������M
	//Franklin Gothic Demi Cond
	DXFONT_MGR->AddStyle(Device, "playerUI", "HY������M", 25.0f);
	DXFONT_MGR->AddStyle(Device, "UI_ī�װ�", "HY�߰��", 20.0f);
	DXFONT_MGR->AddStyle(Device, "playerUI2", "HY������M", 18.0f);
	DXFONT_MGR->AddStyle(Device, "playerUI3", "HY������M", 42.0f);
	DXFONT_MGR->AddStyle(Device, "playerUI4", "HY������M", 30.0f);
	DXFONT_MGR->AddStyle(Device, "playerUI5", "HY������M", 22.0f);
	DXFONT_MGR->AddStyle(Device, "playerUI6", "HY������M", 13.0f);
	//���ӿ� ���Ǵ� �� �߰�
	SCENE_MGR->AddScene("�޴�", new cScene_Menu());
	SCENE_MGR->AddScene("����", new cScene_inGame());
	SCENE_MGR->AddScene( "�ε�1", new cScene_1stLoading());
	SCENE_MGR->AddLoadingScene("�ε�2", new cScene_2ndLoading());

	//���� ���۾�
	SCENE_MGR->ChangeScene( "�޴�" );
	
	//return E_FAIL;
	return S_OK;		
}	

//����
void cMainGame::Release()
{
	
	//������
	//m_pNowScene->Release();
	SAFE_DELETE( m_pNowScene );

	//�Ŵ��� ����
	GIZMO_MGR->Release();
	cGizmoManager::ReleaseInstance();
	TIME_MGR->Release();
	cTimeMgr::ReleaseInstance();
	KEY_MGR->Release();
	cKeyMgr::ReleaseInstance();
	LOG_MGR->Release();
	cLogMgr::ReleaseInstance();
	DXFONT_MGR->Release();
	cDXFontManager::ReleaseInstance();
	SPRITE_MGR->Release();
	cSpriteManager::ReleaseInstance();
	SCENE_MGR->Release();
	cScene_Mgr::ReleaseInstance();
	cPhysicManager::ReleaseInstance();

	RESOURCE_TEXTURE->ClearResource();
	cResourceMgr_Texture::ReleaseInstance();
	RESOURCE_FX->ClearResource();
	cResourceMgr_ShaderFX::ReleaseInstance();
	RESOURCE_STATICXMESH->ClearResource();
	cResourceMgr_XStaticMesh::ReleaseInstance();
	RESOURCE_SKINNEDXMESH->ClearResource();
	cResourceMgr_XSkinnedMesh::ReleaseInstance();

	SOUND_MGR->Destroy();
	cSoundManager::ReleaseInstance();

	//����̽� ����
	ReleaseDevice();
}

//������Ʈ
void cMainGame::Update()
{
	//Ÿ�ӸŴ��� ������Ʈ
	TIME_MGR->UpdateTime(60.0f);
	//�������� ���� �ð�
	double timeDelta = TIME_MGR->GetFrameDeltaSec();

	//��������Ʈ
	SCENE_MGR->Update( timeDelta );
	SOUND_MGR->update();
}

//��ο�
void cMainGame::Draw()
{
	//���۸� û���Ѵ�
	HRESULT result = Device->Clear(
		0,							//û���� ������ D3DRECT �迭 ����		( ��ü Ŭ���� 0 )
		NULL,						//û���� ������ D3DRECT �迭 ������		( ��ü Ŭ���� NULL )
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,		//û�ҵ� ���� �÷��� ( D3DCLEAR_TARGET �÷�����, D3DCLEAR_ZBUFFER ���̹���, D3DCLEAR_STENCIL ���ٽǹ���
		0xff000000,					//�÷����۸� û���ϰ� ä���� ����( 0xAARRGGBB )
		1.0f,						//���̹��۸� û���Ұ� ( 0 ~ 1, 0 �� ī�޶󿡼� ���ϰ���� 1 �� ī�޶󿡼� ���� �� )
		0							//���ٽ� ���۸� ä�ﰪ
		);
	
	//���� û�Ұ� �����ߴٸ�.......
	if( SUCCEEDED( result ) )
	{
		//����̽� ������ ���� ���
		Device->BeginScene();

		//������ ���

		//������
		SCENE_MGR->Render();


#ifdef _DEBUG		//����� ��忡���� ����
		//����̽� ������ ���� ���
		//���� �׸���
		//GIZMO_MGR->WorldGrid( 1, 10 );

		//Ÿ������ ���
		TIME_MGR->DrawTimeInfo();
#endif

		Device->EndScene();


		//�������� ���۳����� ȭ�鿡 ����
		Device->Present(  0, 0, 0, 0 );
	}
	

}



//////////////////////////////////////////////////////////////////////////

//���� ���ν��� �Լ�
LRESULT cMainGame::MainProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
	case WM_MOUSEWHEEL:
	{
		_ptwheel = GET_WHEEL_DELTA_WPARAM(wParam);
	}break;

	case WM_TIMER:
		Update();
		break;

	case WM_KEYDOWN:
		switch( wParam )
		{
		}
		break;

	case WM_DESTROY:			//�����찡 �ı��ȴٸ�..
		PostQuitMessage( 0 );	//���α׷� ���� ��û ( �޽��� ������ ���������� �ȴ� )
		break;
	}
	return DefWindowProc( hWnd, iMessage, wParam, lParam );
}

