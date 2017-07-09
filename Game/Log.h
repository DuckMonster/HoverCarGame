#pragma once
#include <stdarg.h>
#define VERBOSE

namespace Debug
{
	void ConsoleLog( const char* file, long line, const char* format, ... );
}

#if defined(VERBOSE)
#define Print_Log(format, ...) Debug::ConsoleLog( __FILE__, __LINE__, format, __VA_ARGS__ )
#else
#define Print_Log(format, ...)
#endif