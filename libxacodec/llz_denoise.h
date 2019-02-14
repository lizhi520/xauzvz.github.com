
#ifndef	_LLZ_DENOISE_H
#define	_LLZ_DENOISE_H


#ifdef __cplusplus 
extern "C"
{ 
#endif  


enum{
    DENOISE_RNN = 0,
    DENOISE_LMS,
};

uintptr_t llz_denoise_init(int type, int channel, int sample_rate);
void llz_denoise_uninit(uintptr_t handle);
int llz_denoise_framelen_bytes(uintptr_t handle);
int llz_denoise(uintptr_t handle, unsigned char *inbuf, unsigned char * outbuf, int inlen);

#ifdef __cplusplus 
}
#endif  



#endif
