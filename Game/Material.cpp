#include "stdafx.h"
#include "Material.h"
#include "File.h"
#include "Log.h"
#include "GLUtils.h"

using namespace std;

/**	Constructor
*******************************************************************************/
CMaterial::CMaterial( )
{
}

/**	Destructor
*******************************************************************************/
CMaterial::~CMaterial( )
{
}

/**	Un-load
*******************************************************************************/
void CMaterial::Unload( )
{
	if (m_Program == -1)
		return;

	glDeleteProgram( m_Program );
	m_Program = -1;
}

/**	Load Source
*******************************************************************************/
void CMaterial::LoadSource( const char* vertSrc, const char* fragSrc )
{
	Unload( );
	m_Program = GLUtils::CreateShaderSrc( vertSrc, fragSrc );
}

/**	Load Shader
*******************************************************************************/
void CMaterial::LoadFromFile( const char* path )
{
	string vertPath( path ),
		fragPath( path );

	vertPath += ".vert";
	fragPath += ".frag";

	LoadFromFile( vertPath.c_str( ), fragPath.c_str( ) );
}

/**	Load Shaders
*******************************************************************************/
void CMaterial::LoadFromFile( const char* vertPath, const char* fragPath )
{
	string vertSrc, fragSrc;

	if (!File::ReadFile( vertPath, vertSrc ))
	{
		Print_Log( "Failed to load file \"%s\"", vertPath );
		return;
	}

	if (!File::ReadFile( fragPath, fragSrc ))
	{
		Print_Log( "Failed to load file \"%s\"", fragPath );
		return;
	}

	Print_Log( "Loading shader \"%s\" / \"%s\"", File::GetFileFromPath( vertPath ).c_str( ), File::GetFileFromPath( fragPath ).c_str( ) );

	LoadSource( vertSrc.c_str( ), fragSrc.c_str( ) );
}

/**	Load Texture
*******************************************************************************/
void CMaterial::LoadTexture( const char* path )
{
	GLUtils::CreateTextureFromFile( path, m_Texture );
}