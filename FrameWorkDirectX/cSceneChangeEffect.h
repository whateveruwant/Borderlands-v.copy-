#pragma once

#include "cScene.h"

class cSceneChangeEffect
{
private:
	float		m_TimeFactor;		//교체 시간 진행비율 ( Effect 효과에 목숨과도 같은 변수 )

	LPDIRECT3DTEXTURE9		m_pPrevRenderTexture;		//랜더링될 Texture
	LPDIRECT3DTEXTURE9		m_pNextRenderTexture;		//랜더링될 Texture
	

	float		m_TotalTime;		//교체 시간
	float		m_DeltaTime;		//교체 진행시간

	float		m_StartTime;		//교체가 시작된 시간

	cScene*		m_pPrevScene;		//이전씬
	cScene*		m_pNextScene;		//다음씬

	bool		m_DrawReady;		//교체씬 드로우 여부

public:
	cSceneChangeEffect(void);
	~cSceneChangeEffect(void);


	void Init();
	void Relase();
	
	void ChangeStart( cScene* pPrevScene, cScene* pNextScene, float changeTime );
	
	void Update( float timeDelta );
	void Render();

	virtual void ChangeEffect( LPDIRECT3DTEXTURE9 pPrev, LPDIRECT3DTEXTURE9 pNext, float factor );

	bool IsDrawReady(){
		return m_DrawReady;
	}

};

