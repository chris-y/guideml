#define MIBENUM_SYSTEM 0
#define MIBENUM_UTF_32 1
#define MIBENUM_UTF_8  2

LONG ConvertTagList(const char* src, int32 srclen, char* dest, int32 destlen, int inset, int outset, int32* context);
// Defined in ACE.cpp because
