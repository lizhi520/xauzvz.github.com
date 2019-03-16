/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_lms.c 
  time    : 2019/03/19 14:14 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "llz_lms.h"


#define N		30
#define M		1
#define W0		0
#define NPT		N+M
#define SF		32768

#define MU		0.08//*32768

#define LMS_TIME	100

typedef	struct _lms_ctx_t {
	float x[LMS_TIME][NPT], d[LMS_TIME][NPT], dk[LMS_TIME], ek[LMS_TIME];
	float w[LMS_TIME][NPT];

	int time;

}lms_ctx_t;

static void lms_update_data_buffer(lms_ctx_t *f, int time);
static float lms_flt_fir(lms_ctx_t *f, int time);

uintptr_t llz_lms_init(int repeat_count)
{
	int i,k;
    lms_ctx_t *f = NULL;

    f = (lms_ctx_t *)malloc(sizeof(lms_ctx_t));
    memset(f, 0, sizeof(lms_ctx_t));


	for(k = 0 ; k < LMS_TIME ; k++)
	{
		for (i = 0 ; i < NPT ; i++)
		{
			f->x[k][i] = 0;
			f->d[k][i] = 0;
			f->w[k][i] = W0;
		}
	}

	f->time = repeat_count;

    return (uintptr_t)f;
}

void llz_lms_uninit(uintptr_t handle)
{
    lms_ctx_t *f = (lms_ctx_t *)handle;

    free(f);
    f = NULL;
}


static void lms_update_data_buffer(lms_ctx_t *f, int time)
{
	int j,k;
	float *x = f->x[time];
	
	for (j = 1; j < N; j++) {
		k = N - j;
		x[k] = x[k-1];
	}
	x[0] = f->dk[time];

	f->x[time][0] = f->d[time][0];
	f->d[time][0] = f->dk[time];
}

static float lms_flt_fir(lms_ctx_t *f, int time)
{
	int i;
	float uek,yk;
	float *x,*w;

	x = f->x[time];
	w = f->w[time];
	
	yk = 0;
	for (i = 0; i < N; i++)
		yk = yk + w[i] * x[i];

	f->ek[time] = f->dk[time] - yk;
	uek = MU * f->ek[time];
	for (i = 0; i < N; i++)
		w[i] += uek * x[i];

	return yk;
}

void llz_lms(uintptr_t h_lms, short *inbuffer, short *outbuffer,int frame_size)
{
	int   k, i;
	float yk, yk1;
    int   time;
    float *dk;
    lms_ctx_t *f = (lms_ctx_t *)h_lms;

	time = f->time;
	dk = f->dk;

    for (k = 0; k < frame_size; k++) {
        yk1 = (float)inbuffer[k];
        yk = yk1 * 0.000030517578125;

        for(i = 0 ; i < time ; i++) {
            dk[i] = yk;

            lms_update_data_buffer(f, i);
            yk = lms_flt_fir(f, i);
        }

        yk1 = SF * yk;

        if (yk1 > 32767)
            yk1 = 32767;
        if (yk1 < -32767)
            yk1 = -32767;

        outbuffer[k] = (short) yk1;
    }

}


