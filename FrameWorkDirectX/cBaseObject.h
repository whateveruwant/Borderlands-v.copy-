#pragma once

class cXMesh;
class cTransform;
class cSkinnedAnimation;

class cBaseObject
{
public:
	cTransform*			pTransform;
	cXMesh*				pMesh;						//니가 사용하는 메쉬가 뭐니?
	cBoundBox			BoundBox;
	bool				IgnoreCreateShadow;			//쉐도우맵 만들때 무시되니
	bool				ApplyShadow;				//쉐도우 받니?

	DWORD				CullMode;


protected:
	bool				bActive;	//활성화 여부
	cSkinnedAnimation*	pSkinned;	//스킨드 Animtion


public:
	cBaseObject(void);
	~cBaseObject(void);

	void Update( float timeDelta );

	void Render(){
		if( this->bActive )
			this->BaseObjectRender();
	}

	void RenderShadow(){
		if( this->bActive )
			this->BaseObjectRenderShadow();
	}


	//랜더링 될 메쉬를 셋팅한다.
	void SetMesh( cXMesh*	pMesh );

	//활성화 셋팅
	void SetActive( bool bActive );

	cSkinnedAnimation* GetSkinned() {
		return pSkinned;
	}

	//활성화 여부를 얻는다.
	bool IsActive() {
		return this->bActive;
	}


	//셋팅된 메쉬에 따라 바운드박스를 재계산 한다.
	void ComputeBoundBox();



protected:

	//override 해서 쓰시오....
	virtual void BaseObjectEnable() {}							//BaseObject 가 활성화 될때 실행
	virtual void BaseObjectDisable(){}							//BaseObject 가 비활성화 될때 실행
	virtual void BaseObjectUpdate( float timeDelta ) {}			//BaseObject 가 Update 때 실행....
	virtual void BaseObjectNoActiveUpdate( float timeDelte){}	//BaseObject 가 비활성화시 업데이트 실행....
	virtual void BaseObjectRender();							//BaseObject 를 그릴때 실행
	virtual void BaseObjectRenderShadow();						//BaseObject 를 그릴때 실행
};

