/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_bgm.h 
  time    : 2019/04/16 23:41
  author  : luolongzhi ( luolongzhi@gmail.com )
*/


#ifndef _LLZ_BGM_H
#define _LLZ_BGM_H

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>


uintptr_t llz_bgm_init();

void llz_bgm_uninit(uintptr_t handle);

int llz_bgm_extract(uintptr_t handle, unsigned char *buf_in, unsigned char *buf_out, int frame_len_bytes);


#ifdef __cplusplus 
}
#endif  



#endif






