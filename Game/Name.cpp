#include "stdafx.h"
#include "Name.h"
#include "Actor.h"

using namespace std;

size_t CName::s_LastId = 1;
map<std::string, size_t> CName::s_NameMap;

/**	Make Unique
*******************************************************************************/
CName CName::MakeUnique( const CName& name, CActor* parent )
{
	// Parent is null
	if ( !parent )
		return name;

	// No child with this name, green-light!
	if ( !parent->FindChild( name ) )
		return name;

	std::string origName = name.str();
	origName += "_NEW";

	return MakeUnique( origName, parent );
}

/**	Constructor
*******************************************************************************/
CName::CName()
{
}

/**	Constructor
*******************************************************************************/
CName::CName( const std::string& str )
{
	*this = str;
}

/**	Constructor
*******************************************************************************/
CName::CName( const CName& other ) : m_Id( other.m_Id ), m_String( other.m_String )
{
}

/**	Operator = string
*******************************************************************************/
CName& CName::operator=( const string& str )
{
	map<string, size_t>::iterator it = s_NameMap.find( str );
	if ( it == s_NameMap.end() )
	{
		m_Id = s_LastId++;
		s_NameMap[str] = m_Id;
	}
	else
	{
		m_Id = it->second;
	}

	m_String = str;
	return *this;
}
