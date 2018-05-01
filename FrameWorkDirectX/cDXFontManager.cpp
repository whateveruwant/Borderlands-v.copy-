#include "StdAfx.h"
#include "cDXFontManager.h"


cDXFontManager::cDXFontManager(void)
	: m_pDefultFont( NULL )
{
}


cDXFontManager::~cDXFontManager(void)
{
}


HRESULT cDXFontManager::Init( LPDIRECT3DDEVICE9 pDevice )
{
	//디폴트로 사용된 Font 제작
	if( FAILED( D3DXCreateFont(
		pDevice,				//디바이스
		15,					//폰트의 높이, 
		0,					//폰트의 폭 ( 이값이 0 이면 앞에서 설정한 폰트 높이에 비례하여 원래 폰트가 가지고 있는 종횡비에 따라 가로 폭크기가 자동으로 맞춰진다 ) 	
		FW_BOLD,			//폰트의 두께 설정( FW_BOLD 폰트의 BOLD 스타일 크기를 같는다 )
		1,					//밉맵 ( 그냥 1  )
		FALSE,				//이텔릭 여부
		DEFAULT_CHARSET,	//캐릭터 Set ( 그냥 DEFAULT_CHARSET )
		OUT_DEFAULT_PRECIS,	//외각 처리방식 ( 그냥 OUT_DEFAULT_PRECIS )
		DEFAULT_QUALITY,	//퀄리티 ( 그냥 DEFAULT_QUALITY ),
		DEFAULT_PITCH | FF_DONTCARE,		//글시체 패밀리셋 설정 ( DEFAULT_PITCH | FF_DONTCARE )
		"굴림",				//사용될 폰트이름 그냥 폰트이름을 스트링으로 쓰면 된다.
		&m_pDefultFont 
		) ) )
		return E_FAIL;

	return S_OK;
}

void cDXFontManager::Release()
{
	SAFE_RELEASE( m_pDefultFont );

	//Style Map 에있는 폰트 다 턴다
	MAP_FONT_ITER iter;
	for( iter = m_FontStyleMap.begin() ; iter != m_FontStyleMap.end() ; ++iter ){
		SAFE_RELEASE( iter->second );
	}

}


void cDXFontManager::AddStyle( LPDIRECT3DDEVICE9 pDevice, std::string style,  std::string fontName, float fontSize )
{
	//이미 style 이름의 키가 존재한다면 추가되지 않는다
	MAP_FONT_ITER find = m_FontStyleMap.find( style );
	if( find != m_FontStyleMap.end() )
		return;


	//새로운 폰트 추가
	LPD3DXFONT newFont;
	D3DXCreateFont(
		pDevice,			//디바이스
		fontSize,			//폰트의 높이, 
		0.0f,				//폰트의 폭 ( 이값이 0 이면 앞에서 설정한 폰트 높이에 비례하여 원래 폰트가 가지고 있는 종횡비에 따라 가로 폭크기가 자동으로 맞춰진다 ) 	
		FW_BOLD,			//폰트의 두께 설정( FW_BOLD 폰트의 BOLD 스타일 크기를 같는다 )
		1,					//밉맵 ( 그냥 1  )
		FALSE,				//이텔릭 여부
		DEFAULT_CHARSET,	//캐릭터 Set ( 그냥 DEFAULT_CHARSET )
		OUT_DEFAULT_PRECIS,	//외각 처리방식 ( 그냥 OUT_DEFAULT_PRECIS )
		DEFAULT_QUALITY,	//퀄리티 ( 그냥 DEFAULT_QUALITY ),
		DEFAULT_PITCH | FF_DONTCARE,		//글시체 패밀리셋 설정 ( DEFAULT_PITCH | FF_DONTCARE )
		fontName.c_str(),				//사용될 폰트이름 그냥 폰트이름을 스트링으로 쓰면 된다.
		&newFont 
		);

	//style 이름을 Key Map 에 추가
	m_FontStyleMap.insert( std::make_pair(style,newFont) );

}


void cDXFontManager::PrintText( std::string str, int x, int y, DWORD color )
{
	//출력영역
	RECT rcTextArea = { x, y, 0, 0 };

	m_pDefultFont->DrawTextA(
		NULL, 
		str.c_str(),		//string 의 내용을 char* 로 전달
		-1, 
		&rcTextArea, 
		DT_LEFT | DT_NOCLIP,		// DT_NOCLIP 영영은 무시됨
		color );

}	

void cDXFontManager::PrintTextShadow( std::string str, int x, int y, DWORD color, DWORD shadow )
{
	//쉐도우 먼저 찍는다.
	PrintText( str, x + 1, y + 1, shadow );

	//본래 Text 를 찍는다.
	PrintText( str, x, y, color );
}

void cDXFontManager::PrintTextOutline( std::string str, int x, int y, DWORD color, DWORD outLine )
{
	//외각 찍는다
	PrintText( str, x + 1, y, outLine );
	PrintText( str, x, y + 1, outLine );
	PrintText( str, x - 1, y, outLine );
	PrintText( str, x, y - 1, outLine );

	//본래 Text 를 찍는다.
	PrintText( str, x, y, color );
}

void  cDXFontManager::PrintStyleText( std::string style, std::string str, int x, int y, DWORD color )
{
	LPD3DXFONT pFont = NULL;

	//style 키에 해당되는 pair 노드를 찾아 해당위치의 값을 iter 에 대입
	//만약 못찾았다면... end() 가 참조된다.
	MAP_FONT_ITER find = m_FontStyleMap.find( style );

	//못찾았다면.. Default 스타일로.
	if( find == m_FontStyleMap.end() ){
		pFont = m_pDefultFont;
	}
	//찾았다면...
	else{
		pFont = find->second;
	}

	//출력영역
	RECT rcTextArea = { x, y, 0, 0 };

	pFont->DrawTextA(
		NULL, 
		str.c_str(),		//string 의 내용을 char* 로 전달
		-1, 
		&rcTextArea, 
		DT_LEFT | DT_NOCLIP,		// DT_NOCLIP 영영은 무시됨
		color );

}

void  cDXFontManager::PrintStyleTextShadow( std::string style, std::string str, int x, int y, DWORD color, DWORD shadow  )
{
	//쉐도우 먼저 찍는다.
	PrintStyleText( style, str, x + 1, y + 1, shadow );

	//본래 Text 를 찍는다.
	PrintStyleText( style, str, x, y, color );
}

void  cDXFontManager::PrintStyleTextOutline( std::string style, std::string str, int x, int y, DWORD color, DWORD outLine  )
{
	//외각 찍는다
	PrintStyleText(style, str, x + 2, y, outLine);
	PrintStyleText(style, str, x, y + 2, outLine);
	PrintStyleText(style, str, x - 2, y, outLine);
	PrintStyleText(style, str, x, y - 2, outLine);

	//본래 Text 를 찍는다.
	PrintStyleText( style, str, x, y, color );
}
