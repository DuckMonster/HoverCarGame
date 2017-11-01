#pragma once
#include "Name.h"

class CPath
{
public:
	CPath() {}
	CPath( const CPath& other ) : m_PathNames( other.m_PathNames ) {}
	CPath( const std::string& str ) { FromURI( str ); }

	void PushName( const CName& name );
	void PopName();
	bool Contains( const CName& name ) const;

	CPath& operator =( const CPath& other ) { m_PathNames = other.m_PathNames; return *this; }
	CPath& operator =( const std::string& uri ) { FromURI( uri ); return *this; }
	bool operator == ( const CPath& other ) const;
	bool operator == ( const std::string& uri ) const { return ( *this == CPath( uri ) ); }
	CPath operator +( const CPath& other ) const;
	CPath operator +( const CName& name ) const;
	CPath& operator +=( const CPath& other );
	CPath& operator += ( const CName& name );

	const CName operator[]( size_t index ) const { return GetName( index ); }

	size_t Length() const { return m_PathNames.size(); }
	void Clear() { m_PathNames.clear(); }

	std::string ToURI() const;
	const CName GetName( size_t index ) const;

private:
	void FromURI( const std::string& str );

	std::vector<CName> m_PathNames;
};