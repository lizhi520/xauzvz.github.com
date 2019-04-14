/*
  xaudiopro - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_denoise.c 
  time    : 2018/12/26 18:33 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "libxautil/llz_print.h"
#include "libxafilter/llz_resample.h"
#include "libxafilter/llz_mixer.h"
#include "libxafilter/llz_lms.h"
#include "libxafilter/llz_fft.h"
#include "libxafilter/llz_fir.h"
#include "llz_denoise.h"
#include "libxaext/librnnoise/rnnoise.h"

//this is the rnn frame size
#define RNN_FRAME_SIZE  480

#ifndef PI
#define PI			    3.14159265358979323846
#endif

#ifndef max
#define max(a,b) ( a > b ? a : b)
#endif

#define FRAME_BUF		2048
#define FRAME_BUF_HALF	1024
#define HOP				512
#define OVERLAP			FRAME_BUF-HOP

#define	FFT_PROCESS_N	1025

#define	WINDOW_LEN		FRAME_BUF		//2048 	2048 is the framesize for fft
#define	WINDOW_HALFLEN	FRAME_BUF/2

#define	WINDOW_SCALE	15
#define	WINDOW_SCALE_V	(1<<WINDOW_SCALE)


#define	MUL_32_16_RMX(a,b,c)	(int)(((__int64)(a) *(__int64)(b))>>(c))
#define	COS_SIN_SCALE		    15
#define	COS_SIN_SCALE_V	        (1<<COS_SIN_SCALE)


typedef struct _llz_denoise_t {
    int type;

    int channel;
    int sample_rate;

    //rnn context
    //rnn noise 
    uintptr_t h_resample[2][2];
    unsigned char *inbuf[2];
    unsigned char *outbuf[2];
    unsigned char *rnn_inbuf[2];
    unsigned char *rnn_outbuf[2];
    DenoiseState *rnn_st[2];

    //nr common context
	short data_in[2][FRAME_BUF];
	short data_out[2][FRAME_BUF];
	int data_in_buf_size;

    //lms filter
    uintptr_t h_lms[2];

    //fft spectrum fixed point denoise
	uintptr_t h_fft[2];
	float fft_work_buf[2][FRAME_BUF*2];
	float window[FRAME_BUF];
	float mag[2][FFT_PROCESS_N];
	float real[2][FFT_PROCESS_N];
	float imag[2][FFT_PROCESS_N];
	float cos_phi[2][FFT_PROCESS_N];
	float sin_phi[2][FFT_PROCESS_N];
	float cpu_overlap_buf[2][FRAME_BUF];
	float mag_last[2][FFT_PROCESS_N];
	float mag_avg[2][FFT_PROCESS_N];
	float noise_gain;
	int   strongfreq_index;

    //lpf filter
    uintptr_t h_lpf[2];
    float lpf_fc;  // 0.25~0.5

} llz_denoise_t;

static int mag_noise_tab1[FFT_PROCESS_N]={
	8466 ,
 5061 ,	 1247 ,	 1403 ,	 1653 ,	 2016 ,	 2727 ,	 3642 ,	 3996 ,
 2966 ,	 2154 ,	 2102 ,	 2429 ,	 2317 ,	 2275 ,	 2379 ,	 2191 ,
 2348 ,	 2649 ,	 2486 ,	 1946 ,	 1810 ,	 2112 ,	 2219 ,	 2100 ,
 1992 ,	 1702 ,	 1414 ,	 1320 ,	 1437 ,	 1302 ,	 1122 ,	 1183 ,
 1084 ,	 1039 ,	 1104 ,	 1169 ,	 1046 ,	 1249 ,	 1114 ,	 980 ,
 980 ,	 1072 ,	 992 ,	 891 ,	 1000 ,	 840 ,	 1000 ,	 975 ,
 903 ,	 954 ,	 1110 ,	 896 ,	 820 ,	 792 ,	 749 ,	 935 ,
 1307 ,	 1382 ,	 1378 ,	 1140 ,	 1272 ,	 1336 ,	 1055 ,	 772 ,
 833 ,	 693 ,	 643 ,	 715 ,	 677 ,	 661 ,	 723 ,	 801 ,
 769 ,	 733 ,	 839 ,	 637 ,	 682 ,	 575 ,	 677 ,	 558 ,
 755 ,	 667 ,	 662 ,	 846 ,	 739 ,	 625 ,	 561 ,	 514 ,
 561 ,	 490 ,	 575 ,	 716 ,	 777 ,	 786 ,	 771 ,	 923 ,
 831 ,	 786 ,	 628 ,	 665 ,	 732 ,	 730 ,	 569 ,	 541 ,
 522 ,	 512 ,	 579 ,	 593 ,	 573 ,	 504 ,	 528 ,	 549 ,
 497 ,	 483 ,	 488 ,	 442 ,	 483 ,	 435 ,	 429 ,	 456 ,
 446 ,	 446 ,	 501 ,	 600 ,	 541 ,	 438 ,	 440 ,	 447 ,
 502 ,	 538 ,	 423 ,	 446 ,	 437 ,	 434 ,	 415 ,	 392 ,
 403 ,	 381 ,	 498 ,	 506 ,	 519 ,	 589 ,	 454 ,	 465 ,
 463 ,	 437 ,	 389 ,	 352 ,	 390 ,	 416 ,	 437 ,	 406 ,
 432 ,	 546 ,	 442 ,	 383 ,	 398 ,	 399 ,	 377 ,	 426 ,
 349 ,	 383 ,	 419 ,	 478 ,	 396 ,	 314 ,	 365 ,	 352 ,
 375 ,	 334 ,	 392 ,	 370 ,	 341 ,	 421 ,	 387 ,	 400 ,
 444 ,	 331 ,	 305 ,	 293 ,	 373 ,	 344 ,	 304 ,	 352 ,
 417 ,	 388 ,	 316 ,	 330 ,	 314 ,	 351 ,	 290 ,	 331 ,
 318 ,	 355 ,	 330 ,	 321 ,	 257 ,	 317 ,	 306 ,	 344 ,
 364 ,	 320 ,	 323 ,	 351 ,	 343 ,	 357 ,	 310 ,	 357 ,
 279 ,	 261 ,	 267 ,	 257 ,	 266 ,	 377 ,	 355 ,	 437 ,
 408 ,	 329 ,	 253 ,	 241 ,	 237 ,	 233 ,	 247 ,	 249 ,
 304 ,	 299 ,	 276 ,	 267 ,	 267 ,	 243 ,	 271 ,	 285 ,
 282 ,	 297 ,	 283 ,	 283 ,	 262 ,	 288 ,	 236 ,	 216 ,
 238 ,	 241 ,	 253 ,	 285 ,	 293 ,	 268 ,	 294 ,	 241 ,
 269 ,	 266 ,	 224 ,	 249 ,	 238 ,	 247 ,	 230 ,	 210 ,
 215 ,	 220 ,	 207 ,	 190 ,	 212 ,	 238 ,	 240 ,	 245 ,
 306 ,	 278 ,	 264 ,	 215 ,	 217 ,	 217 ,	 236 ,	 286 ,
 217 ,	 223 ,	 190 ,	 213 ,	 196 ,	 203 ,	 196 ,	 193 ,
 215 ,	 222 ,	 232 ,	 222 ,	 212 ,	 230 ,	 197 ,	 194 ,
 162 ,	 196 ,	 219 ,	 195 ,	 185 ,	 193 ,	 189 ,	 207 ,
 219 ,	 232 ,	 193 ,	 203 ,	 202 ,	 191 ,	 181 ,	 177 ,
 166 ,	 152 ,	 143 ,	 179 ,	 191 ,	 204 ,	 220 ,	 213 ,
 166 ,	 168 ,	 166 ,	 169 ,	 161 ,	 157 ,	 158 ,	 163 ,
 174 ,	 155 ,	 128 ,	 129 ,	 147 ,	 173 ,	 148 ,	 139 ,
 142 ,	 155 ,	 147 ,	 146 ,	 139 ,	 172 ,	 161 ,	 141 ,
 147 ,	 151 ,	 163 ,	 122 ,	 120 ,	 124 ,	 107 ,	 138 ,
 138 ,	 140 ,	 142 ,	 135 ,	 160 ,	 132 ,	 137 ,	 125 ,
 132 ,	 151 ,	 116 ,	 108 ,	 120 ,	 114 ,	 118 ,	 121 ,
 111 ,	 110 ,	 125 ,	 117 ,	 117 ,	 124 ,	 129 ,	 120 ,
 98 ,	 79 ,	 95 ,	 96 ,	 76 ,	 95 ,	 114 ,	 122 ,
 115 ,	 108 ,	 105 ,	 115 ,	 97 ,	 94 ,	 105 ,	 98 ,
 99 ,	 100 ,	 88 ,	 93 ,	 119 ,	 105 ,	 97 ,	 104 ,
 98 ,	 115 ,	 99 ,	 102 ,	 97 ,	 97 ,	 98 ,	 73 ,
 85 ,	 80 ,	 80 ,	 103 ,	 98 ,	 93 ,	 88 ,	 99 ,
 104 ,	 83 ,	 87 ,	 85 ,	 90 ,	 82 ,	 76 ,	 83 ,
 93 ,	 79 ,	 67 ,	 67 ,	 79 ,	 91 ,	 80 ,	 79 ,
 77 ,	 76 ,	 82 ,	 75 ,	 77 ,	 76 ,	 97 ,	 99 ,
 90 ,	 82 ,	 76 ,	 61 ,	 63 ,	 68 ,	 71 ,	 70 ,
 74 ,	 79 ,	 78 ,	 74 ,	 85 ,	 82 ,	 75 ,	 67 ,
 68 ,	 64 ,	 68 ,	 74 ,	 67 ,	 54 ,	 63 ,	 62 ,
 71 ,	 67 ,	 65 ,	 71 ,	 67 ,	 62 ,	 54 ,	 57 ,
 56 ,	 54 ,	 58 ,	 51 ,	 57 ,	 56 ,	 70 ,	 62 ,
 52 ,	 53 ,	 50 ,	 62 ,	 63 ,	 70 ,	 56 ,	 53 ,
 52 ,	 47 ,	 42 ,	 44 ,	 45 ,	 43 ,	 45 ,	 46 ,
 48 ,	 45 ,	 48 ,	 41 ,	 48 ,	 49 ,	 43 ,	 51 ,
 45 ,	 51 ,	 47 ,	 42 ,	 39 ,	 39 ,	 34 ,	 36 ,
 43 ,	 39 ,	 40 ,	 41 ,	 43 ,	 40 ,	 39 ,	 37 ,
 33 ,	 34 ,	 34 ,	 35 ,	 37 ,	 37 ,	 29 ,	 29 ,
 27 ,	 26 ,	 27 ,	 27 ,	 35 ,	 42 ,	 46 ,	 37 ,
 40 ,	 26 ,	 26 ,	 26 ,	 24 ,	 25 ,	 32 ,	 28 ,
 30 ,	 29 ,	 30 ,	 28 ,	 25 ,	 30 ,	 26 ,	 22 ,
 24 ,	 20 ,	 23 ,	 23 ,	 25 ,	 21 ,	 23 ,	 20 ,
 23 ,	 21 ,	 21 ,	 19 ,	 22 ,	 25 ,	 21 ,	 17 ,
 18 ,	 19 ,	 15 ,	 17 ,	 17 ,	 22 ,	 20 ,	 18 ,
 20 ,	 15 ,	 17 ,	 17 ,	 14 ,	 15 ,	 14 ,	 15 ,
 12 ,	 15 ,	 18 ,	 16 ,	 13 ,	 12 ,	 14 ,	 12 ,
 13 ,	 13 ,	 12 ,	 13 ,	 17 ,	 11 ,	 13 ,	 12 ,
 11 ,	 11 ,	 9 ,	 9 ,	 12 ,	 12 ,	 11 ,	 11 ,
 13 ,	 12 ,	 12 ,	 11 ,	 8 ,	 8 ,	 9 ,	 8 ,
 8 ,	 8 ,	 8 ,	 7 ,	 8 ,	 8 ,	 7 ,	 7 ,
 7 ,	 9 ,	 8 ,	 7 ,	 7 ,	 6 ,	 6 ,	 6 ,
 5 ,	 7 ,	 5 ,	 5 ,	 5 ,	 5 ,	 6 ,	 5 ,
 5 ,	 6 ,	 6 ,	 5 ,	 5 ,	 5 ,	 4 ,	 4 ,
 5 ,	 5 ,	 5 ,	 4 ,	 5 ,	 5 ,	 6 ,	 4 ,
 5 ,	 4 ,	 4 ,	 3 ,	 3 ,	 3 ,	 4 ,	 3 ,
 3 ,	 3 ,	 4 ,	 3 ,	 3 ,	 3 ,	 3 ,	 3 ,
 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 3 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 2 ,	 3 ,	 2 ,	 2 ,
 3 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 3 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 3 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 3 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,
 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,
 3 ,	 3 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 3 ,	 3 ,	 2 ,	 3 ,	 2 ,
 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,	 3 ,	 2 ,
 3 ,	 3 ,	 3 ,	 2 ,	 3 ,	 3 ,	 4 ,	 2 ,
 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 4 ,	 2 ,
 3 ,	 3 ,	 3 ,	 3 ,	 3 ,	 2 ,	 3 ,	 2 ,
 3 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,	 3 ,	 2 ,
 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 3 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 3 ,	 3 ,
 3 ,	 3 ,	 3 ,	 2 ,	 2 ,	 2 ,	 3 ,	 3 ,
 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 3 ,	 3 ,	 2 ,
 3 ,	 3 ,	 3 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 3 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 3 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 3 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 3 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,	 2 ,
 
};






static int blackman_window(float *window)
{
	int i;
	for (i = 0 ; i < WINDOW_LEN ; i++){
		window[i] = (0.42 - 0.5*cos(2*PI*i/(WINDOW_LEN))  + 0.08*cos(4*PI*i/(WINDOW_LEN)));
	}
	return WINDOW_LEN;
}

static void spectrum_reduction_init(llz_denoise_t *f, float noise_gain, float fs)
{
	float f_deta;

	blackman_window(f->window);

    f->h_fft[0] = llz_fft_init2(FRAME_BUF, 1);
    f->h_fft[1] = llz_fft_init2(FRAME_BUF, 1);

	f->noise_gain = noise_gain;

	memset(f->fft_work_buf, 0, 2*sizeof(float)*FRAME_BUF*2);

	memset(f->mag,  0, 2*sizeof(float)*FFT_PROCESS_N);
	memset(f->real, 0, 2*sizeof(float)*FFT_PROCESS_N);
	memset(f->imag, 0, 2*sizeof(float)*FFT_PROCESS_N);

	memset(f->cos_phi, 0, 2*sizeof(float)*FFT_PROCESS_N);
	memset(f->sin_phi, 0, 2*sizeof(float)*FFT_PROCESS_N);
	memset(f->cpu_overlap_buf, 0, 2*sizeof(float)*FRAME_BUF);

	memset(f->mag_last, 0, 2*sizeof(float)*FFT_PROCESS_N);
	memset(f->mag_avg,  0, 2*sizeof(float)*FFT_PROCESS_N);

	f_deta = fs/(2*FFT_PROCESS_N);
	f->strongfreq_index = 250/f_deta;

}

static void spectrum_reduction_uninit(llz_denoise_t *f)
{
	llz_fft_uninit(f->h_fft[0]);
	llz_fft_uninit(f->h_fft[1]);
}

static void change_noisegain(llz_denoise_t *f, float noise_gain)
{
	f->noise_gain = noise_gain;
}


static void spectrum_reduction(llz_denoise_t *f, short *data_in, short *data_out, int chn)
{
	int i,j,k;

	float *fft_work_buf;
    float *fft_work;
	
	float *window;
	float *mag;
	float *real;
	float *imag;
	float *cos_phi;
	float *sin_phi;
	float *cpu_overlap_buf;
	float gain;
	float gain_tmp;
	int   *mag_noise = mag_noise_tab1;

	i = chn;
	gain = f->noise_gain;

    fft_work_buf = f->fft_work_buf[i];

    window  = f->window;
    mag     = f->mag[i];
    real    = f->real[i];
    imag    = f->imag[i];
    cos_phi = f->cos_phi[i];
    sin_phi = f->sin_phi[i];

    cpu_overlap_buf = f->cpu_overlap_buf[i];

    for(j = 0 ; j < FRAME_BUF ; j++){
        fft_work_buf[j+j] = data_in[j] * window[j];
        fft_work_buf[j+j+1] = 0;
    }

    llz_fft(f->h_fft[i], fft_work_buf);
    fft_work = llz_fft_get_fft_work(f->h_fft[i]);

    for (j = 0 ; j < FFT_PROCESS_N ; j++){
        float  m_real,m_imag;

        m_real =  fft_work[j+j];
        m_imag =  fft_work[j+j+1];

        real[j] = m_real;
        imag[j] = m_imag;

        mag[j] = sqrt(m_real*m_real+m_imag*m_imag);

        cos_phi[j] = m_real/mag[j];
        sin_phi[j] = m_imag/mag[j];

        gain_tmp = 32*gain*mag_noise[j];

        if (mag[j] >= gain_tmp)
            mag[j] = mag[j] - gain_tmp;
        else
            mag[j] = 0;

        m_real = mag[j] * cos_phi[j];
        m_imag = mag[j] * sin_phi[j];

        fft_work_buf[j+j] = m_real;
        fft_work_buf[j+j+1] = m_imag;
    }

    for (j = 0 , k = FRAME_BUF_HALF - 1; j < FRAME_BUF_HALF - 1 ; j++,k--){
        int  m_real,m_imag;

        m_real = mag[k] * cos_phi[k];
        m_imag = mag[k] * sin_phi[k]; 

        fft_work_buf[FRAME_BUF+2+j+j]	= m_real;
        fft_work_buf[FRAME_BUF+2+j+j+1] = -m_imag;
    }

    llz_ifft_f(f->h_fft[i], fft_work_buf);

    for (j = 0 ; j < FRAME_BUF ; j++){
        cpu_overlap_buf[j] += 0.95*fft_work[j+j] * window[j];
    }

    for (j = 0 ; j < HOP ; j++){
        data_out[j] = cpu_overlap_buf[j];
    }

    memmove(cpu_overlap_buf, cpu_overlap_buf+HOP, sizeof(float)*OVERLAP);
    memset(cpu_overlap_buf+OVERLAP, 0, sizeof(float)*HOP);

}





uintptr_t llz_denoise_init(int type, int channel, int sample_rate, float noise_gain, float lpf_fc)
{
    llz_denoise_t *f = NULL;

    f = (llz_denoise_t *)malloc(sizeof(llz_denoise_t));

    f->type = type;

    f->channel = channel;
    f->sample_rate = sample_rate;

    //rnn denoise init
    f->rnn_st[0] = rnnoise_create();
    f->rnn_st[1] = rnnoise_create();
    f->inbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->inbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->outbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->outbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->rnn_inbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->rnn_inbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->rnn_outbuf[0] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);
    f->rnn_outbuf[1] = (unsigned char *)calloc(1, LLZ_RS_FRAMELEN_RNN_MAX*2*channel);


    //lms init
    f->h_lms[0] = llz_lms_init(1);
    f->h_lms[1] = llz_lms_init(1);

    //spectrum reduction init
    spectrum_reduction_init(f, noise_gain, sample_rate);

    //lpf fir init
    if (lpf_fc > 1)
        lpf_fc = 1;
    if (lpf_fc < 0.25)
        lpf_fc = 0.25;

    f->h_lpf[0] = llz_fir_filter_lpf_init(HOP, 31, 0.5*lpf_fc, BLACKMAN);
    f->h_lpf[1] = llz_fir_filter_lpf_init(HOP, 31, 0.5*lpf_fc, BLACKMAN);
    f->lpf_fc = lpf_fc;


    if (type == DENOISE_RNN) {
        if (sample_rate != 48000 &&
            sample_rate != 44100 &&
            sample_rate != 32000 &&
            sample_rate != 16000) {

            LLZ_PRINT_ERR("FAIL: unsupported sample rate\n");

            return 0;
        }

        switch (sample_rate) {
            case 48000: 
                f->h_resample[0][0] = f->h_resample[0][1] = 0;
                f->h_resample[1][0] = f->h_resample[1][1] = 0;
                break;
            case 44100: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(160, 147, 1.0, BLACKMAN);
                f->h_resample[0][1] = llz_resample_filter_rnn_init(147, 160, 1.0, BLACKMAN);
                f->h_resample[1][0] = llz_resample_filter_rnn_init(160, 147, 1.0, BLACKMAN);
                f->h_resample[1][1] = llz_resample_filter_rnn_init(147, 160, 1.0, BLACKMAN);
                break;
            case 32000: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(3, 2, 1.0, BLACKMAN);
                f->h_resample[0][1] = llz_resample_filter_rnn_init(2, 3, 1.0, BLACKMAN);
                f->h_resample[1][0] = llz_resample_filter_rnn_init(3, 2, 1.0, BLACKMAN);
                f->h_resample[1][1] = llz_resample_filter_rnn_init(2, 3, 1.0, BLACKMAN);
                break;
            case 16000: 
                f->h_resample[0][0] = llz_resample_filter_rnn_init(3, 1, 1.0, BLACKMAN); 
                f->h_resample[0][1] = llz_resample_filter_rnn_init(1, 3, 1.0, BLACKMAN); 
                f->h_resample[1][0] = llz_resample_filter_rnn_init(3, 1, 1.0, BLACKMAN); 
                f->h_resample[1][1] = llz_resample_filter_rnn_init(1, 3, 1.0, BLACKMAN); 
                break;
            default:
                LLZ_PRINT_ERR("FAIL: error sample rate\n");
                return 0;
        }
    }

    return (uintptr_t)f;
}

void llz_denoise_uninit(uintptr_t handle) 
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    if (f) {
        rnnoise_destroy(f->rnn_st[0]);
        rnnoise_destroy(f->rnn_st[1]);

        llz_lms_uninit(f->h_lms[0]);
        llz_lms_uninit(f->h_lms[1]);

        spectrum_reduction_uninit(f);

        llz_fir_filter_uninit(f->h_lpf[0]);
        llz_fir_filter_uninit(f->h_lpf[1]);

        free(f);
        f = NULL;
    }
}

int llz_denoise_framelen_bytes(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    if (f->type == DENOISE_RNN) 
        return f->channel * llz_get_resample_framelen_bytes(f->h_resample[0][0]);
    else 
        return f->channel * HOP * 2;

}


static void do_rnn_denoise_mono(DenoiseState *st, unsigned char *inbuf, unsigned char *outbuf, int bytes_len)
{
    int frame_len;
    int loop;
    float x[RNN_FRAME_SIZE];
    short *tmp, *out;
    int i, j;

    frame_len = bytes_len >> 1;
    loop = frame_len / RNN_FRAME_SIZE;

    /*printf("==>fram_len=%d, loop= %d\n", fram_len, loop);*/
#if 1 
    for (i = 0; i < loop; i++) {
        tmp = (short *)(inbuf+i*RNN_FRAME_SIZE*2);

        for (j = 0; j < RNN_FRAME_SIZE; j++) x[j] = tmp[j];

        rnnoise_process_frame(st, x, x);

        out = (short *)(outbuf+i*RNN_FRAME_SIZE*2);
        for (j = 0; j < RNN_FRAME_SIZE; j++) out[j] = x[j];
    }
#else
    memcpy(outbuf, inbuf, bytes_len);
#endif
}

/*FIR delay: (N-1)/(2*fs)*/
//resample use polyphase filter subfilter Q delay: (Q-1)/2
//rnn use frame size, but is 48000 transform, should covert to the original frequency offset by sample rate change ratio
static int get_rnn_delay_offset(llz_denoise_t *f)
{
    int offset_up_resample;
    int offset_down_resample;
    int offset;
    int L, M;

    offset_up_resample = llz_resample_get_delay_offset(f->h_resample[0][0]);

    L = llz_get_resample_l(f->h_resample[0][0]);
    M = llz_get_resample_m(f->h_resample[0][0]);

    //resmplae down offset plus rnn offset
    //rnn is work on 48k, so the offset should convert it from 48 offset to other samplerate offset by multiply M/L
    offset_down_resample = M*(llz_resample_get_delay_offset(f->h_resample[0][1]))/L;
    offset = (int)((offset_up_resample + offset_down_resample + M*RNN_FRAME_SIZE/L)*2);

    /*printf("od=%d,M=%d,L=%d\n", offset_down_resample, M, L);*/
    /*printf("offset=%d\n", offset);*/

    return f->channel*offset;
}


int llz_denoise_delay_offset(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    if (f->type == DENOISE_RNN) {
        return get_rnn_delay_offset(f);
    } else {
        return 0;
    }
}

static int do_spectrum_denoise(llz_denoise_t *f, unsigned char *inbuf, int in_bytes_len, unsigned char *outbuf)
{
	int i,j;

	short data_tmp_in[4*HOP], data_tmp_out[4*HOP];
	float buf_tmp_in[4*HOP], buf_tmp_out[4*HOP];
	short *p_inbuf=(short *)inbuf, *p_outbuf=(short *)outbuf;

	int chn = f->channel;

	short *data_in;
	short *data_out;

	for (i = 0 ; i < chn ; i++) {
		data_in = f->data_in[i];
		data_out = f->data_out[i];

		memmove(data_in, data_in+HOP, sizeof(short)*OVERLAP);
		
		if (chn == 1) {
			memcpy(data_in+OVERLAP, inbuf, sizeof(short)*HOP);
		} else {
			if(i == 0) {
				for(j = 0 ; j < HOP ; j++)
					data_in[OVERLAP+j] = p_inbuf[2*j];
			} else {
				for(j = 0 ; j < HOP ; j++)
					data_in[OVERLAP+j] = p_inbuf[2*j+1];	
			}
		}

		f->data_in_buf_size += sizeof(short)*HOP;
		if (f->data_in_buf_size < (sizeof(short)*FRAME_BUF*chn)) {
			if((i == 0) && (chn == 2))
				continue;
			else
				return 0;
		} else {
			f->data_in_buf_size = sizeof(short)*FRAME_BUF*chn;
		}

		if (f->type == DENOISE_LMS) {
			llz_lms(f->h_lms[i], data_in, data_out, HOP); 
		}

		
		if ((f->type == DENOISE_FFT_LMS) || (f->type == DENOISE_FFT_LMS_LPF)) {
			spectrum_reduction(f, data_in, data_out, i);

			data_out = (short *)f->data_out[i];

			for(j = 0 ; j < HOP ; j++) 
				data_tmp_in[j] = data_out[j];				

			llz_lms(f->h_lms[i], data_tmp_in, data_tmp_out, HOP); 

			for(j = 0 ; j < HOP ; j++) 
				data_out[j] = data_tmp_out[j];
		}

		if(f->type == DENOISE_FFT_LMS_LPF) {
			data_out = f->data_out[i];

			for(j = 0 ; j < HOP ; j++) {
				buf_tmp_in[j] = (float)data_out[j]/32768;
				buf_tmp_out[j] = 0;
			}
					
            llz_fir_filter(f->h_lpf[i], buf_tmp_in, buf_tmp_out, HOP);
			
			for(j = 0 ; j < HOP ; j++) {
				float temp;

				temp = buf_tmp_out[j] * 32768;
				if (temp >= 32768)
					temp = 32768;
				if (temp < -32767)
					temp = -32767;
				
				data_out[j] = temp;
			}
		}

#if 0

		if((f->type == DENOISE_LEARN1) || (f->type == DENOISE_LEARN2)) {
		
			if(f->type == DENOISE_LEARN1)
				SpectrumReductionWithLearn(&nr->f,data_in,data_out,i,nr->noise_learn[i]);
			if(nr->nr_mode == 5)
				SpectrumReductionWithLearn(&nr->f,data_in,data_out,i,nr->noise_learn[i]);

			data_out = (short *)nr->data_out[i];
		
			for(j = 0 ; j < HOP ; j++)
				data_tmp_in[j] = data_out[j];				
			LMS_FLT(&nr->lmsfilter[i],data_tmp_in,data_tmp_out,HOP); 
			for(j = 0 ; j < HOP ; j++)
				data_out[j] = data_tmp_out[j];
		}
#endif

	}

	if (chn == 1) {
		memcpy((unsigned char *)outbuf, (unsigned char *)f->data_out[0], sizeof(short)*HOP*chn);
    } else {
		for(i = 0; i < HOP; i++) {
			p_outbuf[2*i] = f->data_out[0][i];
			p_outbuf[2*i+1] = f->data_out[1][i];
		}
	}

	return in_bytes_len;
}


static int do_rnn_denoise(llz_denoise_t *f, unsigned char *inbuf, int inlen, unsigned char *outbuf)
{
    int frame_len, out_len_bytes, out_frame_len;
    int out_size;


    int i;

    frame_len = inlen /(2*f->channel);
    /*printf("--frame_len=%d\n", frame_len);*/

    if (f->channel == 1) {
        llz_resample(f->h_resample[0][0], inbuf, inlen, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise_mono(f->rnn_st[0], f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, outbuf, &out_size);

        return out_size;
    } else {
        llz_mixer_stereo_left(inbuf, frame_len, f->inbuf[0], &out_frame_len);
        llz_resample(f->h_resample[0][0], f->inbuf[0], inlen>>1, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise_mono(f->rnn_st[0], f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, f->outbuf[0], &out_size);

        llz_mixer_stereo_right(inbuf, frame_len, f->inbuf[1], &out_frame_len);
        llz_resample(f->h_resample[1][0], f->inbuf[1], inlen>>1, f->rnn_inbuf[1], &out_len_bytes);
        do_rnn_denoise_mono(f->rnn_st[1], f->rnn_inbuf[1], f->rnn_outbuf[1], out_len_bytes);
        llz_resample(f->h_resample[1][1], f->rnn_outbuf[1], out_len_bytes, f->outbuf[1], &out_size);

        llz_mixer_lr2stereo(f->outbuf[0], f->outbuf[1], out_size>>1, outbuf, &out_size);

        return 2*f->channel*out_size;
    }
}


int llz_denoise(uintptr_t handle, unsigned char *inbuf, int inlen, unsigned char *outbuf, int *outlen) 
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    switch (f->type) {
        case DENOISE_RNN:
            *outlen = do_rnn_denoise(f, inbuf, inlen, outbuf);
            break;
        default:
            *outlen = do_spectrum_denoise(f, inbuf, inlen, outbuf);
    }

    return 0;
}

int llz_denoise_flush_spectrum(uintptr_t handle, int inlen, unsigned char *outbuf, int *outlen)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    unsigned char *tmpbuf;
    int out_size = 0;
    int frame_len;

    frame_len = f->channel * HOP * 2;

    tmpbuf = (unsigned char *)malloc(frame_len);
    memset(tmpbuf, 0, frame_len);

    out_size  = frame_len;
    out_size += do_spectrum_denoise(f, tmpbuf, frame_len, outbuf+out_size);
    out_size += do_spectrum_denoise(f, tmpbuf, frame_len, outbuf+out_size);
    out_size += do_spectrum_denoise(f, tmpbuf, frame_len, outbuf+out_size);

    *outlen = out_size - (frame_len - inlen);

    free(tmpbuf);

    return 0;
}
