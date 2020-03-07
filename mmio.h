#ifndef _MMIO_H_
#define _MMIO_H_

#include <stdio.h>
#include <stdlib.h>
#include "mmtypes.h"

#define _mm_boundscheck(v,a,b)  (v > b) ? b : ((v < a) ? a : v)

#ifdef MM_BIG_ENDIAN

#define _mm_HI_SLONG(x) ((SLONG *)&x)
#define _mm_LO_SLONG(x) ((SLONG *)&x + 1)
#define _mm_HI_ULONG(x) ((ULONG *)&x)
#define _mm_LO_ULONG(x) ((ULONG *)&x + 1)

#else

#define _mm_HI_SLONG(x) ((SLONG *)&x + 1)
#define _mm_LO_SLONG(x) ((SLONG *)&x)
#define _mm_HI_ULONG(x) ((ULONG *)&x + 1)
#define _mm_LO_ULONG(x) ((ULONG *)&x)

#endif

#ifdef __cplusplus
extern "C" {
#endif


// LOG.C Prototypes
// ================

#define LOG_SILENT   0
#define LOG_VERBOSE  1

extern int  log_init(const CHAR *logfile, BOOL val);
extern void log_exit(void);
extern void log_verbose(void);
extern void log_silent(void);
extern void log_print(const CHAR *fmt, ... );
extern void log_printv(const CHAR *fmt, ... );

extern void (*printlog)(CHAR *fmt, ... );


#ifdef __WATCOMC__
#pragma aux log_init    parm nomemory modify nomemory;
#pragma aux log_exit    parm nomemory modify nomemory;
#pragma aux log_verbose parm nomemory modify nomemory;
#pragma aux log_silent  parm nomemory modify nomemory;
#pragma aux printlog    parm nomemory modify nomemory;
#pragma aux printlogv   parm nomemory modify nomemory;
#endif


// MikMod's new error handling routines
// ====================================

// Generic error codes.

enum
{   MMERR_INVALID_PARAMS = 1,
    MMERR_OPENING_FILE,
    MMERR_UNSUPPORTED_FILE,
    MMERR_OUT_OF_MEMORY,
    MMERR_END_OF_FILE,
    MMERR_DISK_FULL,
    MMERR_DETECTING_DEVICE,
    MMERR_INITIALIZING_DRIVER
};


extern void   _mmerr_sethandler(void (*func)(int, CHAR *));
extern void   _mmerr_set(int err_is_human, CHAR *human_is_err);
extern int    _mmerr_getinteger(void);
extern CHAR  *_mmerr_getstring(void);
extern void   _mmerr_handler(void);

typedef struct MMSTREAM

// Mikmod customized module file pointer structure.
// Contains various data that may be required for a module.

{   FILE   *fp;      // File pointer
    UBYTE  *dp;

    long   iobase;       // base seek position within the file
    long   seekpos;      // used in data(mem) streaming mode only.
} MMSTREAM;

extern MMSTREAM *_mmfile_createfp(FILE *fp, int iobase);
extern MMSTREAM *_mmfile_createmem(void *data, int iobase);


// Memory allocation with error handling - MMALLOC.C
// =================================================

extern void *_mm_malloc(size_t size);
extern void *_mm_calloc(size_t nitems, size_t size);
extern void *_mm_realloc(void *old_blk, size_t size);
extern void *_mm_recalloc(void *old_blk, size_t nitems, size_t size);


extern void _mm_RegisterErrorHandler(void (*proc)(int, CHAR *));
extern BOOL _mm_FileExists(CHAR *fname);

extern void StringWrite(CHAR *s, MMSTREAM *fp);
extern CHAR *StringRead(MMSTREAM *fp);


//  MikMod/DivEnt style file input / output -
//    Solves several portability issues.
//    Notably little vs. big endian machine complications.

//#define _mm_write_SBYTE(x,y)    fputc((int)x,y)
//#define _mm_write_UBYTE(x,y)    fputc((int)x,y)

//#define _mm_read_SBYTE(x) (SBYTE)fgetc(x)
//#define _mm_read_UBYTE(x) (UBYTE)fgetc(x)

//#define _mm_write_SBYTES(x,y,z)  fwrite((void *)x,1,y,z)
//#define _mm_write_UBYTES(x,y,z)  fwrite((void *)x,1,y,z)
//#define _mm_read_SBYTES(x,y,z)   fread((void *)x,1,y,z)
//#define _mm_read_UBYTES(x,y,z)   fread((void *)x,1,y,z)

#define _mm_rewind(x) _mm_fseek(x,0,SEEK_SET)

extern int      _mm_fseek(MMSTREAM *stream, long offset, int whence);
extern long     _mm_ftell(MMSTREAM *stream);
extern BOOL     _mm_feof(MMSTREAM *stream);
extern MMSTREAM *_mm_fopen(const CHAR *fname, const CHAR *attrib);
extern void     _mm_fclose(MMSTREAM *mmfile);

extern long     _mm_flength(FILE *stream);
extern void     _mm_fputs(MMSTREAM *fp, CHAR *data);
extern BOOL     _mm_copyfile(FILE *fpi, FILE *fpo, ULONG len);
extern void     _mm_write_string(CHAR *data, MMSTREAM *fp);
extern int      _mm_read_string (CHAR *buffer, int number, MMSTREAM *fp);


extern SBYTE _mm_read_SBYTE (MMSTREAM *fp);
extern UBYTE _mm_read_UBYTE (MMSTREAM *fp);

extern SWORD _mm_read_M_SWORD (MMSTREAM *fp);
extern SWORD _mm_read_I_SWORD (MMSTREAM *fp);

extern UWORD _mm_read_M_UWORD (MMSTREAM *fp);
extern UWORD _mm_read_I_UWORD (MMSTREAM *fp);

extern SLONG _mm_read_M_SLONG (MMSTREAM *fp);
extern SLONG _mm_read_I_SLONG (MMSTREAM *fp);

extern ULONG _mm_read_M_ULONG (MMSTREAM *fp);
extern ULONG _mm_read_I_ULONG (MMSTREAM *fp);


extern int _mm_read_SBYTES    (SBYTE *buffer, int number, MMSTREAM *fp);
extern int _mm_read_UBYTES    (UBYTE *buffer, int number, MMSTREAM *fp);

extern int _mm_read_M_SWORDS  (SWORD *buffer, int number, MMSTREAM *fp);
extern int _mm_read_I_SWORDS  (SWORD *buffer, int number, MMSTREAM *fp);

extern int _mm_read_M_UWORDS  (UWORD *buffer, int number, MMSTREAM *fp);
extern int _mm_read_I_UWORDS  (UWORD *buffer, int number, MMSTREAM *fp);

extern int _mm_read_M_SLONGS  (SLONG *buffer, int number, MMSTREAM *fp);
extern int _mm_read_I_SLONGS  (SLONG *buffer, int number, MMSTREAM *fp);

extern int _mm_read_M_ULONGS  (ULONG *buffer, int number, MMSTREAM *fp);
extern int _mm_read_I_ULONGS  (ULONG *buffer, int number, MMSTREAM *fp);


extern void _mm_write_SBYTE     (SBYTE data, MMSTREAM *fp);
extern void _mm_write_UBYTE     (UBYTE data, MMSTREAM *fp);

extern void _mm_write_M_SWORD   (SWORD data, MMSTREAM *fp);
extern void _mm_write_I_SWORD   (SWORD data, MMSTREAM *fp);

extern void _mm_write_M_UWORD   (UWORD data, MMSTREAM *fp);
extern void _mm_write_I_UWORD   (UWORD data, MMSTREAM *fp);

extern void _mm_write_M_SLONG   (SLONG data, MMSTREAM *fp);
extern void _mm_write_I_SLONG   (SLONG data, MMSTREAM *fp);

extern void _mm_write_M_ULONG   (ULONG data, MMSTREAM *fp);
extern void _mm_write_I_ULONG   (ULONG data, MMSTREAM *fp);

extern void _mm_write_SBYTES    (SBYTE *data, int number, MMSTREAM *fp);
extern void _mm_write_UBYTES    (UBYTE *data, int number, MMSTREAM *fp);

extern void _mm_write_M_SWORDS  (SWORD *data, int number, MMSTREAM *fp);
extern void _mm_write_I_SWORDS  (SWORD *data, int number, MMSTREAM *fp);

extern void _mm_write_M_UWORDS  (UWORD *data, int number, MMSTREAM *fp);
extern void _mm_write_I_UWORDS  (UWORD *data, int number, MMSTREAM *fp);

extern void _mm_write_M_SLONGS  (SLONG *data, int number, MMSTREAM *fp);
extern void _mm_write_I_SLONGS  (SLONG *data, int number, MMSTREAM *fp);

extern void _mm_write_M_ULONGS  (ULONG *data, int number, MMSTREAM *fp);
extern void _mm_write_I_ULONGS  (ULONG *data, int number, MMSTREAM *fp);

#ifdef __WATCOMC__
#pragma aux _mm_fseek      parm nomemory modify nomemory
#pragma aux _mm_ftell      parm nomemory modify nomemory
#pragma aux _mm_flength    parm nomemory modify nomemory
#pragma aux _mm_fopen      parm nomemory modify nomemory
#pragma aux _mm_fputs      parm nomemory modify nomemory
#pragma aux _mm_copyfile   parm nomemory modify nomemory
#pragma aux _mm_iobase_get parm nomemory modify nomemory
#pragma aux _mm_iobase_set parm nomemory modify nomemory
#pragma aux _mm_iobase_setcur parm nomemory modify nomemory
#pragma aux _mm_iobase_revert parm nomemory modify nomemory
#pragma aux _mm_write_string  parm nomemory modify nomemory
#pragma aux _mm_read_string   parm nomemory modify nomemory

#pragma aux _mm_read_M_SWORD parm nomemory modify nomemory; 
#pragma aux _mm_read_I_SWORD parm nomemory modify nomemory; 
#pragma aux _mm_read_M_UWORD parm nomemory modify nomemory; 
#pragma aux _mm_read_I_UWORD parm nomemory modify nomemory; 
#pragma aux _mm_read_M_SLONG parm nomemory modify nomemory; 
#pragma aux _mm_read_I_SLONG parm nomemory modify nomemory; 
#pragma aux _mm_read_M_ULONG parm nomemory modify nomemory; 
#pragma aux _mm_read_I_ULONG parm nomemory modify nomemory; 

#pragma aux _mm_read_M_SWORDS parm nomemory modify nomemory; 
#pragma aux _mm_read_I_SWORDS parm nomemory modify nomemory; 
#pragma aux _mm_read_M_UWORDS parm nomemory modify nomemory; 
#pragma aux _mm_read_I_UWORDS parm nomemory modify nomemory; 
#pragma aux _mm_read_M_SLONGS parm nomemory modify nomemory; 
#pragma aux _mm_read_I_SLONGS parm nomemory modify nomemory; 
#pragma aux _mm_read_M_ULONGS parm nomemory modify nomemory; 
#pragma aux _mm_read_I_ULONGS parm nomemory modify nomemory; 

#pragma aux _mm_write_M_SWORD parm nomemory modify nomemory; 
#pragma aux _mm_write_I_SWORD parm nomemory modify nomemory; 
#pragma aux _mm_write_M_UWORD parm nomemory modify nomemory; 
#pragma aux _mm_write_I_UWORD parm nomemory modify nomemory; 
#pragma aux _mm_write_M_SLONG parm nomemory modify nomemory; 
#pragma aux _mm_write_I_SLONG parm nomemory modify nomemory; 
#pragma aux _mm_write_M_ULONG parm nomemory modify nomemory; 
#pragma aux _mm_write_I_ULONG parm nomemory modify nomemory; 

#pragma aux _mm_write_M_SWORDS parm nomemory modify nomemory; 
#pragma aux _mm_write_I_SWORDS parm nomemory modify nomemory; 
#pragma aux _mm_write_M_UWORDS parm nomemory modify nomemory; 
#pragma aux _mm_write_I_UWORDS parm nomemory modify nomemory; 
#pragma aux _mm_write_M_SLONGS parm nomemory modify nomemory; 
#pragma aux _mm_write_I_SLONGS parm nomemory modify nomemory; 
#pragma aux _mm_write_M_ULONGS parm nomemory modify nomemory; 
#pragma aux _mm_write_I_ULONGS parm nomemory modify nomemory; 
#endif


#ifndef __WATCOMC__
#ifndef __GNUC__
#ifndef WIN32
#ifndef _MSC_VER
#ifndef __BORLANDC__
extern CHAR *strdup(const CHAR *str);
#endif
#endif
#endif
#endif
#endif

#ifdef __cplusplus
};
#endif

#endif
