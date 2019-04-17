/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_bgm.c 
  time    : 2019/04/16 23:46 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#include <stdio.h>
#include <stdlib.h>
#include "llz_bgm.h"


typedef struct _llz_bgm_t {
    float gain;
} llz_bgm_t;


uintptr_t llz_bgm_init(float gain)
{

    llz_bgm_t *f = NULL;

    f = (llz_bgm_t *)malloc(sizeof(llz_bgm_t));
    memset(f, 0, sizeof(llz_bgm_t));

    f->gain = gain;

    return (uintptr_t)f;
}

void llz_bgm_uninit(uintptr_t handle)
{
    llz_bgm_t *f = (llz_bgm_t *)handle;

    free(f);
    f = NULL;
}


void llz_bgm_extract(uintptr_t handle, short *buf_in, short *buf_out, int frame_len)
{
    llz_bgm_t *f = (llz_bgm_t *)handle;
    short left, right;
    float result;
    int i;

    for (i = 0; i < frame_len; i++) {
        left  = buf_in[i+i];
        right = buf_in[i+i+1];

        result = f->gain * (left - right);

        if (result >= 32767) {
            result = 32767;
        }

        if (result <= -32767) {
            result = -32767;
        }

        buf_out[i+i] = buf_out[i+i+1] = (short)result;
    }

}
