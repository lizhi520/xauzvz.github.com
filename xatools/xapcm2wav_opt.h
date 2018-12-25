#ifndef _XA_PCM2WAV_H
#define _XA_PCM2WAV_H


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

extern char  opt_inputfile[] ;
extern char  opt_outputfile[];

extern int   opt_channel;
extern int   opt_samplerate;
extern float opt_gain;

int pcm2wav_parseopt(int argc, char *argv[]);

#endif
