#pragma once


class cTransform;

class cQuad
{
private:
	typedef struct QUAD_VERTEX{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 uv;
		enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1};
	}QUAD_VERTEX, *LPQUAD_VERTEX;

	QUAD_VERTEX					quadVertex[4];		//æ¿«√∑£ πˆ≈ÿΩ∫
	WORD						quadPlaneIndex[6];			//¿Œµ¶Ω∫

public:
	cQuad(void);
	~cQuad(void);

	void Render( cTransform* pTransform, LPDIRECT3DTEXTURE9 grab, LPDIRECT3DTEXTURE9 distortNormal, float distrotAmount );
};

