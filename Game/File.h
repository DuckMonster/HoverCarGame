#pragma once
namespace File
{
	bool ReadFile( const char* filePath, std::string& outString );
	std::string GetFileFromPath( const std::string& fullPath );
}