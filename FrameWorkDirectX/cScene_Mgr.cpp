#include "StdAfx.h"
#include "cScene_Mgr.h"
#include "cSceneChangeEffectUpDown.h"


DWORD CALLBACK LoadingThread( LPVOID lpParam ) // LPVOID 는 void* 형 포인터 void* 포인터는 모든 주소를 다 받을수 있으며 받은 주소를 형변환하여 사용한다.
{
	//매개변수로 받은씬 형변환
	cScene* pScene = (cScene*)lpParam;

	//로딩정보 초기화
	SCENE_MGR->fProgress = 0.0f;
	SCENE_MGR->fString = "";


	//매개변수로 받은 씬 초기화
	pScene->Init();

	//여기까지 왔다면 pScene 은 Init 가 끝난것임
	//SCENE_MGR->m_pNowScene = pScene;


	//초기화가 씬전환효과 보다 먼저이루어 질수 있다.
	//이때 효과가 진행중일때 교체하지 말자 
	//참고로 효과가 진행중일때 씬을 교체하게 되면 
	//씬효과 가끝나고 로딩씬이 불러지는 상황이라면
	//로딩씬이 호출이된다.

	while( true )
	{
		//교체 Effect 가 끝났다....
		if( SCENE_MGR->m_pNowChangeEffect == NULL ){

			//이곳에서 이전씬을 해제 할필요는 없다 어차피 이전씬은 로딩씬이기 때문이다..
			//SCENE_MGR->m_pReleaseScene = SCENE_MGR->m_pNowScene;

			//빠져 나가는 효과
			cSceneChangeEffect* pChangeEffect = SCENE_MGR->m_pChangeEffects[ SCENE_MGR->m_nOutEffect ];

			//빠져나가는 효과가 존재하지 않는다면..
			if( pChangeEffect == NULL )
			{
				SCENE_MGR->m_pNowScene = pScene;
			}

			//빠져나가는 효과가 존재한다면...
			else 
			{
				//변경 효과씬 등록
				SCENE_MGR->m_pNowChangeEffect = pChangeEffect;

				//이전씬은 효과가 끝나고 자동으로 해제된다. ( 이때 이전씬은 로딩씬이기 때문에 해제가 등록된다고 하더라도 해제 되지 않는다 )
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
	//씬전환 효과 추가
	m_pChangeEffects.push_back( NULL );		//0 번은 효과 없음 따라서 NULL 추가

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
	
	//물려있는 씬이 있다면..
	if( this->m_pNowScene != NULL )
	{
		this->m_pNowScene->Release();
	}

	
	//전환효과 싹다 날린다. ( 0 번 은 NULL 이니 1 번부터 )
	for( int i = 1 ; i < this->m_pChangeEffects.size() ; i++ )
	{
		m_pChangeEffects[i]->Relase();
		SAFE_DELETE( m_pChangeEffects[i] );
	}



}

void cScene_Mgr::Update( float timeDelta )
{
	//날릴씬이있다면
	if( m_pReleaseScene )
	{
		bool sceneIsLoading = false;
		MAP_SCENE::iterator iter;

		for( iter = this->m_LoadingScenes.begin() ; iter != m_LoadingScenes.end() ; ++iter ){
			//날릴씬이 로딩씬이라면 날리면 안된다.
			if( iter->second == m_pReleaseScene )
			{
				sceneIsLoading = true;
				break;
			}
		} 

		//지금현제 날리는 씬이 로딩씬이 아니라면 안심하고 날려라....
		if( sceneIsLoading == false ){	
			m_pReleaseScene->Release();
		}
		m_pReleaseScene = NULL;
	}



	//변경 효과가 있다면....
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
	//변경 효과가 있다면...
	if( m_pNowChangeEffect && m_pNowChangeEffect->IsDrawReady() )	//드로우 준비가 되지 않았다면 체인지 Effect 를 그리면 안된다.
	{
		m_pNowChangeEffect->Render();
	}

	else{

		if( m_pNowScene ) 
			m_pNowScene->Render();

	}
}


//게임에 사용되는 씬추가 씬들은 Init 가 안된다.
void cScene_Mgr::AddScene( std::string sceneName, cScene* pScene )
{
	MAP_SCENE::iterator  iter = m_Scenes.find( sceneName );
	
	//중복방지
	if( iter == m_Scenes.end() ){
		m_Scenes.insert( std::make_pair(sceneName,pScene) );
	}
	
}



//게임에 사용되는 로딩씬 추가 씬들은 모두 Init 된상태가 된다.
void cScene_Mgr::AddLoadingScene( std::string sceneName, cScene* pScene )
{
	MAP_SCENE::iterator  iter = m_LoadingScenes.find( sceneName );
	
	//중복방지
	if( iter == m_LoadingScenes.end() ){

		//Init 를 하고 는다.
		pScene->Init();
		m_LoadingScenes.insert( std::make_pair(sceneName,pScene) );
	}
}



//씬변경
void cScene_Mgr::ChangeScene( std::string sceneName, int effectNum )
{
	//교체 Effect
	cSceneChangeEffect* pChangeEffect = this->m_pChangeEffects[ effectNum ];

	//씬을 찾는다.
	MAP_SCENE::iterator  iter = m_Scenes.find( sceneName );

	//못찾았다.
	if( iter == m_Scenes.end() ){
		return;
	}

	//기존에 물려있는 씬이 있고 체인지 효과가 있다면...
	if( m_pNowScene && pChangeEffect != NULL ){

		m_pNowChangeEffect = pChangeEffect;

		//두번째씬은 Init 된상태로 들어간다.
		iter->second->Init();

		//체인지 효과 발동
		m_pNowChangeEffect->ChangeStart(
				m_pNowScene, 
				iter->second, 
				1.0f			//효과 체인지 시간 
				);
	}

	//그게 아니라면..
	else{

		//기존에 물려있던 씬 해제 씬으로 대입
		m_pReleaseScene = m_pNowScene;

		//새로운씬을 현재씬으로 
		m_pNowScene = iter->second;

		//새로운씬 초기화
		m_pNowScene->Init();

	}


}


//씬을 변경하는데 로딩씬을 추가하여 변경
void cScene_Mgr::ChangeSceneWithLoading( 
		std::string sceneName,
		std::string loadingSceneName, 
		int inEffect, int outEffect )
{
	//로딩 씬을 찾는다.
	MAP_SCENE::iterator  iter = m_LoadingScenes.find( loadingSceneName );

	//못찾았다.
	if( iter == m_LoadingScenes.end() ){
		return;
	}

	//로딩씬기억
	cScene* pLoadingScene = iter->second;


	//변경될 씬을 찾는다.
	iter = m_Scenes.find( sceneName );

	//못찾았다.
	if( iter == m_Scenes.end() ){
		return;
	}

	//변경될씬 기억
	cScene* pNextScene = iter->second;

	//나가는 Effect 기억
	m_nOutEffect = outEffect;


	//
	// 멀티 쓰레드 로딩 시작
	//
	//다른 쓰레드로 다음 씬을 Init 한다.
	DWORD dwThreadID;
	HANDLE hThread;

	//일꾼 생성
	hThread = CreateThread(	
		NULL,					//쓰레드 속성 일단 NULL				
		0,						//쓰레드의 스텍 사이즈 (0 으로하면 MainThread 와 같은 1MB 가 된다 )
		LoadingThread,			//새롭게생성되는 쓰레드의 메인함수 CALLBACK 함수포인터
		pNextScene,				//새롭게생성되는 쓰레드의 메인함수의 매개변수 포인터
		0,						//쓰레드 플레그 ( 0 으로 주면 바로 LoadingThread 가실행 )
		&dwThreadID				//만들어진 쓰레드 ID 
		);
	
	//더이상 조작할필요는 없느니 핸들은 날려도 무방
	CloseHandle( hThread );


	//씬 변경 효과 Effect
	cSceneChangeEffect* pChangeEffect = m_pChangeEffects[ inEffect ];

	//진입시 효과 전환이 없다면..
	if( pChangeEffect == NULL )
	{
		//이전씬은 해제씬으로 등록
		m_pReleaseScene = m_pNowScene;
		
		//새로운씬을 로딩씬으로 
		m_pNowScene = pLoadingScene;
	}

	//진입씬 효과가 존재 한다면...
	else
	{
		//변경 효과씬 등록
		m_pNowChangeEffect = pChangeEffect;

		//이전씬은 효과가 끝나고 자동으로 해제된다.
		pChangeEffect->ChangeStart( 
			m_pNowScene, 
			pLoadingScene, 
			1.0f );
	}
}