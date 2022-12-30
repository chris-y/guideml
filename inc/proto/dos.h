#pragma once

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <exec/types.h>

#include <dos/rdargs.h>

#define MODE_OLDFILE (addr_t)"r+"
#define MODE_NEWFILE (addr_t)"w"
#define MODE_READWRITE (addr_t)"w+"

#define ACCESS_READ 0

enum {
	OFFSET_CURRENT = SEEK_CUR,
	OFFSET_BEGINING = SEEK_SET,
	OFFSET_BEGINNING = SEEK_SET,
	OFFSET_END = SEEK_END,
};

enum {
	SHARED_LOCK = F_RDLCK
};

enum {
	// Not existing values in Haiku
	ST_ROOT = -1,
	ST_LINKDIR = -2,
	ST_LINKFILE = -3,

	ST_USERDIR = S_IFDIR,
	ST_SOFTLINK = S_IFLNK,
	ST_FILE = S_IFREG,
	ST_PIPEFILE = S_IFIFO,
};

static inline BPTR Open( CONST_STRPTR name, addr_t accessMode )
	/* Open a file for read or write */
{
	FILE* f = fopen(name, (const char*)accessMode);

	// The "OLDFILE" mode can open either r+ (if possible) or r (on readonly FS)
	if (f == NULL && accessMode == MODE_OLDFILE)
		f = fopen(name, "r");
	return (BPTR)f;
}

static inline LONG Close( BPTR file )
	/* Close a file */
{
	return !fclose((FILE*)file);
}

static inline LONG Read( BPTR file, APTR buffer, LONG length )
	/* Read n bytes into a buffer (unbuffered) */
{
	return fread(buffer, 1, length, (FILE*)file);
}

static inline LONG Write( BPTR file, CONST APTR buffer, LONG length )
	/* Write n bytes into a buffer (unbuffered) */
{
	return fwrite(buffer, 1, length, (FILE*)file);
}

static inline LONG FPutC(BPTR file, char c)
{
	return fputc(c, (FILE*)file);
}

static inline int Flush(BPTR file)
{
	return fflush((FILE*)file);
}

static inline off_t Seek(BPTR file, off_t offset, int whence)
{
	fseek((FILE*)file, offset, whence);
	return ftell((FILE*)file);
}

#define FPrintf(x, ...) fprintf((FILE*)x, __VA_ARGS__)
#define Printf printf

static inline LONG FPuts(BPTR file, STRPTR str)
{
	return fputs(str, (FILE*)file);
}

static inline const char* FGets(BPTR f, char* buffer, int len)
{
	return fgets(buffer, len, (FILE*)f);
}

static inline int FGetC(BPTR f)
{
	return fgetc((FILE*)f);
}

static inline void UnGetC(BPTR f, char c)
{
	ungetc(c, (FILE*)f);
}

static inline BPTR Lock(CONST_STRPTR name, int __attribute__((unused)) type)
{
	int fd = open(name, O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	lockf(fd, F_LOCK, 0);

	return (BPTR)fd;
}

static inline int DupLock(int i)
{
	int fd = dup(i);
	lockf(fd, F_LOCK, 0);
	return fd;
}

static inline void UnLock(BPTR fd)
{
	lockf((int)(addr_t)fd, F_ULOCK, 0);
	close((int)(addr_t)fd);
}

static inline BPTR CurrentDir(BPTR fd)
{
	static int cwd = 0;
	int old;
	if (cwd != 0)
		old = cwd;
	else
		old = open(".", O_RDONLY);

	fchdir((int)(addr_t)fd);
	cwd = (int)(addr_t)fd;

	return (BPTR)old;
}

static inline long StrToLong(const char* str, LONG* result)
{
	char* end;
	*result = strtol(str, &end, 10);
	return end - str;
}

static inline void PutStr(const char* str)
{
	puts(str);
}

static inline BPTR Input()
{
	return (BPTR)stdin;
}

static inline BPTR Output()
{
	return (BPTR)stdout;
}

static inline char ToUpper(char c)
{
	return toupper(c);
}

static inline char ToLower(char c)
{
	return tolower(c);
}

enum {
	DOS_FIB,
	DOS_RDARGS
};

struct FileInfoBlock {
	struct stat st;
	DIR* D;
	struct dirent* d;

	bool hasDir;

#define fib_DirEntryType st.st_mode & S_IFMT
#define fib_FileName d->d_name
#define fib_Size st.st_size
#define fib_Protection st.st_mode
};


static inline void* AllocDosObject(int what, void* ptr)
{
	assert(ptr == NULL);
	size_t size = 0;
	switch(what)
	{
		case DOS_FIB:
			size = sizeof(struct FileInfoBlock);
			break;
		case DOS_RDARGS:
			size = sizeof(struct RDArgs);
			break;
		default:
			assert(false);
			return NULL;
	}
	return calloc(size, 1);
}

static inline void FreeDosObject(int type, void* object)
{
	assert(type == DOS_FIB);

	struct FileInfoBlock* fib = (struct FileInfoBlock*)object;

	if (fib->hasDir)
		closedir(fib->D);

	free(fib);
}

static inline int ExamineFH(BPTR file, struct FileInfoBlock* finfo)
{
	int err = fstat(fileno((FILE*)file), &finfo->st);

	if (err != 0)
		return 0;

	// Invert some mode bits so we match AmigaDOS sepentics (protection rather
	// than permission)
	finfo->st.st_mode ^= 0x1ff;
	finfo->hasDir = false;

	return 1;
}

static inline bool Examine(BPTR fd, struct FileInfoBlock* finfo)
{
	if (fstat((int)(addr_t)fd, &finfo->st) != 0)
	{
		return false;
	}

	// Invert some mode bits so we match AmigaDOS semantics (protection rather
	// than permission)
	finfo->st.st_mode ^= 0x1ff;

	if (S_ISDIR(finfo->st.st_mode)) {
		finfo->D = fdopendir((int)(addr_t)fd);
		finfo->hasDir = (finfo->D != NULL);
		return finfo->hasDir;
	} else {
		finfo->hasDir = false;
		return true;
	}
}

static inline bool ExNext(BPTR fd, struct FileInfoBlock* finfo)
{
	assert(fd != NULL);

	finfo->d = readdir(finfo->D);

	if(finfo->d == NULL)
		return false;

	// Save the current dir
	char buffer[PATH_MAX];
	getcwd(buffer, sizeof(buffer));

	// Change dir to the directory being scanned so that d_name is found there
	fchdir(dirfd(finfo->D));

	// Update our stat info
	stat(finfo->d->d_name, &finfo->st);

	// Change back to original directory
	chdir(buffer);

	return true;
}

static inline bool SetProtection(const char* file, int mask)
{
	if (file == NULL)
		return false;
	return !chmod(file, mask ^ 0x1ff);
}
