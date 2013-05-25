#pragma once
#include <stdio.h>
#include <stdarg.h>

#define RAWFMTFUNC_STRING 1
#define RAWFMTFUNC_COUNT 2

static inline void NewRawDoFmt(const char* str, int opcode, char* out, ...)
{
	va_list args;
	va_start(args, out);
	if (opcode == RAWFMTFUNC_COUNT) {
		unsigned long count = vsnprintf(NULL, 0, str, args);
		*(unsigned long*)out = count;
	} else
		vsprintf(out, str, args);
	va_end(args);
}

static inline void RawDoFmt(const char* str, void* args[], int opcode, char* out)
{
	if (opcode == RAWFMTFUNC_COUNT) {
		unsigned long count = snprintf(NULL, 0, str, args[0], args[1], args[2], args[3]);
		*(unsigned long*)out = count;
	} else
		sprintf(out, str, args[0], args[1], args[2], args[3]);
}
