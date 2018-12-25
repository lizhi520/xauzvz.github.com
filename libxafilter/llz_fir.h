/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_fir.h 
  time    : 2012/07/09 23:38 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#ifndef _LLZ_FIR_H
#define _LLZ_FIR_H

#ifndef		M_PI
#define		M_PI							3.14159265358979323846
#endif

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

typedef int win_t;

enum {
    HAMMING = 0,
    BLACKMAN,
    KAISER,
};

/*
    4 type filter init and uninit
    note: the fc is the normalized frequency according the fmax, fmax = 0.5*fs
*/
uintptr_t llz_fir_filter_lpf_init(int frame_len, 
                                 int flt_len, float fc, win_t win_type);

uintptr_t llz_fir_filter_hpf_init(int frame_len, 
                                 int flt_len, float fc, win_t win_type);

uintptr_t llz_fir_filter_bandpass_init(int frame_len, 
                                      int flt_len, float fc1, float fc2, win_t win_type);

uintptr_t llz_fir_filter_bandstop_init(int frame_len, 
                                      int flt_len, float fc1, float fc2, win_t win_type);

void      llz_fir_filter_uninit(uintptr_t handle);


/*
    main function, do fir filting
*/
int llz_fir_filter(uintptr_t handle, float *buf_in, float *buf_out, int frame_len);

int llz_fir_filter_flush(uintptr_t handle, float *buf_out);

/*
    three main ascending cos window
*/

int   llz_hamming(float *w,const int N);

int   llz_blackman(float *w,const int N);

int   llz_kaiser(float *w, const int N);
int   llz_kaiser_beta(float *w, const int N, const float beta);
float llz_kaiser_atten2beta(float atten);

/*
    below are the utils which will be used in fir, and they maybe used seperatly, so
    I place them here
*/
int llz_hamming_cof_num(float ftrans);

int llz_blackman_cof_num(float ftrans);

int llz_kaiser_cof_num(float ftrans, float atten);

/*
    these below 5 functions, you can use them directly to get the filter coffs,
    WARN: the **h is dynamic be genearted and be malloced, you MUST free this 
    data memory when you no need use
*/
int llz_fir_lpf_cof(float **h, int N, float fc, win_t win_type);

int llz_fir_hpf_cof(float **h, int N, float fc, win_t win_type);

int llz_fir_bandpass_cof(float **h, int N, float fc1, float fc2, win_t win_type);

int llz_fir_bandstop_cof(float **h, int N, float fc1, float fc2, win_t win_type);

float llz_conv(const float *x, const float *h, int h_len);

#ifdef __cplusplus 
}
#endif  


#endif
