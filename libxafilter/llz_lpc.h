/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_lpc.h
  time    : 2012/11/17 23:52
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#ifndef _LLZ_LPC_H
#define _LLZ_LPC_H 

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

uintptr_t llz_lpc_init(int p);
void      llz_lpc_uninit(uintptr_t handle);
float llz_lpc(uintptr_t handle, float *x, int x_len, float *lpc_cof, float *kcof, float *err);
 

#ifdef __cplusplus 
}
#endif  



#endif
