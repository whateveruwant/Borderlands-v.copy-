#include "StdAfx.h"
#include "cSceneChangeEffect.h"



cSceneChangeEffect::cSceneChangeEffect(void)
	:m_pPrevScene(NULL), m_pNextScene(NULL),
	m_pNextRenderTexture( NULL ), m_pPrevRenderTexture( NULL )
{
}


cSceneChangeEffect::~cSceneChangeEffect(void)
{
}



void cSceneChangeEffect::Init()
{
	/*
		//��Texture �����
	Device->CreateTexture(
		WINSIZE_X,					//Texture ���� �ػ� 
		WINSIZE_Y,					//Texture ���� �ػ�
		1,							//�Ӹ�ü�� ����
		D3DUSAGE_RENDERTARGET,		//RenderTarget �� Texture�̴�
		D3DFMT_A8R8G8B8,			//TEXTURE ���� RenderTexture ����D3DFMT_A8R8G8B8 ������
		D3DPOOL_DEFAULT,			//RenderTarget �� Texture �� Pool �� Default
		&m_pPrevRenderTexture,			//������ Texture �޾ƿ� ������
		NULL						
		);


	//��Texture �����
	Device->CreateTexture(
		WINSIZE_X,					//Texture ���� �ػ� 
		WINSIZE_Y,					//Texture ���� �ػ�
		1,							//�Ӹ�ü�� ����
		D3DUSAGE_RENDERTARGET,		//RenderTarget �� Texture�̴�
		D3DFMT_A8R8G8B8,			//TEXTURE ���� RenderTexture ����D3DFMT_A8R8G8B8 ������
		D3DPOOL_DEFAULT,			//RenderTarget �� Texture �� Pool �� Default
		&m_pNextRenderTexture,			//������ Texture �޾ƿ� ������
		NULL						
		);*/

}


void cSceneChangeEffect::Relase()
{
	//SAFE_RELEASE( m_pPrevRenderTexture );
	//SAFE_RELEASE( m_pNextRenderTexture );
}




void cSceneChangeEffect::ChangeStart( cScene* pPrevScene, cScene* pNextScene, float changeTime )
{
	//��ü ȿ���� ���� ���� ���� �������� ����Ѵ�...
	m_pPrevScene = pPrevScene;
	m_pNextScene = pNextScene;

	//�ð� �ʱ�ȭ
	m_TotalTime = changeTime;
	m_DeltaTime = 0.0f;
	m_TimeFactor = 0.0f;

	//���۽ð� ���
	m_StartTime = ( float )(TIME_MGR->GetNowRealTimeSec() );

	//��ü�� ��ο� ���� false �� ����
	m_DrawReady = false;		
}


void cSceneChangeEffect::Update( float timeDelta )
{
	if( m_DrawReady == false )
	{
		//LOG_MGR->AddLog("�� ��ü ��ο�" );

		//������ȭ�� ����
		this->m_pPrevScene->Update(timeDelta);
		this->m_pPrevScene->RenderToMainCamTexture();

		this->m_pNextScene->Update(timeDelta);
		this->m_pNextScene->RenderToMainCamTexture();

		//���� ����� �Ծ��� ������ ���� ȭ�� Texture �� ���ü� �ִ�....
		this->m_pPrevRenderTexture = this->m_pPrevScene->GetTexture();
		this->m_pNextRenderTexture =  this->m_pNextScene->GetTexture();

		/*
		//���� ����

		//���� Texture
		LPDIRECT3DTEXTURE9 pSrcTex = m_pPrevScene->GetTexture();
		
		//������ ��ġ Texture ( ��Texture �� ��������־�� �Ѵ� )
		LPDIRECT3DTEXTURE9 pDstTex = m_pPrevRenderTexture;

		
		// pSrcTex �� Surface �� ��´�.
		LPDIRECT3DSURFACE9 srcSurface = NULL;
		pSrcTex->GetSurfaceLevel( 0, &srcSurface );

		// pDstTex �� Surface �� ��´�.
		LPDIRECT3DSURFACE9 destSurface = NULL;
		pDstTex->GetSurfaceLevel( 0, &destSurface );

		//��Ӻ���
		Device->StretchRect( 
			srcSurface,				//���� Surface
			NULL,					//���� Surface �� ���� ( NULL �̸� ��ü )
			destSurface, 			//�ҽ� Surface
			NULL, 					//�ҽ� Surface �� ���� ( NULL �̸� ��ü )
			D3DTEXF_LINEAR			//���͸�
			);
	
		srcSurface->Release();
		pDstTex->Release();
		*/

		m_DrawReady = true;
	}


	//�����ð� 
	float nowTime = ( float )(TIME_MGR->GetNowRealTimeSec() );
	m_DeltaTime = nowTime - m_StartTime;


	//Factor ���
	m_TimeFactor = m_DeltaTime / m_TotalTime;
	if( m_TimeFactor > 1.0f )
		m_TimeFactor = 1.0f;

	//��ä�ð��� �� �Ǿ��ٸ�....
	if( m_DeltaTime > m_TotalTime )
	{
		SCENE_MGR->GetInstance()->m_pNowScene = m_pNextScene;

		//�������� ����������...
		SCENE_MGR->GetInstance()->m_pReleaseScene = m_pPrevScene;
		
		//���޴����� ��ü Effect �� ����.
		SCENE_MGR->GetInstance()->m_pNowChangeEffect = NULL;

		m_DrawReady = false;
	}
}




void cSceneChangeEffect::Render()
{
	//��ο� ���� �Ǿ����� �ȴٸ� �׸��� �ʴ´�
	if( m_DrawReady )
	{
		this->ChangeEffect( m_pPrevRenderTexture, m_pNextRenderTexture, m_TimeFactor  );
	}
}



void cSceneChangeEffect::ChangeEffect( LPDIRECT3DTEXTURE9 pPrev, LPDIRECT3DTEXTURE9 pNext, float factor )
{
	
	SPRITE_MGR->BeginSpriteRender();
		
	RECT rcSour = { 0, 0, WINSIZE_X, WINSIZE_Y };
	SPRITE_MGR->DrawTexture( pPrev, &rcSour, 0, 0, D3DXCOLOR(1, 1, 1, 1.0f - factor ) );
	SPRITE_MGR->DrawTexture( pNext, &rcSour, 0, 0, D3DXCOLOR(1, 1, 1, factor ) );

	SPRITE_MGR->EndSpriteRender();
}