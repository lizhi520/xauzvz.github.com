noinst_PROGRAMS=test
bin_PROGRAMS=xaresample

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

#INCLUDES=-I$(top_srcdir)/libxautil -I$(top_srcdir)/libxafilter -I$(top_srcdir)/libxacodec
INCLUDES=-I$(top_srcdir)

