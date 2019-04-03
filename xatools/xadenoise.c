/*
  xaudiopro - luolongzhi xaudiopro lab 
  Copyright (C) 2018 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of xaudiopro, all copyrights are reserved by luolongzhi. 

  filename: xadenoise.c 
  time    : 2018/12/25 18:33 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <memory.h>
#include "libxacodec/llz_wavfmt.h"
#include "libxafilter/llz_resample.h"
#include "libxacodec/llz_denoise.h"


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

/*global option vaule*/
static char  opt_inputfile[256]  = "";
static char  opt_outputfile[256] = "";

static int   opt_type = DENOISE_RNN;

const char *usage =
"\n\n"
"Usage: xadenoise <-i> <inputfile> <-o> <outputfile> [options] \n"
"\n\n"
"See also:\n"
"    --help               for short help on ...\n"
"    --long-help          for a description of all options for ...\n"
"    --license            for the license terms for xaudiopro.\n\n";

const char *default_set =
"\n\n"
"No argument input, run by default settings\n"
"    --type [0]\n"
"\n\n";

const char *short_help =
"\n\n"
"Usage: xadenoise <-i> <inputfile> <-o> <outputfile> [options] ...\n"
"Options:\n"
"    -i <inputfile>       Set input filename\n"
"    -o <outputfile>      Set output filename\n"
"    -t <type>            Set denoise type\n"
"    --help               Show this abbreviated help.\n"
"    --long-help          Show complete help.\n"
"    --license            for the license terms for llzlab.\n"
"\n\n";

const char *long_help =
"\n\n"
"Usage: xadenoise <-i> <inputfile> <-o> <outputfile> [options] ...\n"
"Options:\n"
"    -i <inputfile>       Set input filename\n"
"    -o <outputfile>      Set output filename\n"
"    --help               Show this abbreviated help.\n"
"    --long-help          Show complete help.\n"
"    --license            for the license terms for llzlab.\n"
"    --input <inputfile>  Set input filename\n"
"    --output <inputfile> Set output filename\n"
"    --type <type>        Set denoise type\n"
"\n\n";

const char *license =
"\n\n"
"**************************************  WARN  *******************************************\n"
"*    Please note that the use of this software may require the payment of patent        *\n"
"*    royalties. You need to consider this issue before you start building derivative    *\n"
"*    works. We are not warranting or indemnifying you in any way for patent royalities! *\n"
"*                                                                                       *\n" 
"*                YOU ARE SOLELY RESPONSIBLE FOR YOUR OWN ACTIONS!                       *\n"
"*****************************************************************************************\n"
"\n"
"\n"
"xaudiopro - luolongzhi xaudiopro \n"
"Copyright (C) 2013 luolongzhi (Chengdu, China)\n"
"\n"
"\n"
"This program is part of xaudiopro, all copyrights are reserved by luolongzhi.\n"
"\n";


static void llz_printopt()
{
    LLZ_PRINT("NOTE: configuration is below\n");
    LLZ_PRINT("NOTE: inputfile = %s\n", opt_inputfile);
    LLZ_PRINT("NOTE: outputfile= %s\n", opt_outputfile);
    LLZ_PRINT("NOTE: type = %d\n", opt_type);
}

/**
 * @brief: check the input value valid, should check the valid scope
 *
 * @return: 0 if success, -1 if error
 */
static int llz_checkopt(int argc)
{
    float ratio;

    if(argc < 5) {
        LLZ_PRINT_ERR("FAIL: input and output file should input\n");
        return -1;
    }

    if(strlen(opt_inputfile) == 0 || strlen(opt_outputfile) == 0) {
        LLZ_PRINT_ERR("FAIL: input and output file should input\n");
        return -1;
    }

    if (opt_type != DENOISE_RNN && 
        opt_type != DENOISE_LMS &&
        opt_type != DENOISE_FFT_LMS &&
        opt_type != DENOISE_FFT_LMS_LPF &&
        opt_type != DENOISE_LEARN1 &&
        opt_type != DENOISE_LEARN2) {
        LLZ_PRINT_ERR("FAIL: unsupported denoise type \n");
        return -1;
    }

    LLZ_PRINT("SUCC: check option ok\n");
    return 0;
}


/**
 * @brief: parse the command line
 *         this is the simple template which will be used by llzlab projects
 *
 * @param:argc
 * @param:argv[]
 *
 * @return: 0 if success, -1 if error(input value maybe not right)
 */
static int denoise_parseopt(int argc, char *argv[])
{
    int ret;
    const char *die_msg = NULL;

    while (1) {
        static char * const     short_options = "hHLi:o:t:";  
        static struct option    long_options[] = 
                                {
                                    { "help"       , 0, 0, 'h'}, 
                                    { "long-help"  , 0, 0, 'H'},
                                    { "license"    , 0, 0, 'L'},
                                    { "input"      , 1, 0, 'i'},                 
                                    { "output"     , 1, 0, 'o'},                 
                                    { "type"       , 1, 0, 't'},        
                                    {0             , 0, 0,  0},
                                };
        int c = -1;
        int option_index = 0;

        c = getopt_long(argc, argv, 
                        short_options, long_options, &option_index);

        if (c == -1) {
            break;
        }

        if (!c) {
            die_msg = usage;
            break;
        }

        switch (c) {
            case 'h': {
                          die_msg = short_help;
                          break;
                      }

            case 'H': {
                          die_msg = long_help;
                          break;
                      }
                      
            case 'L': {
                          die_msg = license;
                          break;
                      }

            case 'i': {
                          if (sscanf(optarg, "%s", opt_inputfile) > 0) {
                              LLZ_PRINT("SUCC: inputfile is %s\n", opt_inputfile);
                          }else {
                              LLZ_PRINT_ERR("FAIL: no inputfile\n");
                          }
                          break;
                      }

            case 'o': {
                          if (sscanf(optarg, "%s", opt_outputfile) > 0) {
                              LLZ_PRINT("SUCC: outputfile is %s\n", opt_outputfile);
                          }else {
                              LLZ_PRINT_ERR("FAIL: no outputfile\n");
                          }
                          break;
                      }

            case 't': {
                          unsigned int i;
                          if (sscanf(optarg, "%u", &i) > 0) {
                              opt_type = (int)i;
                              LLZ_PRINT("SUCC: set denoise type = %i\n", opt_type);
                          }
                          break;
                      }

            case '?':
            default:
                      die_msg = usage;
                      break;
        }
    }

    if(die_msg) {
        LLZ_PRINT("%s\n", die_msg);
        goto fail;
    }

    /*check the input validity*/
    ret = llz_checkopt(argc);
    if(ret) {
        die_msg = usage;
        LLZ_PRINT("%s\n", die_msg);
        goto fail;
    }

    /*print the settings*/
    llz_printopt();

    return 0;
fail:
    return -1;
}



#if 0
int main()
{
    printf("22222222222222\n");

    return 0;

}


#else

int main(int argc, char *argv[])
{
    int ret;
    int frame_index = 0;

	FILE  * destfile;
	FILE  * sourcefile;
	llz_wavfmt_t fmt;
	llz_wavfmt_t fmt1;
    int samplerate_in;
    int samplerate_out;

    int is_first         = 1;
    int is_last          = 0;
    int in_len_bytes     = 0;
    int out_len_bytes;
    int read_len         = 0;
    int write_total_size = 0;

    uintptr_t h_denoise;

	short wavsamples_in[2*LLZ_RS_FRAMELEN_RNN_MAX]={0};
	/*short wavsamples_out[LLZ_RS_FRAMELEN_RNN_MAX];*/
	short wavsamples_out[2*(141120+8192)]={0}; //big array
    unsigned char * p_wavin  = (unsigned char *)wavsamples_in;
    unsigned char * p_wavout = (unsigned char *)wavsamples_out;

    ret = denoise_parseopt(argc, argv);
    if(ret) return -1;

    if ((destfile = fopen(opt_outputfile, "w+b")) == NULL) {
		LLZ_PRINT("error##file:output file can not be opened\n");
		return 0; 
	}                         

	if ((sourcefile = fopen(opt_inputfile, "rb")) == NULL) {
		LLZ_PRINT("error##file:input file can not be opened;\n");
		return 0; 
    }

    LLZ_PRINT("INFO: begin");
    fmt = llz_wavfmt_readheader(sourcefile);
    /*printf("format: %d\n", fmt.format);*/
    /*printf("info##chn: %d\n", fmt.channels);*/
    /*printf("info##samplerate: %d\n", fmt.samplerate);*/
    /*printf("info##block_align: %d\n", fmt.block_align);*/
    samplerate_in = fmt.samplerate;
    fseek(sourcefile,44,0);

    h_denoise = llz_denoise_init(opt_type, fmt.channels, fmt.samplerate, 1, 0.6);

    fmt1.format = 0x0001;
    fmt1.channels = fmt.channels;
    fmt1.samplerate = fmt.samplerate;
    fmt1.bytes_per_sample = 2;
    fmt1.block_align = 2*fmt.channels;
    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt1, destfile);

    in_len_bytes = llz_denoise_framelen_bytes(h_denoise);

    LLZ_PRINT("INFO: file_size=%d\n", fmt.data_size);
    LLZ_PRINT("INFO: frame_len=%d\n", in_len_bytes);

    while(1) {
        if(is_last)
            break;

        memset(p_wavin, 0, in_len_bytes);
        read_len = fread(p_wavin, 1, in_len_bytes, sourcefile);
        if(read_len < in_len_bytes) {
            is_last = 1;
        }

        llz_denoise(h_denoise, p_wavin, in_len_bytes, p_wavout, &out_len_bytes);
        /*printf("111111111=%d\n", out_len_bytes);*/
        /*printf("--------> %d, %d, %d, %d\n", ((short *)p_wavin)[16], ((short *)p_wavout)[16], ((short *)p_wavin)[0], ((short *)p_wavout)[0]);*/

        frame_index++;
        LLZ_PRINT("INFO: frame_index=%d\n", frame_index);

        if (out_len_bytes == 0)
            continue;

        if (is_first) {
            int offset;

            offset = llz_denoise_delay_offset(h_denoise);
            /*printf("delay offset=%d\n", offset);*/
            fwrite(p_wavout+offset, 1, out_len_bytes-offset, destfile);
            is_first = 0;
            write_total_size += out_len_bytes-offset;
        } else {
            if (is_last) {
                int offset;
                int diff;

                offset = llz_denoise_delay_offset(h_denoise);
                diff = out_len_bytes - read_len;

                fwrite(p_wavout, 1, read_len, destfile);
                if (diff > offset) {
                    fwrite(p_wavout+read_len, 1, offset, destfile);
                    write_total_size += read_len+offset;
                } else {
                    fwrite(p_wavout+read_len, 1, diff, destfile);
                    write_total_size += read_len+diff;

                    memset(p_wavin, 0, in_len_bytes);
                    llz_denoise(h_denoise, p_wavin, in_len_bytes, p_wavout, &out_len_bytes);
                    fwrite(p_wavout, 1, (offset-diff), destfile);
                    write_total_size += (offset-diff);
                }
            } else {
                fwrite(p_wavout, 1, out_len_bytes, destfile);
                write_total_size += out_len_bytes;
            }
        }

        LLZ_PRINT("INFO: progress=%d\n", (int)(write_total_size*100/fmt.data_size));
    }

    fmt1.data_size = write_total_size / fmt1.block_align;
    /*printf("the file total size=%d\n", write_total_size);*/
    /*printf("the file block_align =%d\n", fmt1.block_align);*/
    /*printf("the file data size=%d\n", fmt1.data_size);*/

    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt1, destfile);

    llz_denoise_uninit(h_denoise);
    fclose(sourcefile);
    fclose(destfile);

    LLZ_PRINT("info##end");

    return 0;
}

#endif
