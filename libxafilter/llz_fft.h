/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_fft.h 
  time    : 2012/07/15 14:14 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#ifndef _LLZ_FFT_H
#define _LLZ_FFT_H

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

uintptr_t llz_fft_init(int size);
void llz_fft_uninit(uintptr_t handle);
uintptr_t llz_fft_init2(int size, int use_fft_work);

void llz_fft(uintptr_t handle, float *data);
void llz_ifft(uintptr_t handle, float* data);
void llz_ifft_f(uintptr_t handle, float* data);

float * llz_fft_get_fft_work(uintptr_t handle);


#ifdef __cplusplus 
}
#endif  


#endif
