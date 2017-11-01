#include "stdafx.h"
#include "ResourceBank.h"
#include "MeshResource.h"

using namespace std;

namespace Resource
{
	/**	Constructor
	*******************************************************************************/
	CResourceBank::CResourceBank()
	{
	}

	/**	Destructor
	*******************************************************************************/
	CResourceBank::~CResourceBank()
	{
	}

	/**	Load Mesh
	*******************************************************************************/
	CMeshResource* CResourceBank::GetMesh( const std::string& path )
	{
		// Mesh already loaded?
		{
			map<string, CResource*>::iterator it = m_LookUp.find( path );
			if ( it != m_LookUp.end() )
				return (CMeshResource*)it->second;
		}

		// Load it
		CMeshResource* res = new CMeshResource( this );
		res->Load( path );

		m_ResourceList.push_back( res );
		m_LookUp[path] = res;

		return res;
	}
}