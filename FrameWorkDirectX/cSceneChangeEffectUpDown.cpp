#include "StdAfx.h"
#include "cSceneChangeEffectUpDown.h"


cSceneChangeEffectUpDown::cSceneChangeEffectUpDown(void)
{
}


cSceneChangeEffectUpDown::~cSceneChangeEffectUpDown(void)
{
}

void cSceneChangeEffectUpDown::ChangeEffect( LPDIRECT3DTEXTURE9 pPrev, LPDIRECT3DTEXTURE9 pNext, float factor )
{
	SPRITE_MGR->BeginSpriteRender();

	RECT rcSour = { 0, 0, WINSIZE_X, WINSIZE_Y };
	float y1 =1.0f - factor;
	float y2 = factor;

	SPRITE_MGR->DrawTexture( pPrev, &rcSour, 0,  -WINSIZE_Y * factor , D3DXCOLOR(1, 1, 1, 1 ) );
	SPRITE_MGR->DrawTexture( pNext, &rcSour, 0, WINSIZE_Y * factor, D3DXCOLOR(1, 1, 1, 1 ) );

	SPRITE_MGR->EndSpriteRender();
}
