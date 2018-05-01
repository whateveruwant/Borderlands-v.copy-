#pragma once


class cCamera;
class cLight_Direction;
class cLight;
class cTerrain;

class cScene
{
private:
	typedef struct SCENE_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
		DWORD color;
		enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 };
	}SCENE_VERTEX, *LPSCENE_VETEX;

protected:
	cCamera*						pMainCamera;
	LPDIRECT3DCUBETEXTURE9			evironmentTexture;			//환경 Texture
	LPD3DXMESH						evironmemtSphereMesh;		//환경 구
	LPD3DXEFFECT					evironmentEffect;			//환경 Effect

	SCENE_VERTEX					scenePlaneVertex[4];		//씬플랜 버텍스
	WORD							scenePlaneIndex[6];			//인덱스

	LPD3DXEFFECT					postEffect;					//PostEffect

	cLight_Direction*				pSceneBaseDirectionLight;	//씬에 배치된 기본라이팅
	vector<cLight*>					pSceneDarkLight;			//씬에 배치된 기본라이팅
	cCamera*						pDirectionLightCamera;		//방향성 광원에 따른 카메라...

	float							shadowDistance;				//그림자 거리


public:

	sGameKeySetting					gGameKey;
	sGameElseSetting				gGameElse;

	cScene(void);
	~cScene(void);
	
	HRESULT Init();
	void Release();
	void Update( float timeDelta );
	void Render();

	//메인 카메라의 RenderToTexture 만 업데이트한다.
	void RenderToMainCamTexture();

	void SetEnvironment( std::string cubeFilePath );

	void ReadyShadowMap( std::vector<cBaseObject*>* renderObjects, cTerrain* pTerrain = NULL );

	
	//메인카메라의 랜더 Texture 를 얻는다.
	LPDIRECT3DTEXTURE9 GetTexture();

private:

	//씬에서 호출될 함수들을 추상함수로...
	virtual HRESULT Scene_Init() = 0;
	virtual void Scene_Release() = 0;
	virtual void Scene_Update(float timeDelta) = 0;
	

	virtual void Scene_Render0(){}
	virtual void Scene_Render1() = 0;
	virtual void Scene_Render2(){}
	virtual void Scene_Render_AfterPostEffect( LPDIRECT3DTEXTURE9 pScreen ){}

	virtual void Scene_RenderSprite(){}
	virtual void Scene_RenderFont() {}

	//환경구 랜더
	void RenderEnvironment();
	

private:
	void RenderBase();
	void RenderOutline();
	void RenderBBo();
	void RenderDepthOfField();
	void RenderColorLevel();
	void RenderGlow();

};