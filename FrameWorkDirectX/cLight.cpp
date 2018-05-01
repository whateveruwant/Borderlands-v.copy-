#include "StdAfx.h"
#include "cLight.h"
#include "cTransform.h"

cLight::cLight(void)
	: Intensity( 1.0 )
{
	this->pTransform = new cTransform();
}


cLight::~cLight(void)
{
	SAFE_DELETE( this->pTransform );
}
