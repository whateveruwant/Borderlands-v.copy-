#pragma once

#include "cSingletonBase.h"
#include <string>
#include <map> 
#include "cSceneChangeEffect.h"
#include "cScene.h"

class cScene_Mgr : public cSingletonBase<cScene_Mgr>
{
private:
	friend class cSceneChangeEffect;		//������ Ŭ������ ��ģ����....
	

private:
	typedef std::map<std::string, cScene*>		MAP_SCENE;

private:
	cScene*			m_pNowScene;			//���� ���� �ִ¾�
	cScene*			m_pReleaseScene;		//���� ��
	
	MAP_SCENE		m_Scenes;				//���� ������ �ִ¾�
	MAP_SCENE		m_LoadingScenes;		//�ε�����


	std::vector<cSceneChangeEffect*>		m_pChangeEffects;			//�� ���� Effect ��
	cSceneChangeEffect*						m_pNowChangeEffect;			//���� ���� Effect


	int				m_nOutEffect;			//�ε��� ���������� Effect ���

	//�ε� ����
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

	//���ӿ� ���Ǵ� ���߰� ������ Init �� �ȵȴ�.
	void AddScene( std::string sceneName, cScene* pScene );

	//���ӿ� ���Ǵ� �ε��� �߰� ������ ��� Init �Ȼ��°� �ȴ�.
	void AddLoadingScene( std::string sceneName, cScene* pScene );

	//������
	void ChangeScene( 
		std::string sceneName, 
		int effectNum = 0 );

	//���� �����ϴµ� �ε����� �߰��Ͽ� ����
	void ChangeSceneWithLoading( 
		std::string sceneName,
		std::string loadingSceneName,
		int inEffect, 
		int outEffect 
		);

		//�ε������� �Լ��� ��ģ��
	friend DWORD CALLBACK LoadingThread( LPVOID lpParam );

};


#define SCENE_MGR cScene_Mgr::GetInstance()