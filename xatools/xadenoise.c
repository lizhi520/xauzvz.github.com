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
#include <math.h>
#include <memory.h>
#include "libxacodec/llz_wavfmt.h"
#include "libxafilter/llz_resample.h"
#include "libxacodec/llz_denoise.h"
#include "xadenoise_opt.h"

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

    int is_last          = 0;
    int in_len_bytes     = 0;
    int out_len_bytes;
    int read_len         = 0;
    int write_total_size = 0;

    uintptr_t h_denoise;

	short wavsamples_in[LLZ_RS_FRAMELEN_MAX];
	short wavsamples_out[LLZ_RS_FRAMELEN_MAX];
    unsigned char * p_wavin  = (unsigned char *)wavsamples_in;
    unsigned char * p_wavout = (unsigned char *)wavsamples_out;


    ret = denoise_parseopt(argc, argv);
    if(ret) return -1;

    if ((destfile = fopen(opt_outputfile, "w+b")) == NULL) {
		printf("output file can not be opened\n");
		return 0; 
	}                         

	if ((sourcefile = fopen(opt_inputfile, "rb")) == NULL) {
		printf("input file can not be opened;\n");
		return 0; 
    }

    fmt = llz_wavfmt_readheader(sourcefile);
    printf("format: %d\n", fmt.format);
    printf("chn: %d\n", fmt.channels);
    printf("samplerate: %d\n", fmt.samplerate);
    printf("block_align: %d\n", fmt.block_align);
    samplerate_in = fmt.samplerate;
    fseek(sourcefile,44,0);

    h_denoise = llz_denoise_init(opt_type, fmt.channels, fmt.samplerate);

    fmt1.format = 0x0001;
    fmt1.channels = fmt.channels;
    fmt1.samplerate = 3*fmt.samplerate;
    fmt1.bytes_per_sample = 2;
    fmt1.block_align = 2*fmt.samplerate;
    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt1, destfile);

    printf("2222\n");
    in_len_bytes = llz_denoise_framelen_bytes(h_denoise);
    printf("33\n");

    while(1) {
        if(is_last)
            break;

        memset(p_wavin, 0, in_len_bytes);
        read_len = fread(p_wavin, 1, in_len_bytes, sourcefile);
        if(read_len < in_len_bytes)
            is_last = 1;

        llz_denoise(h_denoise, p_wavin, in_len_bytes, p_wavout, &out_len_bytes);

        fwrite(p_wavout, 1, out_len_bytes, destfile);
        write_total_size += out_len_bytes;

        frame_index++;
        /*printf("the frame = [%d]\r", frame_index);*/
    }

    fmt1.data_size = write_total_size/fmt1.block_align;
    fseek(destfile, 0, SEEK_SET);
    llz_wavfmt_writeheader(fmt1, destfile);

    llz_denoise_uninit(h_denoise);
    fclose(sourcefile);
    fclose(destfile);

    return 0;
}
