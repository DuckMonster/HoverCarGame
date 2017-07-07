#pragma once
namespace ShaderUtils
{
	std::string ReadFile( const char* filePath );
	GLuint CreateShaderProgram( const char* vertSrc, const char* fragSrc );
}