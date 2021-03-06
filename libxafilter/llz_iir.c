/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_iir.c 
  time    : 2013/03/18 18:38 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "llz_iir.h"

typedef struct _llz_iir_filter_t {
    int   M;     //number of acof, pole point
    int   N;     //number of bcof, zero point

    float *a;    //pole point cof
    float *b;    //zero point cof

    float *y;    //buffer the last M output signals
    float *x;    //buffer the last N input signals
} llz_iir_filter_t;


/*
    iir filter
    y(n) = sum{ b[k]*x[n-k] } - sum{ a[k]*y[n-k] }
            k = 0...N             k = 1...M

*/

/*use matlab the generate the a and b coffienets*/
uintptr_t llz_iir_filter_init(int M, float *a, int N, float *b)
{
    int i;
    llz_iir_filter_t *flt = NULL;

    flt = (llz_iir_filter_t *)malloc(sizeof(llz_iir_filter_t));
    memset(flt, 0, sizeof(llz_iir_filter_t));

    flt->M = M;
    flt->N = N;

    flt->a = (float *)malloc((M+1) * sizeof(float));
    memset(flt->a, 0, (M+1)*sizeof(float));
    for (i = 0; i <= M; i++)
        flt->a[i] = a[i];     // a[0] always equalt to 1

    flt->b = (float *)malloc((N+1) * sizeof(float));
    memset(flt->b, 0, (N+1)*sizeof(float));
    if (b) {
        for (i = 0; i <= N; i++) {
            flt->b[i] = b[i];
        }
    }

    flt->y = (float *)malloc((M+1) * sizeof(float));
    flt->x = (float *)malloc((N+1) * sizeof(float));
    memset(flt->y, 0, (M+1)*sizeof(float));
    memset(flt->x, 0, (N+1)*sizeof(float));

    return (uintptr_t)flt;

}


void      llz_iir_filter_uninit(uintptr_t handle)
{
    llz_iir_filter_t *flt = (llz_iir_filter_t *)handle;

    if (flt) {
        if (flt->a) {
            free(flt->a);
            flt->a = NULL;
        }

        if (flt->b) {
            free(flt->b);
            flt->b = NULL;
        }

        if (flt->x) {
            free(flt->x);
            flt->x = NULL;
        }

        if (flt->y) {
            free(flt->y);
            flt->y = NULL;
        }

        free(flt);
        flt = NULL;
    }

}


static float iir_flt(llz_iir_filter_t *flt, float x_in)
{
    int i;
    int k;
    float y_out;
    int N;
    int M;

    N = flt->N;
    M = flt->M;

    y_out = 0.;
    /*move x and y buf*/
    /*memcpy(flt->x, &(flt->x[1]), N*sizeof(float));*/
    for (i = 0; i < N; i++)
        flt->x[i] = flt->x[i+1];
    flt->x[N] = x_in;
    /*memcpy(flt->y, &(flt->y[1]), M*sizeof(float));*/
    for (i = 0; i < M; i++)
        flt->y[i] = flt->y[i+1];

    for (k = 0; k <= N; k++)
        y_out += flt->b[k] * flt->x[N-k];
    for (k = 1; k <= M; k++)
        y_out -= flt->a[k] * flt->y[M-k];

    flt->y[M] = y_out;

    return y_out;
}


/*delay is flt->N points*/
int llz_iir_filter(uintptr_t handle, float *x, float *y, int frame_len)
{
    int i;
    llz_iir_filter_t *flt = (llz_iir_filter_t *)handle;

    for(i = 0 ; i < frame_len; i++)
        y[i] = iir_flt(flt, x[i]);

    return frame_len;
}

int llz_iir_filter_flush(uintptr_t handle, float *y)
{
    int i;
    llz_iir_filter_t *flt = (llz_iir_filter_t *)handle;

    for (i = 0; i < flt->N; i++)
        y[i] = iir_flt(flt, 0.0);

    return flt->N;
}



