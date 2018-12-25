/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_corr.h 
  time    : 2012/11/13 22:52
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#ifndef _LLZ_CORR_H
#define _LLZ_CORR_H 

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

void  llz_autocorr(float *x, int n, int p, float *r);

void  llz_crosscorr(float *x, float *y, int n, int p, float *r);

float llz_corr_cof(float *a, float *b, int len);

uintptr_t llz_autocorr_fast_init(int n);
void  llz_autocorr_fast_uninit(uintptr_t handle);
void  llz_autocorr_fast(uintptr_t handle, float *x, int n, int p, float *r);


#ifdef __cplusplus 
}
#endif  



#endif
