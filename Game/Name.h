#pragma once

class CActor;

class CName
{
	static size_t s_LastId;
	static std::map<std::string, size_t> s_NameMap;

public:
	static CName MakeUnique( const CName& name, CActor* parent );

public:
	CName();
	CName( const std::string& str );
	CName( const char* str ) : CName( std::string( str ) ) {}
	CName( const CName& other );

	CName& operator=( const CName& other ) { m_Id = other.m_Id; m_String = other.m_String; return *this; }
	CName& operator=( const std::string& str );
	CName& operator=( const char* str ) { return ( *this = std::string( str ) ); }
	bool operator ==( const CName& other ) const { return IsValid() && m_Id == other.m_Id; }
	bool operator ==( const std::string& str ) const { return m_String == str; }
	bool operator !=( const CName& other ) const { return !( *this == other ); }
	bool operator !=( const std::string& str ) const { return !( *this == str ); }

	bool IsValid() const { return m_Id != 0; }

	const std::string& str() const { return m_String; }
	const char* c_str() const { return m_String.c_str(); }

private:
	size_t m_Id = 0;
	std::string m_String = "";
};