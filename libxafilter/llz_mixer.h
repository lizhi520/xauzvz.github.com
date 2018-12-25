#ifndef _LLZ_MIXER_H
#define _LLZ_MIXER_H

#ifdef __cplusplus 
extern "C"
{ 
#endif  


int llz_mixer_stereo2mono(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size);
int llz_mixer_mono2stereo(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size);
int llz_mixer_stereo_left(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size);
int llz_mixer_stereo_right(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size);


#ifdef __cplusplus 
}
#endif  

#endif
