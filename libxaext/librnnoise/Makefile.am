rnnoise_src=celt_lpc.c denoise.c kiss_fft.c pitch.c rnn.c rnn_data.c

INCLUDES=-I$(top_srcdir) -I$(top_srcdir)/libext/librnnoise

lib_LTLIBRARIES=librnnoise.la 
librnnoise_la_SOURCES= $(rnnoise_src)
librnnoise_la_LIBADD=-lm
librnnoise_la_includedir=$(includedir)/libxaext/librnnoise
librnnoise_la_include_HEADERS=rnnoise.h


