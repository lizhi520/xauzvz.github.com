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


int llz_bgm_extract(uintptr_t handle, unsigned char *buf_in, unsigned char *buf_out, int frame_len_bytes)
{
    llz_bgm_t *f = (llz_bgm_t *)handle;


    return frame_len_bytes;
}
