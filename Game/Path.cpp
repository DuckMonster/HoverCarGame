#include "stdafx.h"
#include "Path.h"
#include "StringUtils.h"

using namespace std;

/**	Push Name
*******************************************************************************/
void CPath::PushName( const CName& name )
{
	m_PathNames.push_back( name );
}

/**	Pop Name
*******************************************************************************/
void CPath::PopName( )
{
	m_PathNames.erase( m_PathNames.begin() + m_PathNames.size() - 1 );
}

/**	Contains
*******************************************************************************/
bool CPath::Contains( const CName& name ) const
{
	for ( size_t i = 0; i < m_PathNames.size(); i++ )
	{
		if ( m_PathNames[i] == name )
			return true;
	}

	return false;
}

/**	Operator == CPath
*******************************************************************************/
bool CPath::operator==( const CPath & other ) const
{
	if ( m_PathNames.size() != other.m_PathNames.size() )
		return false;

	for ( size_t i = 0; i < m_PathNames.size(); i++ )
	{
		if ( m_PathNames[i] != other.m_PathNames[i] )
			return false;
	}

	return true;
}

/**	Operators
*******************************************************************************/
CPath CPath::operator+( const CPath& other ) const
{
	CPath newPath(*this);
	newPath += other;
	
	return newPath;
}

CPath CPath::operator+( const CName & name ) const
{
	CPath newPath( *this );
	newPath += name;

	return newPath;
}

CPath& CPath::operator+=( const CPath& other )
{
	for ( const CName& path : other.m_PathNames )
		m_PathNames.push_back( path );

	return *this;
}

CPath& CPath::operator+=( const CName& name )
{
	m_PathNames.push_back( name );
	return *this;
}

/**	To URI
*******************************************************************************/
string CPath::ToURI() const
{
	if ( m_PathNames.size() == 0 )
		return string( "/" );

	string result;

	for ( size_t i = 0; i < m_PathNames.size(); i++ )
	{
		result += "/" + m_PathNames[i].str();
	}

	return result;
}

/**	Get Name
*******************************************************************************/
const CName CPath::GetName( size_t index ) const
{
	if ( EnsureNot( index >= m_PathNames.size() ) )
		return CName();

	return m_PathNames[index];
}

/**	From URI
*******************************************************************************/
void CPath::FromURI( const string& str )
{
	Clear();

	vector<string> tokenized;
	StringUtils::Tokenize( str, '/', tokenized, true );

	for ( const string& str : tokenized )
		m_PathNames.emplace_back( str );
}
