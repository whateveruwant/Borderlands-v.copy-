#pragma once

class cBoundBox;
class cBaseObject;

class cXMesh
{
public:

public:
	cXMesh(void);
	~cXMesh(void);
	
	virtual HRESULT Init( std::string filePath, const D3DXMATRIXA16* matCorrection = NULL ) = 0;
	virtual void Release() = 0;
	virtual void Render( const cTransform* pTrans ) = 0;



};

