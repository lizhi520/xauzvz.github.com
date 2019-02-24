
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
int llz_denoise(uintptr_t handle, unsigned char *inbuf, int inlen, unsigned char * outbuf, int *outlen);

int llz_denoise_delay_offset(uintptr_t handle);


#ifdef __cplusplus 
}
#endif  



#endif
