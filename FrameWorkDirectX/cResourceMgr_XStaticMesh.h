#pragma once
#include "cresourcemgr.h"
#include "cXMesh_Static.h"

class cResourceMgr_XStaticMesh : public cResourceMgr< cXMesh_Static* , cResourceMgr_XStaticMesh >
{
public:
	cResourceMgr_XStaticMesh(void);
	~cResourceMgr_XStaticMesh(void);

	virtual cXMesh_Static* LoadResource( std::string filePath, void* pParam = NULL );

	virtual void ReleaseResource( cXMesh_Static* data );
};

#define RESOURCE_STATICXMESH cResourceMgr_XStaticMesh::GetInstance()


