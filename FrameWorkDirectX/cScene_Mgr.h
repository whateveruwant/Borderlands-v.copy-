#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map> 
#include "cSceneChangeEffect.h"
#include "cScene.h"

class cScene_Mgr : public cSingletonBase<cScene_Mgr>
{
private:
	friend class cSceneChangeEffect;		//씬변경 클레스를 내친구로....
	

private:
	typedef std::map<std::string, cScene*>		MAP_SCENE;

private:
	cScene*			m_pNowScene;			//현재 물려 있는씬
	cScene*			m_pReleaseScene;		//날릴 씬
	
	MAP_SCENE		m_Scenes;				//현재 가지고 있는씬
	MAP_SCENE		m_LoadingScenes;		//로딩씬들


	std::vector<cSceneChangeEffect*>		m_pChangeEffects;			//총 변경 Effect 들
	cSceneChangeEffect*						m_pNowChangeEffect;			//현제 변경 Effect


	int				m_nOutEffect;			//로딩시 빠져나가는 Effect 기억

	//로딩 정보
public:
	float			fProgress;
	std::string		fString;

public:
	cScene_Mgr(void);
	~cScene_Mgr(void);

	
	HRESULT Init();
	void Release();
	void Update( float timeDelta );
	void Render();

	//게임에 사용되는 씬추가 씬들은 Init 가 안된다.
	void AddScene( std::string sceneName, cScene* pScene );

	//게임에 사용되는 로딩씬 추가 씬들은 모두 Init 된상태가 된다.
	void AddLoadingScene( std::string sceneName, cScene* pScene );

	//씬변경
	void ChangeScene( 
		std::string sceneName, 
		int effectNum = 0 );

	//씬을 변경하는데 로딩씬을 추가하여 변경
	void ChangeSceneWithLoading( 
		std::string sceneName,
		std::string loadingSceneName,
		int inEffect, 
		int outEffect 
		);

		//로딩쓰래드 함수는 내친구
	friend DWORD CALLBACK LoadingThread( LPVOID lpParam );

};


#define SCENE_MGR cScene_Mgr::GetInstance()