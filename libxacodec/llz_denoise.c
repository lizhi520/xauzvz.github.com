/*
  xaudiopro - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_denoise.c 
  time    : 2018/12/26 18:33 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#include "libxautil/llz_print.h"
#include "libxafilter/llz_resample.h"
#include "libxafilter/llz_mixer.h"
#include "llz_denoise.h"
#include "libxaext/librnnoise/rnnoise.h"

//this is the rnn frame size
#define FRAME_SIZE 480

typedef struct _llz_denoise_t {
    int type;

    int channel;
    int sample_rate;

    //rnn noise 
    uintptr_t h_resample[2][2];
    unsigned char *inbuf[2];
    unsigned char *outbuf[2];
    unsigned char *rnn_inbuf[2];
    unsigned char *rnn_outbuf[2];
    DenoiseState *rnn_st;


} llz_denoise_t;


uintptr_t llz_denoise_init(int type, int channel, int sample_rate)
{
    llz_denoise_t *f = NULL;

    f = (llz_denoise_t *)malloc(sizeof(llz_denoise_t));

    f->type = type;

    f->channel = channel;
    f->sample_rate = sample_rate;

    f->rnn_st = rnnoise_create();
    f->inbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->inbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->outbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->outbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->rnn_inbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->rnn_inbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->rnn_outbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);
    f->rnn_outbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2);

    if (type == DENOISE_RNN) {
        if (sample_rate != 48000 &&
            sample_rate != 44100 &&
            sample_rate != 32000 &&
            sample_rate != 16000) {

            LLZ_PRINT_ERR("FAIL: unsupported sample rate\n");

            return 0;
        }

        switch (sample_rate) {
            case 48000: 
                f->h_resample[0][0] = f->h_resample[0][1] = 0;
                f->h_resample[1][0] = f->h_resample[1][1] = 0;
                break;
            case 44100: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(160, 147, 1.0, BLACKMAN);
                f->h_resample[0][1] = llz_resample_filter_rnn_init(147, 160, 1.0, BLACKMAN);
                f->h_resample[1][0] = llz_resample_filter_rnn_init(160, 147, 1.0, BLACKMAN);
                f->h_resample[1][1] = llz_resample_filter_rnn_init(147, 160, 1.0, BLACKMAN);
                break;
            case 32000: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(3, 2, 1.0, BLACKMAN);
                f->h_resample[0][1] = llz_resample_filter_rnn_init(2, 3, 1.0, BLACKMAN);
                f->h_resample[1][0] = llz_resample_filter_rnn_init(3, 2, 1.0, BLACKMAN);
                f->h_resample[1][1] = llz_resample_filter_rnn_init(2, 3, 1.0, BLACKMAN);
                break;
            case 16000: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(3, 1, 1.0, BLACKMAN); 
                f->h_resample[0][1] = llz_resample_filter_rnn_init(1, 3, 1.0, BLACKMAN); 
                f->h_resample[1][0] = llz_resample_filter_rnn_init(3, 1, 1.0, BLACKMAN); 
                f->h_resample[1][1] = llz_resample_filter_rnn_init(1, 3, 1.0, BLACKMAN); 
                break;
            default:
                LLZ_PRINT_ERR("FAIL: error sample rate\n");
                return 0;
        }
    }

    return (uintptr_t)f;
}

void llz_denoise_uninit(uintptr_t handle) 
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    if (f) {
        rnnoise_destroy(f->rnn_st);
        free(f);
        f = NULL;
    }
}

int llz_denoise_framelen_bytes(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    return llz_get_resample_framelen_bytes(f->h_resample[0][0]);

}


static int do_rnn_denoise(DenoiseState *st, unsigned char *inbuf, unsigned char *outbuf, int bytes_len)
{
    int frame_len;
    int loop;
    float x[FRAME_SIZE];
    short *tmp, *out;
    int i, j;

    frame_len = bytes_len >> 1;
    loop = frame_len / FRAME_SIZE;

    /*printf("==>fram_len=%d, loop= %d\n", fram_len, loop);*/

    for (i = 0; i < loop; i++) {
        tmp = inbuf+i*FRAME_SIZE*2;

        for (j = 0; j < FRAME_SIZE; j++) x[j] = tmp[j];

        rnnoise_process_frame(st, x, x);

        out = outbuf+i*FRAME_SIZE*2;
        for (j = 0; j < FRAME_SIZE; j++) out[j] = x[j];
    }

    /*memcpy(outbuf, inbuf, bytes_len);*/
}

int llz_denoise_first_out_offset(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;
    float offset_up_resample;
    float offset_down_resample;
    int offset;

    offset_up_resample = llz_resample_get_first_out_offset(f->h_resample[0][0]);
    offset_down_resample = llz_resample_get_first_out_offset(f->h_resample[0][1]);

    offset = (int)((offset_up_resample + offset_down_resample + FRAME_SIZE/3)*2);

    if (offset % 2 != 0)
        offset += 1;

    return offset;
}

int llz_denoise(uintptr_t handle, unsigned char *inbuf, int inlen, unsigned char *outbuf, int *outlen) 
{
    llz_denoise_t *f = (llz_denoise_t *)handle;
    int frame_len, out_len_bytes, out_frame_len;
    int out_size;


    int i;

    frame_len = inlen /(2*f->channel);
    /*printf("--frame_len=%d\n", frame_len);*/

    if (f->channel == 1) {
        llz_resample(f->h_resample[0][0], inbuf, inlen, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise(f->rnn_st, f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, outbuf, &out_size);

        *outlen = out_size;
    } else {
        llz_mixer_stereo_left(inbuf, frame_len, f->inbuf[0], &out_frame_len);
        llz_resample(f->h_resample[0][0], f->inbuf[0], inlen>>1, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise(f->rnn_st, f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, f->outbuf[0], &out_size);

        llz_mixer_stereo_right(inbuf, frame_len, f->inbuf[1], &out_frame_len);
        llz_resample(f->h_resample[1][0], f->inbuf[1], inlen>>1, f->rnn_inbuf[1], &out_len_bytes);
        do_rnn_denoise(f->rnn_st, f->rnn_inbuf[1], f->rnn_outbuf[1], out_len_bytes);
        llz_resample(f->h_resample[1][1], f->rnn_outbuf[1], out_len_bytes, f->outbuf[1], &out_size);

        llz_mixer_lr2stereo(f->outbuf[0], f->outbuf[1], out_size>>1, outbuf, &out_size);
    }

    return 0;
}
