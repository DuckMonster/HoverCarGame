#include "stdafx.h"
#include "ShaderUtils.h"

#include <fstream>
using namespace std;

std::string ShaderUtils::ReadFile( const char* filePath )
{
	static char buffer[1024];

	// Open file
	ifstream fileStream( filePath, ifstream::binary );
	if (!fileStream.is_open( ))
		return "";

	// Get file length
	fileStream.seekg( ios::end );
	size_t fileLen = fileStream.tellg( );
	fileStream.seekg( ios::beg );

	// Read whole file
	fileStream.read( buffer, fileLen );
	buffer[fileLen + 1] = '\0';

	return std::string( buffer );
}

GLuint ShaderUtils::CreateShaderProgram( const char* vertSrc, const char* fragSrc )
{
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

	// Clean up
	/*glDetachShader( program, vert );
	glDetachShader( program, frag );
	glDeleteShader( vert );
	glDeleteShader( frag );*/

	return program;
}
