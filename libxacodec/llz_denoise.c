/*
  xaudiopro - luolongzhi algorithm lab 
  Copyright (C) 2013 luolongzhi (Chengdu, China)

  This program is part of llzlab, all copyrights are reserved by luolongzhi. 

  filename: llz_denoise.c 
  time    : 2018/12/26 18:33 
  author  : luolongzhi ( luolongzhi@gmail.com )
*/

#include "libxautil/llz_print.h"
#include "libxafilter/llz_resample.h"
#include "libxafilter/llz_mixer.h"
#include "libxafilter/llz_lms.h"
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

#define	WINDOW_SCALE	16
#define	WINDOW_SCALE_V	(1<<WINDOW_SCALE)


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

} llz_denoise_t;


uintptr_t llz_denoise_init(int type, int channel, int sample_rate)
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


        free(f);
        f = NULL;
    }
}

int llz_denoise_framelen_bytes(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;

    return f->channel * llz_get_resample_framelen_bytes(f->h_resample[0][0]);

}


static void do_rnn_denoise(DenoiseState *st, unsigned char *inbuf, unsigned char *outbuf, int bytes_len)
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
        tmp = inbuf+i*RNN_FRAME_SIZE*2;

        for (j = 0; j < RNN_FRAME_SIZE; j++) x[j] = tmp[j];

        rnnoise_process_frame(st, x, x);

        out = outbuf+i*RNN_FRAME_SIZE*2;
        for (j = 0; j < RNN_FRAME_SIZE; j++) out[j] = x[j];
    }
#else
    memcpy(outbuf, inbuf, bytes_len);
#endif
}

/*FIR delay: (N-1)/(2*fs)*/
//resample use polyphase filter subfilter Q delay: (Q-1)/2
//rnn use frame size, but is 48000 transform, should covert to the original frequency offset by sample rate change ratio
int llz_denoise_delay_offset(uintptr_t handle)
{
    llz_denoise_t *f = (llz_denoise_t *)handle;
    int offset_up_resample;
    int offset_down_resample;
    int offset;
    int L, M;

    offset_up_resample = llz_resample_get_delay_offset(f->h_resample[0][0]);

    L = llz_get_resample_l(f->h_resample[0][0]);
    M = llz_get_resample_m(f->h_resample[0][0]);

    //resmplae down offset plus rnn offset
    offset_down_resample = M*(llz_resample_get_delay_offset(f->h_resample[0][1]))/L;
    offset = (int)((offset_up_resample + offset_down_resample + M*RNN_FRAME_SIZE/L)*2);

    /*printf("od=%d,M=%d,L=%d\n", offset_down_resample, M, L);*/
    /*printf("offset=%d\n", offset);*/

    return f->channel*offset;
}

static int do_spectrum_denoise(llz_denoise_t *f, unsigned char *inbuf, unsigned char *outbuf, int in_bytes_len)
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
			memcpy(data_in+OVERLAP, inbuf, sizeof(short)*HOP*chn);
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
			LMS_FLT(&nr->lmsfilter[i],data_in,data_out,HOP); 
		}
		
		if ((f->type == 2) || (f->type == 3)) {
			SpectrumReduction(&nr->srd,data_in,data_out,i);

			data_out = (short *)f->data_out[i];

			for(j = 0 ; j < HOP ; j++) 
				data_tmp_in[j] = data_out[j];				

			LMS_FLT(&nr->lmsfilter[i],data_tmp_in,data_tmp_out,HOP); 

			for(j = 0 ; j < HOP ; j++) 
				data_out[j] = data_tmp_out[j];

		}

		if(f->type == 3) {
			data_out = f->data_out[i];

			for(j = 0 ; j < HOP ; j++) {
				buf_tmp_in[j] = (float)data_out[j]/32768;
				buf_tmp_out[j] = 0;
			}
					
			FIR(buf_tmp_in,buf_tmp_out,HOP,&nr->lpf[i]);
			
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


		if((nr->nr_mode == 4)||(nr->nr_mode == 5)) {
		
			if(nr->nr_mode == 4)
				SpectrumReductionWithLearn(&nr->srd,data_in,data_out,i,nr->noise_learn[i]); //?Ƚ????׼?ȥ??
			if(nr->nr_mode == 5)
				SpectrumReductionWithLearn(&nr->srd,data_in,data_out,i,nr->noise_learn[i]); //?Ƚ????׼?ȥ??

			data_out = (short *)nr->data_out[i];
		
			for(j = 0 ; j < HOP ; j++)
			{
				data_tmp_in[j] = data_out[j];				
			}
			LMS_FLT(&nr->lmsfilter[i],data_tmp_in,data_tmp_out,HOP); 
			for(j = 0 ; j < HOP ; j++)
			{
				data_out[j] = data_tmp_out[j];
			}
		
		}


#if 1			
		
		if(nr->nr_mode == 6)	//һ??ģʽ??????ģʽ????Ҫ?׼����???
		{

			SpectrumReductionStrongFreq(&nr->srd,data_in,data_out,i);	//?Ƚ????׼?ȥ??

			//????????LMS??ǿ
			data_out = (short *)nr->data_out[i];

			for(j = 0 ; j < HOP ; j++)
			{
				data_tmp_in[j] = data_out[j];				
			}
			LMS_FLT(&nr->lmsfilter[i],data_tmp_in,data_tmp_out,HOP); 
			for(j = 0 ; j < HOP ; j++)
			{
				data_out[j] = data_tmp_out[j];
			}

		}

#endif

#if 1			
		
		if(nr->nr_mode == 7)	
		{
			SF_ReductionMono(nr->SF_HANDLE,data_in,data_out,HOP,i);

	//		SpectrumReductionStrongFreq(&nr->srd,data_in,data_out,i);	//?Ƚ????׼?ȥ??

			//????????LMS??ǿ
			data_out = (short *)nr->data_out[i];

			for(j = 0 ; j < HOP ; j++)
			{
				data_tmp_in[j] = data_out[j];				
			}
			LMS_FLT(&nr->lmsfilter[i],data_tmp_in,data_tmp_out,HOP); 
			for(j = 0 ; j < HOP ; j++)
			{
				data_out[j] = data_tmp_out[j];
			}

		}

#endif





#if 0
		{
			for(j = 0 ; j < FRAME_BUF ; j++)
			{
				data_tmp_in[j] = data_out[j];				
			}		
			MedianFilting(data_tmp_in,data_tmp_out,HOP);
			for(j = 0 ; j < HOP ; j++)
			{
				data_out[j] = data_tmp_out[j];
			}
			
		}
#endif


	}

	if(chn == 1)
		memcpy((BYTE *)outbuf,(BYTE *)nr->data_out[0],sizeof(short)*HOP*chn);
	else
	{
		for(i = 0; i < HOP; i++)
		{
			p_outbuf[2*i] = nr->data_out[0][i];
			p_outbuf[2*i+1] = nr->data_out[1][i];
			
		}
	
	}

	
	return wavinsize;






}



int llz_denoise(uintptr_t handle, unsigned char *inbuf, int inlen, unsigned char *outbuf, int *outlen) 
{
    llz_denoise_t *f = (llz_denoise_t *)handle;
    int frame_len, out_len_bytes, out_frame_len;
    int out_size;


    int i;

    frame_len = inlen /(2*f->channel);
    /*printf("--frame_len=%d\n", frame_len);*/

    if (f->channel == 1) {
        llz_resample(f->h_resample[0][0], inbuf, inlen, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise(f->rnn_st[0], f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, outbuf, &out_size);

        *outlen = out_size;
    } else {
        llz_mixer_stereo_left(inbuf, frame_len, f->inbuf[0], &out_frame_len);
        llz_resample(f->h_resample[0][0], f->inbuf[0], inlen>>1, f->rnn_inbuf[0], &out_len_bytes);
        do_rnn_denoise(f->rnn_st[0], f->rnn_inbuf[0], f->rnn_outbuf[0], out_len_bytes);
        llz_resample(f->h_resample[0][1], f->rnn_outbuf[0], out_len_bytes, f->outbuf[0], &out_size);

        llz_mixer_stereo_right(inbuf, frame_len, f->inbuf[1], &out_frame_len);
        llz_resample(f->h_resample[1][0], f->inbuf[1], inlen>>1, f->rnn_inbuf[1], &out_len_bytes);
        do_rnn_denoise(f->rnn_st[1], f->rnn_inbuf[1], f->rnn_outbuf[1], out_len_bytes);
        llz_resample(f->h_resample[1][1], f->rnn_outbuf[1], out_len_bytes, f->outbuf[1], &out_size);

        llz_mixer_lr2stereo(f->outbuf[0], f->outbuf[1], out_size>>1, outbuf, &out_size);

        *outlen = 2*f->channel*out_size;
    }

    return 0;
}
