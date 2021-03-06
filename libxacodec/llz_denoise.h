
#ifndef	_LLZ_DENOISE_H
#define	_LLZ_DENOISE_H


#ifdef __cplusplus 
extern "C"
{ 
#endif  

#include <stdint.h>

enum{
    DENOISE_RNN = 0,
    DENOISE_LMS = 1,
    DENOISE_FFT_LMS = 2,
    DENOISE_FFT_LMS_LPF = 3,
    DENOISE_LEARN1 = 4,
    DENOISE_LEARN2 = 5,
};

uintptr_t llz_denoise_init(int type, int channel, int sample_rate, float noise_gain, float lpf_fc);
void llz_denoise_uninit(uintptr_t handle);
int llz_denoise_framelen_bytes(uintptr_t handle);
int llz_denoise(uintptr_t handle, unsigned char *inbuf, int inlen, unsigned char * outbuf, int *outlen);
int llz_denoise_flush_spectrum(uintptr_t handle, int inlen, unsigned char *outbuf, int *outlen);

int llz_denoise_delay_offset(uintptr_t handle);


#ifdef __cplusplus 
}
#endif  



#endif
