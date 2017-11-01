#include "stdafx.h"
#include "Resource.h"

namespace Resource
{
	CResource::CResource( CResourceBank * bank ) :
		m_Bank( bank )
	{
	}

	CResource::~CResource()
	{
		Destroy();
	}
}