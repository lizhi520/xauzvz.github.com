/*
  llzlab - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi 罗龙智 (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_lms.h 
  time    : 2019/03/19 14:14 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#ifndef _LLZ_LMS_H
#define _LLZ_LMS_H

#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

uintptr_t llz_lms_init(int repeat_count);
void llz_lms_uninit(uintptr_t handle);

void llz_lms(uintptr_t h_lms, short *inbuffer, short *outbuffer, int frame_size);

#ifdef __cplusplus 
}
#endif  


#endif
