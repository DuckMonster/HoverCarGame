#pragma once
namespace ShaderUtils
{
	bool ReadFile( const char* filePath, std::string& outString );

	GLuint CreateShaderFromFile( const char* path );
	GLuint CreateShaderFromFiles( const char* vertPath, const char* fragPath );
	GLuint CreateShaderSrc( const char* vertSrc, const char* fragSrc );
}