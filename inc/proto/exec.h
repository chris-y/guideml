#pragma once

#include <libgen.h>
#include <malloc.h>
#include <string.h>

#include <OS.h>

#include <exec/types.h>

enum {
	MEMF_PUBLIC = B_FULL_LOCK,
	MEMF_ANY = 0, // Chip-RAM doesn't exist on Haiku
	MEMF_CLEAR = 0x100,
};

#define MEM_BLOCKSIZE B_PAGE_SIZE

static inline void* AllocVec(ULONG byteSize, ULONG attributes)
{
	void* addr = malloc(byteSize);
	if (attributes & MEMF_CLEAR) {
		memset(addr, 0, byteSize);
	}
	return addr;
}

static inline void* AllocVecAligned(ULONG byteSize, ULONG attributes, ULONG alignment, ULONG padding)
{
	void* addr = memalign(alignment, byteSize + padding);
	if (attributes & MEMF_CLEAR) {
		memset(addr, 0, byteSize + padding);
	}
	return addr;
}

static inline void FreeVec(void* area)
{
	free(area);
}

static inline void* AllocMem(ULONG byteSize, __attribute__((unused)) ULONG attributes)
{
	return calloc(1, byteSize);
}

static inline void FreeMem(void* address, __attribute__((unused)) int size)
{
	free(address);
}

static inline void CopyMem(const void* src, APTR dest, LONG size)
{
	memcpy(dest, src, size);
}

static inline void CopyMemQuick(APTR src, APTR dest, LONG size)
{
	memcpy(dest, src, size);
}

static inline const char* FilePart(char* source)
{
	return basename(source);
}

#ifdef __cplusplus
extern "C" {
#endif

APTR CreatePool(int, int, int);
void* AllocPooled(APTR pool, int size);
void DeletePool(APTR pool);

#ifdef __cplusplus
};
#endif
