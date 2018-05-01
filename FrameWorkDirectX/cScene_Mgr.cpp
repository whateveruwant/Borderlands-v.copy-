#include "StdAfx.h"
#include "cScene_Mgr.h"
#include "cSceneChangeEffectUpDown.h"


DWORD CALLBACK LoadingThread( LPVOID lpParam ) // LPVOID �� void* �� ������ void* �����ʹ� ��� �ּҸ� �� ������ ������ ���� �ּҸ� ����ȯ�Ͽ� ����Ѵ�.
{
	//�Ű������� ������ ����ȯ
	cScene* pScene = (cScene*)lpParam;

	//�ε����� �ʱ�ȭ
	SCENE_MGR->fProgress = 0.0f;
	SCENE_MGR->fString = "";


	//�Ű������� ���� �� �ʱ�ȭ
	pScene->Init();

	//������� �Դٸ� pScene �� Init �� ��������
	//SCENE_MGR->m_pNowScene = pScene;


	//�ʱ�ȭ�� ����ȯȿ�� ���� �����̷�� ���� �ִ�.
	//�̶� ȿ���� �������϶� ��ü���� ���� 
	//����� ȿ���� �������϶� ���� ��ü�ϰ� �Ǹ� 
	//��ȿ�� �������� �ε����� �ҷ����� ��Ȳ�̶��
	//�ε����� ȣ���̵ȴ�.

	while( true )
	{
		//��ü Effect �� ������....
		if( SCENE_MGR->m_pNowChangeEffect == NULL ){

			//�̰����� �������� ���� ���ʿ�� ���� ������ �������� �ε����̱� �����̴�..
			//SCENE_MGR->m_pReleaseScene = SCENE_MGR->m_pNowScene;

			//���� ������ ȿ��
			cSceneChangeEffect* pChangeEffect = SCENE_MGR->m_pChangeEffects[ SCENE_MGR->m_nOutEffect ];

			//���������� ȿ���� �������� �ʴ´ٸ�..
			if( pChangeEffect == NULL )
			{
				SCENE_MGR->m_pNowScene = pScene;
			}

			//���������� ȿ���� �����Ѵٸ�...
			else 
			{
				//���� ȿ���� ���
				SCENE_MGR->m_pNowChangeEffect = pChangeEffect;

				//�������� ȿ���� ������ �ڵ����� �����ȴ�. ( �̶� �������� �ε����̱� ������ ������ ��ϵȴٰ� �ϴ��� ���� ���� �ʴ´� )
				pChangeEffect->ChangeStart( 
					SCENE_MGR->m_pNowScene, 
					pScene, 
					1.0f );
			}
			break;
		}
	}




	return 0;

}

cScene_Mgr::cScene_Mgr(void)
	: m_pNowScene( NULL ), m_pReleaseScene( NULL ), m_pNowChangeEffect( NULL )
{
}


cScene_Mgr::~cScene_Mgr(void)
{
}


HRESULT cScene_Mgr::Init()
{
	//����ȯ ȿ�� �߰�
	m_pChangeEffects.push_back( NULL );		//0 ���� ȿ�� ���� ���� NULL �߰�

	cSceneChangeEffect* pEffect1 = new cSceneChangeEffect;
	pEffect1->Init();
	m_pChangeEffects.push_back(pEffect1  );


	cSceneChangeEffect* pEffect2 = new cSceneChangeEffectUpDown;
	pEffect2->Init();
	m_pChangeEffects.push_back(pEffect2  );

	return S_OK;

}

void cScene_Mgr::Release()
{
	
	//�����ִ� ���� �ִٸ�..
	if( this->m_pNowScene != NULL )
	{
		this->m_pNowScene->Release();
	}

	
	//��ȯȿ�� �ϴ� ������. ( 0 �� �� NULL �̴� 1 ������ )
	for( int i = 1 ; i < this->m_pChangeEffects.size() ; i++ )
	{
		m_pChangeEffects[i]->Relase();
		SAFE_DELETE( m_pChangeEffects[i] );
	}



}

void cScene_Mgr::Update( float timeDelta )
{
	//���������ִٸ�
	if( m_pReleaseScene )
	{
		bool sceneIsLoading = false;
		MAP_SCENE::iterator iter;

		for( iter = this->m_LoadingScenes.begin() ; iter != m_LoadingScenes.end() ; ++iter ){
			//�������� �ε����̶�� ������ �ȵȴ�.
			if( iter->second == m_pReleaseScene )
			{
				sceneIsLoading = true;
				break;
			}
		} 

		//�������� ������ ���� �ε����� �ƴ϶�� �Ƚ��ϰ� ������....
		if( sceneIsLoading == false ){	
			m_pReleaseScene->Release();
		}
		m_pReleaseScene = NULL;
	}



	//���� ȿ���� �ִٸ�....
	if( m_pNowChangeEffect )
	{
		m_pNowChangeEffect->Update( timeDelta );
	}

	else
	{
		if( m_pNowScene ) 
			m_pNowScene->Update( timeDelta );
	}



}

void cScene_Mgr::Render()
{
	//���� ȿ���� �ִٸ�...
	if( m_pNowChangeEffect && m_pNowChangeEffect->IsDrawReady() )	//��ο� �غ� ���� �ʾҴٸ� ü���� Effect �� �׸��� �ȵȴ�.
	{
		m_pNowChangeEffect->Render();
	}

	else{

		if( m_pNowScene ) 
			m_pNowScene->Render();

	}
}


//���ӿ� ���Ǵ� ���߰� ������ Init �� �ȵȴ�.
void cScene_Mgr::AddScene( std::string sceneName, cScene* pScene )
{
	MAP_SCENE::iterator  iter = m_Scenes.find( sceneName );
	
	//�ߺ�����
	if( iter == m_Scenes.end() ){
		m_Scenes.insert( std::make_pair(sceneName,pScene) );
	}
	
}



//���ӿ� ���Ǵ� �ε��� �߰� ������ ��� Init �Ȼ��°� �ȴ�.
void cScene_Mgr::AddLoadingScene( std::string sceneName, cScene* pScene )
{
	MAP_SCENE::iterator  iter = m_LoadingScenes.find( sceneName );
	
	//�ߺ�����
	if( iter == m_LoadingScenes.end() ){

		//Init �� �ϰ� �´�.
		pScene->Init();
		m_LoadingScenes.insert( std::make_pair(sceneName,pScene) );
	}
}



//������
void cScene_Mgr::ChangeScene( std::string sceneName, int effectNum )
{
	//��ü Effect
	cSceneChangeEffect* pChangeEffect = this->m_pChangeEffects[ effectNum ];

	//���� ã�´�.
	MAP_SCENE::iterator  iter = m_Scenes.find( sceneName );

	//��ã�Ҵ�.
	if( iter == m_Scenes.end() ){
		return;
	}

	//������ �����ִ� ���� �ְ� ü���� ȿ���� �ִٸ�...
	if( m_pNowScene && pChangeEffect != NULL ){

		m_pNowChangeEffect = pChangeEffect;

		//�ι�°���� Init �Ȼ��·� ����.
		iter->second->Init();

		//ü���� ȿ�� �ߵ�
		m_pNowChangeEffect->ChangeStart(
				m_pNowScene, 
				iter->second, 
				1.0f			//ȿ�� ü���� �ð� 
				);
	}

	//�װ� �ƴ϶��..
	else{

		//������ �����ִ� �� ���� ������ ����
		m_pReleaseScene = m_pNowScene;

		//���ο���� ��������� 
		m_pNowScene = iter->second;

		//���ο�� �ʱ�ȭ
		m_pNowScene->Init();

	}


}


//���� �����ϴµ� �ε����� �߰��Ͽ� ����
void cScene_Mgr::ChangeSceneWithLoading( 
		std::string sceneName,
		std::string loadingSceneName, 
		int inEffect, int outEffect )
{
	//�ε� ���� ã�´�.
	MAP_SCENE::iterator  iter = m_LoadingScenes.find( loadingSceneName );

	//��ã�Ҵ�.
	if( iter == m_LoadingScenes.end() ){
		return;
	}

	//�ε������
	cScene* pLoadingScene = iter->second;


	//����� ���� ã�´�.
	iter = m_Scenes.find( sceneName );

	//��ã�Ҵ�.
	if( iter == m_Scenes.end() ){
		return;
	}

	//����ɾ� ���
	cScene* pNextScene = iter->second;

	//������ Effect ���
	m_nOutEffect = outEffect;


	//
	// ��Ƽ ������ �ε� ����
	//
	//�ٸ� ������� ���� ���� Init �Ѵ�.
	DWORD dwThreadID;
	HANDLE hThread;

	//�ϲ� ����
	hThread = CreateThread(	
		NULL,					//������ �Ӽ� �ϴ� NULL				
		0,						//�������� ���� ������ (0 �����ϸ� MainThread �� ���� 1MB �� �ȴ� )
		LoadingThread,			//���ӰԻ����Ǵ� �������� �����Լ� CALLBACK �Լ�������
		pNextScene,				//���ӰԻ����Ǵ� �������� �����Լ��� �Ű����� ������
		0,						//������ �÷��� ( 0 ���� �ָ� �ٷ� LoadingThread ������ )
		&dwThreadID				//������� ������ ID 
		);
	
	//���̻� �������ʿ�� ������ �ڵ��� ������ ����
	CloseHandle( hThread );


	//�� ���� ȿ�� Effect
	cSceneChangeEffect* pChangeEffect = m_pChangeEffects[ inEffect ];

	//���Խ� ȿ�� ��ȯ�� ���ٸ�..
	if( pChangeEffect == NULL )
	{
		//�������� ���������� ���
		m_pReleaseScene = m_pNowScene;
		
		//���ο���� �ε������� 
		m_pNowScene = pLoadingScene;
	}

	//���Ծ� ȿ���� ���� �Ѵٸ�...
	else
	{
		//���� ȿ���� ���
		m_pNowChangeEffect = pChangeEffect;

		//�������� ȿ���� ������ �ڵ����� �����ȴ�.
		pChangeEffect->ChangeStart( 
			m_pNowScene, 
			pLoadingScene, 
			1.0f );
	}
}