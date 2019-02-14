/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_parseopt.c 
  time    : 2012/07/09 23:38 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "libxacodec/llz_wavfmt.h"
#include "libxacodec/llz_denoise.h"
#include "xadenoise_opt.h"

/*global option vaule*/
char  opt_inputfile[256]  = "";
char  opt_outputfile[256] = "";

int   opt_type = DENOISE_RNN;

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
        opt_type != DENOISE_LMS) {
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
int denoise_parseopt(int argc, char *argv[])
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
