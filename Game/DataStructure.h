#pragma once
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

class CDataStructure
{
public:
	CDataStructure();
	CDataStructure( const std::string& file );
	~CDataStructure();

	void LoadFile( const std::string& file );
	template<typename T>
	inline T Read( const char* key ) const;
	template<typename T>
	inline bool Read( const char* key, T& outValue ) const;
	inline bool Read( const char* key, std::string& outString ) const;

	bool LoadIfChanged();

private:
	std::string m_File;
	std::string m_JsonString;
	rapidjson::Document m_Document;

	time_t m_FileChangeTime = 0;
};

/**	Read (return value)
*******************************************************************************/
template<typename T>
inline T CDataStructure::Read( const char * key ) const
{
	using namespace std;
	using namespace rapidjson;

	Pointer ptr( key );
	if ( !Ensure( ptr.IsValid() ) )
		return T();

	const Value* val = ptr.Get( m_Document );
	if ( !Ensure( val != nullptr ) || !Ensure( val->Is<T>() ) )
		return T();

	return val->Get<T>();
}

/**	Read (assign reference)
*******************************************************************************/
template<typename T>
inline bool CDataStructure::Read( const char * key, T & outValue ) const
{
	using namespace std;
	using namespace rapidjson;

	Pointer ptr( key );
	if ( !Ensure( ptr.IsValid() ) )
		return false;

	const Value* val = ptr.Get( m_Document );
	if ( !Ensure( val != nullptr ) || !Ensure( val->Is<T>() ) )
		return false;

	outValue = val->Get<T>();
	return true;
}

/**	Read (string)
*******************************************************************************/
bool CDataStructure::Read( const char * key, std::string & outString ) const
{
	using namespace std;
	using namespace rapidjson;

	Pointer ptr( key );
	if ( !Ensure( ptr.IsValid() ) )
		return false;

	const Value* val = ptr.Get( m_Document );
	if ( !Ensure( val != nullptr ) || !Ensure( val->IsString() ) )
		return false;

	outString = string( val->GetString(), val->GetStringLength() );
	return true;

}