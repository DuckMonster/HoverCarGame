#include "stdafx.h"
#include "DataStructure.h"
#include "File.h"

using namespace rapidjson;
using namespace std;

/**	Constructor
*******************************************************************************/
CDataStructure::CDataStructure()
{
}

/**	Constructor
*******************************************************************************/
CDataStructure::CDataStructure( const std::string& file )
{
	LoadFile( file );
}

/**	Destructor
*******************************************************************************/
CDataStructure::~CDataStructure()
{
}

/**	Load File
*******************************************************************************/
void CDataStructure::LoadFile( const std::string& file )
{
	if ( !File::ReadFile( file.c_str(), m_JsonString ) )
	{
		EnsureMsg( false, "Failed to read data file \"%s\"", file );
		return;
	}

	m_File = file;
	m_Document.Parse( m_JsonString.c_str() );
	m_FileChangeTime = File::GetFileModifiedTime( m_File );
}

/**	Load If Changed
*******************************************************************************/
bool CDataStructure::LoadIfChanged()
{
	if ( m_File.empty() )
		return false;

	time_t newChanged = File::GetFileModifiedTime( m_File );
	if ( newChanged != m_FileChangeTime )
	{
		LoadFile( m_File );
		return true;
	}

	return false;
}
