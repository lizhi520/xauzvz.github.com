#ifndef _LLZ_GETOPT_H
#define _LLZ_GETOPT_H

#include "libxafilter/llz_fir.h"

/*
    Below macro maybe defined in llz_print, if you do not want to use llz_print,
    you can define any other printf functions which you like.  Moreover, if you
    dont want print anything (for the effienece purpose of the program), you 
    can also define null, and the program will print nothing when running.
*/
#ifndef LLZ_PRINT 
//#define LLZ_PRINT(...)       
#define LLZ_PRINT       printf
#define LLZ_PRINT_ERR   LLZ_PRINT
#define LLZ_PRINT_DBG   LLZ_PRINT
#endif


enum{
    LLZ_DECIMATE = 0,
    LLZ_INTERP,
    LLZ_RESAMPLE,
};

extern char  opt_inputfile[] ;
extern char  opt_outputfile[];

extern int   opt_type      ;
extern int   opt_downfactor;
extern int   opt_upfactor  ;
extern float opt_gain      ;


int llz_parseopt(int argc, char *argv[]);

#endif
