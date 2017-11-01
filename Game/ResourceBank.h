#pragma once
#include <map>

namespace Resource
{
	class CResource;
	class CMeshResource;

	class CResourceBank
	{
	public:
		CResourceBank();
		~CResourceBank();

		CMeshResource* GetMesh( const std::string& path );

	private:
		std::map<std::string, CResource*> m_LookUp;
		std::vector<CResource*> m_ResourceList;
	};
}