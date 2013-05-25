#pragma once

#include <strings.h>

static inline int Stricmp(const char* a, const char* b)
{
	return strcasecmp(a,b);
}

static inline int Strnicmp(const char* a, const char* b, int len)
{
	return strncasecmp(a,b, len);
}

