/*
  xaudiopro - luolongzhi xaudiopro lab 
  Copyright (C) 2018 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of xaudiopro, all copyrights are reserved by luolongzhi. 

  filename: xa_resample.c 
  time    : 2018/12/25 18:33 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "libxacodec/llz_wavfmt.h"
#include "libxafilter/llz_mixer.h"
#include "xapcm2wav_opt.h"

#define FRAME_LEN 1024

int main(int argc, char *argv[])
{
    int ret;
    int frame_index = 0;

	FILE  * destfile;
	FILE  * sourcefile;
	llz_wavfmt_t fmt;

    int is_last          = 0;
    int in_len_bytes     = 0;
    int out_len_bytes    = 0;
    int read_len         = 0;
    int write_total_size = 0;

	short wavsamples_in[FRAME_LEN];
	short wavsamples_out[FRAME_LEN];
    unsigned char * p_wavin  = (unsigned char *)wavsamples_in;
    unsigned char * p_wavout = (unsigned char *)wavsamples_out;


    ret = pcm2wav_parseopt(argc, argv);
    if(ret) return -1;

    if ((destfile = fopen(opt_outputfile, "w+b")) == NULL) {
		printf("output file can not be opened\n");
		return 0; 
	}                         

	if ((sourcefile = fopen(opt_inputfile, "rb")) == NULL) {
		printf("input file can not be opened;\n");
		return 0; 
    }

    fmt.format = 0x0001;
    fmt.channels = opt_channel;
    fmt.samplerate = opt_samplerate;
    fmt.bytes_per_sample = 2;
    fmt.block_align = 2*opt_channel;

    fseek(sourcefile, 0, 0);
    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt, destfile);

    in_len_bytes = FRAME_LEN * opt_channel;

    while(1) {
        if(is_last)
            break;

        memset(p_wavin, 0, in_len_bytes);
        read_len = fread(p_wavin, 1, in_len_bytes, sourcefile);
        if(read_len < in_len_bytes)
            is_last = 1;

        llz_mixer_pass(p_wavin, in_len_bytes, p_wavout);
        out_len_bytes = in_len_bytes;

        fwrite(p_wavout, 1, out_len_bytes, destfile);
        write_total_size += out_len_bytes;

        frame_index++;
        printf("the frame = [%d]\r", frame_index);
    }

    fmt.data_size = write_total_size/fmt.block_align;
    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt,destfile);

    fclose(sourcefile);
    fclose(destfile);

    return 0;
}
