#noinst_PROGRAMS=test
bin_PROGRAMS=xaresample xapcm2wav xadenoise xabgm
#bin_PROGRAMS=xaresample.bc xapcm2wav.bc xadenoise.bc

test_SOURCES=test.c test_opt.c
test_INCLUDES=test_opt.h
test_LDADD=-lm \
		   $(top_srcdir)/libxautil/libxautil.la

xaresample_SOURCES=xaresample.c xaresample_opt.c
xaresample_INCLUDES=xaresample_opt.h
xaresample_LDADD=-lm \
		   $(top_srcdir)/libxautil/libxautil.la \
		   $(top_srcdir)/libxafilter/libxafilter.la \
		   $(top_srcdir)/libxacodec/libxacodec.la

xapcm2wav_SOURCES=xapcm2wav.c xapcm2wav_opt.c
xapcm2wav_INCLUDES=xapcm2wav_opt.h
xapcm2wav_LDADD=-lm \
		   $(top_srcdir)/libxautil/libxautil.la \
		   $(top_srcdir)/libxafilter/libxafilter.la \
		   $(top_srcdir)/libxacodec/libxacodec.la

xadenoise_SOURCES=xadenoise.c 
xadenoise_INCLUDES=
xadenoise_LDADD=-lm \
		   $(top_srcdir)/libxautil/libxautil.la \
		   $(top_srcdir)/libxafilter/libxafilter.la \
		   $(top_srcdir)/libxacodec/libxacodec.la \
		   $(top_srcdir)/libxaext/librnnoise/librnnoise.la

xabgm_SOURCES=xabgm.c 
xabgm_INCLUDES=
xabgm_LDADD=-lm \
		   $(top_srcdir)/libxautil/libxautil.la \
		   $(top_srcdir)/libxafilter/libxafilter.la \
		   $(top_srcdir)/libxacodec/libxacodec.la 



#INCLUDES=-I$(top_srcdir)/libxautil -I$(top_srcdir)/libxafilter -I$(top_srcdir)/libxacodec
INCLUDES=-I$(top_srcdir)

