xacodec_src=llz_wavfmt.c llz_denoise.c

lib_LTLIBRARIES=libxacodec.la 
libxacodec_la_SOURCES= $(xacodec_src)
libxacodec_la_LIBADD=-lm
libxacodec_la_includedir=$(includedir)/libxacodec
libxacodec_la_include_HEADERS=llz_wavfmt.h

INCLUDES=-I$(top_srcdir)

