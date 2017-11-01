#pragma once
#include <stdio.h>
#include <string>
#include "Path.h"

class CScene;
class CActor;
class CComponent;

class CSerializer
{
public:
	CSerializer( const CScene* scene = nullptr ) :
		m_ReadData( nullptr ), m_IsWriting( true ), m_Size( 16 ), m_Scene( scene )
	{
		m_WriteData = new char[m_Size];
	}
	CSerializer( const void* data, size_t size, const CScene* scene = nullptr ) :
		m_WriteData( nullptr ), m_ReadData( (const char*)data ), m_Size( size ), m_IsWriting( false ), m_Scene( scene )
	{
	}
	CSerializer( const CSerializer& other ) = delete;
	CSerializer( CSerializer&& other ) = delete;

	~CSerializer()
	{
		if ( m_IsWriting )
		{
			delete[] m_WriteData;
			m_WriteData = nullptr;
		}
	}

	bool IsWriting() const { return m_IsWriting; }
	const char* Data() const { return m_IsWriting ? m_WriteData : m_ReadData; }
	size_t Size() const { return m_Offset; }

	template<typename T>
	void Serialize( T& value )
	{
		Serialize_Impl( value );
	}

	template<typename T>
	void Serialize( T&& value )
	{
		EnsureMsg( m_IsWriting, "Serializing rvalue when reading" );

		T lvalue = value;
		Serialize( lvalue );
	}

private:
	//--------------------------------------------------- SERIALIZE IMPLEMENTATIONS (generic)
	template<typename T>
	void Serialize_Impl( T& value )
	{
		size_t size = sizeof( T );
		EnsureSize( m_Offset + size );

		if ( IsWriting() )
		{
			memcpy( m_WriteData + m_Offset, &value, size );
		}
		else
		{
			memcpy( &value, m_ReadData + m_Offset, size );
		}

		m_Offset += size;
	}
	//--------------------------------------------------- SERIALIZE IMPLEMENTATIONS (specific)
	/**	STRINGS
	*******************************************************************************/
	void Serialize_Impl( std::string& str )
	{
		if ( IsWriting() )
		{
			const char* cString = str.c_str();
			Serialize_Impl( cString );
		}
		else
		{
			const char* cString;
			Serialize_Impl( cString );
			str = cString;
		}
	}
	void Serialize_Impl( CName& name )
	{
		if ( IsWriting() )
		{
			const char* cString = name.c_str();
			Serialize_Impl( cString );
		}
		else
		{
			const char* cString;
			Serialize_Impl( cString );
			name = cString;
		}
	}
	void Serialize_Impl( const char*& str )
	{
		int len = strlen( m_IsWriting ? str : m_ReadData + m_Offset ) + 1;
		EnsureSize( m_Offset + len );

		if ( m_IsWriting )
		{
			memcpy( m_WriteData + m_Offset, str, len );
			m_Offset += len;
		}
		else
		{
			str = ( m_ReadData + m_Offset );
			m_Offset += len;
		}
	}

	void Serialize_Impl( CPath& path )
	{
		if ( m_IsWriting )
		{
			std::string uri = path.ToURI();
			const char* cString = uri.c_str();

			Serialize_Impl( cString );
		}
		else
		{
			const char* cString;
			Serialize_Impl( cString );

			path = cString;
		}
	}

	//--------------------------------------------------- Pointers
	void Serialize_Impl( CActor *& value );
	void Serialize_Impl( CComponent *& value );
	//---------------------------------------------------

	void EnsureSize( size_t size )
	{
		if ( m_IsWriting )
		{
			while ( size > m_Size )
			{
				m_Size *= 2;
				char* newData = new char[m_Size];
				memcpy( newData, m_WriteData, m_Offset );

				delete[] m_WriteData;
				m_WriteData = newData;
			}
		}
		else
			Ensure( size <= m_Size );
	}

	const bool m_IsWriting = false;
	char* m_WriteData = nullptr;
	const char* m_ReadData = nullptr;
	size_t m_Offset = 0;
	size_t m_Size = 0;

	const CScene* const m_Scene = nullptr;
};