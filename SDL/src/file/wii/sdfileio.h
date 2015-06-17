/****************************************************************************
* Generic File I/O for VisualBoyAdvance
*
* Currently only supports SD
****************************************************************************/
#ifndef __SDFILEIO__
#define __SDFILEIO__
#include "tff.h"

#define MAXDIRENTRIES 1000

#ifdef __cplusplus
extern "C" {
#endif

    /* Required Functions */
    FIL *gen_fopen( const char *filename, const char *mode );
    int gen_fwrite( const void *buffer, int len, int block, FIL *f );
    int gen_fread( void *buffer, int len, int block, FIL *f );
    void gen_fclose( FIL *f );
    int gen_fseek(FIL *f, int where, int whence);
    int gen_fgetc( FIL *f );
    void SDInit( void );
    int gen_getdir( char *thisdir );
    extern char *direntries[MAXDIRENTRIES];

#ifdef __cplusplus
}
#endif

#endif

