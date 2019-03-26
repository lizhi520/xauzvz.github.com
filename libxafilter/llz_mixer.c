#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int llz_mixer_pass(unsigned char *sample_in, int bytes_len, unsigned char *sample_out) {
    memcpy(sample_out, sample_in, bytes_len); 

    return 0;
}

int llz_mixer_stereo2mono(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size) {
    int i;

    short *psample_in;
    short *psample_out;
    int val;

    psample_in = (short *)sample_in;
    psample_out = (short *)sample_out;

    for (i = 0; i < sample_in_size; i++) {
        val = psample_in[i+i];
        val += psample_in[i+i+1];

        val >>= 1;

        if (val > 32767)
            val = 32767;
        if (val < -32768)
            val = -32768;

        psample_out[i] = (short)val;
    }

    *sample_out_size = sample_in_size;

    return 0;
}


int llz_mixer_mono2stereo(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size) {
    int i;

    short *psample_in;
    short *psample_out;
    int val;

    psample_in = (short *)sample_in;
    psample_out = (short *)sample_out;

    for (i = 0; i < sample_in_size; i++) {
        psample_out[i+i] = psample_in[i];
        psample_out[i+i+1] = psample_in[i];
    }

    *sample_out_size = sample_in_size;

    return 0;
}


int llz_mixer_stereo_left(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size) {
    int i;

    short *psample_in;
    short *psample_out;
    int val;

    psample_in = (short *)sample_in;
    psample_out = (short *)sample_out;

    for (i = 0; i < sample_in_size; i++) {
        psample_out[i] = psample_in[i+i];
    }

    *sample_out_size = sample_in_size;

    return 0;
}


int llz_mixer_stereo_right(unsigned char *sample_in, int sample_in_size, unsigned char *sample_out, int *sample_out_size) {
    int i;

    short *psample_in;
    short *psample_out;
    int val;

    psample_in = (short *)sample_in;
    psample_out = (short *)sample_out;

    for (i = 0; i < sample_in_size; i++) {
        psample_out[i] = psample_in[i+i+1];
    }

    *sample_out_size = sample_in_size;

    return 0;
}


int llz_mixer_lr2stereo(unsigned char *sample_in_left, unsigned char *sample_in_right, int sample_in_size, unsigned char *sample_out, int *sample_out_size) {
    int i;

    short *psample_in_left, *psample_in_right;
    short *psample_out;
    int val;

    psample_in_left  = (short *)sample_in_left;
    psample_in_right = (short *)sample_in_right;
    psample_out = (short *)sample_out;

    for (i = 0; i < sample_in_size; i++) {
        psample_out[i+i] = psample_in_left[i];
        psample_out[i+i+1] = psample_in_right[i];
    }

    *sample_out_size = sample_in_size;

    return 0;
}





