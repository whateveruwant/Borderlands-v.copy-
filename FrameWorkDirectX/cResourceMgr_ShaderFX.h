#pragma once
#include "cresourcemgr.h"

class cResourceMgr_ShaderFX : public cResourceMgr<LPD3DXEFFECT, cResourceMgr_ShaderFX>
{
public:
	cResourceMgr_ShaderFX(void);
	~cResourceMgr_ShaderFX(void);

	virtual LPD3DXEFFECT LoadResource( std::string filePath, void* pParam = NULL );

	virtual void ReleaseResource( LPD3DXEFFECT data );

};

#define RESOURCE_FX cResourceMgr_ShaderFX::GetInstance()