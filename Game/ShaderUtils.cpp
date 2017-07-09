#include "stdafx.h"
#include "ShaderUtils.h"
#include <fstream>

using namespace std;

/**	Read File
*******************************************************************************/
bool ShaderUtils::ReadFile( const char* filePath, string& outString )
{
	static char buffer[1024];

	// Open file
	ifstream fileStream( filePath, ifstream::binary );
	if (!fileStream.is_open( ))
		return false;

	// Read whole stream
	fileStream.read( buffer, sizeof( buffer ) );

	// Add null-terminator
	buffer[(size_t)fileStream.gcount( )] = '\0';

	outString = buffer;
	return true;
}

/**	Create Shader From File
*******************************************************************************/
GLuint ShaderUtils::CreateShaderFromFile( const char * path )
{
	Print_Log( "Loading shader \"%s\"", path );

	std::string test;
	ReadFile( "sometext.txt", test );

	std::string vertPath( path ), fragPath( path ),
		vertSrc, fragSrc;

	vertPath += ".vert";
	fragPath += ".frag";

	ReadFile( vertPath.c_str( ), vertSrc );
	ReadFile( fragPath.c_str( ), fragSrc );

	return CreateShaderSrc( vertSrc.c_str( ), fragSrc.c_str( ) );
}

/**	Create Shader From Files
*******************************************************************************/
GLuint ShaderUtils::CreateShaderFromFiles( const char * vertPath, const char * fragPath )
{
	Print_Log( "Loading shaders \"%s\", \"%s\"", vertPath, fragPath );

	std::string vertSrc, fragSrc;

	ReadFile( vertPath, vertSrc );
	ReadFile( fragPath, fragSrc );

	return CreateShaderSrc( vertSrc.c_str( ), fragSrc.c_str( ) );
}

/**	Create Shader From Source
*******************************************************************************/
GLuint ShaderUtils::CreateShaderSrc( const char* vertSrc, const char* fragSrc )
{
	static char logBuffer[1024];

	GLuint program = glCreateProgram( ),
		vert = glCreateShader( GL_VERTEX_SHADER ),
		frag = glCreateShader( GL_FRAGMENT_SHADER );

	glShaderSource( vert, 1, &vertSrc, nullptr );
	glShaderSource( frag, 1, &fragSrc, nullptr );
	glCompileShader( vert );
	glCompileShader( frag );

	glAttachShader( program, vert );
	glAttachShader( program, frag );
	glLinkProgram( program );

	glGetProgramInfoLog( program, 1024, nullptr, logBuffer );
	if (strlen( logBuffer ) > 0)
	{
		Print_Log( "Shader compile log:\n%s", logBuffer );
	}

	// Clean up
	/*glDetachShader( program, vert );
	glDetachShader( program, frag );
	glDeleteShader( vert );
	glDeleteShader( frag );*/

	return program;
}
