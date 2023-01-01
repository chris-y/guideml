#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <OS.h>

typedef int64_t QUAD;
typedef uint64_t UQUAD;
typedef int32_t LONG;
typedef uint32_t ULONG;
typedef int16_t SHORT;
typedef int16_t WORD;
typedef uint16_t USHORT;
typedef uint16_t UWORD;
typedef int8_t BYTE;
typedef uint8_t UBYTE;
typedef char TEXT;

typedef double DOUBLE;

typedef void VOID;

typedef int8_t* BPTR;

#ifndef STRPTR_TYPEDEF
typedef char* STRPTR;
typedef const char* CONST_STRPTR;
#endif
typedef void* APTR;
typedef intptr_t IPTR;
typedef uintptr_t UIPTR;

typedef unsigned char BOOL;

typedef int WCHAR;

#define CONST const
#define STATIC static
