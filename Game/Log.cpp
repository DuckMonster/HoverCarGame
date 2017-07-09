#include "stdafx.h"
#include <windows.h>

namespace
{
}

void Debug::ConsoleLog( const char * file, long line, const char * format, ... )
{
	//--------------------------------------------------- Trim path to find last file
	const char* filePtr = file;
	int lastSplit = 0;
	int i = 0;

	while (*filePtr)
	{
		if (*filePtr == '\\' || *filePtr == '/')
			lastSplit = i;

		++i;
		++filePtr;
	}

	filePtr = file + lastSplit + 1;
	//---------------------------------------------------

	static char buffer[1024];
	int cursor = 0;

	// File and line header
	cursor += sprintf( buffer, "(%s:%d)> \t", filePtr, line );

	// Message
	va_list args;
	va_start( args, format );
	cursor += _vsnprintf( buffer + cursor, 1024, format, args );
	va_end( args );

	// Newline and null-terminator
	buffer[cursor] = '\n';
	buffer[cursor + 1] = '\0';

	// Print to console
	printf( buffer );
	// Print to output window
	OutputDebugStringA( buffer );
}