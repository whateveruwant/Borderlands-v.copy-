#pragma once

class cBaseObject;

class cFrustum
{
private:
	D3DXVECTOR3				m_RHWPos[8];		//viewProjection 변환전 위치
	D3DXVECTOR3				m_Pos[8];			//변환 후 위치
	D3DXPLANE				m_Plane[6];			//절두체 플렌


public:
	cFrustum(void);
	~cFrustum(void);

	//프러스텀 정보 업데이트 ( View * Projection 행렬을 요구한다 )
	void UpdateFrustum( const D3DXMATRIXA16* pmatViewProj );

	//Base 오브젝트가 프러스텀 안에있는지 체크
	bool IsInFrustum( cBaseObject* pObject );

	//프러스텀 기즈모를 그린다.
	void RenderGizmo();

};

