#pragma once

#include "cScene.h"

class cSceneChangeEffect
{
private:
	float		m_TimeFactor;		//��ü �ð� ������� ( Effect ȿ���� ������� ���� ���� )

	LPDIRECT3DTEXTURE9		m_pPrevRenderTexture;		//�������� Texture
	LPDIRECT3DTEXTURE9		m_pNextRenderTexture;		//�������� Texture
	

	float		m_TotalTime;		//��ü �ð�
	float		m_DeltaTime;		//��ü ����ð�

	float		m_StartTime;		//��ü�� ���۵� �ð�

	cScene*		m_pPrevScene;		//������
	cScene*		m_pNextScene;		//������

	bool		m_DrawReady;		//��ü�� ��ο� ����

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

