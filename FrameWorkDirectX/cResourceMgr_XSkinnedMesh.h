#pragma once

#include "cresourcemgr.h"
#include "cXMesh_Skinned.h"

class cResourceMgr_XSkinnedMesh : public cResourceMgr< cXMesh_Skinned* , cResourceMgr_XSkinnedMesh >
{
public:
	cResourceMgr_XSkinnedMesh(void);
	~cResourceMgr_XSkinnedMesh(void);

	virtual cXMesh_Skinned* LoadResource( std::string filePath, void* pParam = NULL );

	virtual void ReleaseResource( cXMesh_Skinned* data );
};

#define RESOURCE_SKINNEDXMESH cResourceMgr_XSkinnedMesh::GetInstance()