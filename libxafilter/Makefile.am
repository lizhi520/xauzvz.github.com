xafilter_src=llz_corr.c llz_fft.c llz_fft_fixed.c llz_fir.c\
			 llz_iir.c llz_levinson.c llz_lpc.c llz_mdct.c llz_mdct_fixed.c llz_resample.c

lib_LTLIBRARIES=libxafilter.la 
libxafilter_la_SOURCES= $(xafilter_src)
libxafilter_la_LIBADD=-lm
libxafilter_la_includedir=$(includedir)/libxafilter
libxafilter_la_include_HEADERS=llz_corr.h llz_fft.h llz_fft_fixed.h llz_fir.h\
							   llz_iir.h llz_levinson.h llz_lpc.h llz_mdct.h llz_mdct_fixed.h llz_resample.h


