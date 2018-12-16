#  llzlab - luolongzhi algorithm lab 
#  Copyright (C) 2012 luolongzhi (Chengdu, China)
#
#  This program is part of llzlab, all copyrights are reserved by luolongzhi. 
#
#  filename: Makefile
#  time    : 2012/07/07 18:42 
#  author  : luolongzhi ( luolongzhi@gmail.com )
#
#


include ../Makefile.include

TARGET = libxautil.so

SRCS = $(shell ls *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

CFLAGS += -I. -fPIC -shared
LDFLAGS += -lm -lpthread 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)	 

all: $(TARGET) 

clean: 
	@rm -f *.o
	@rm -f *.out
	@rm -f $(TARGET)
	@rm -rf $(TARGET).*

install: all
	mkdir -p $(INSTALL_INCLUDE)/libxautil
	cp llz_print.h llz_clock.h llz_sort.h $(INSTALL_INCLUDE)
	cp $(TARGET) $(INSTALL_LIB)

.PHONY : all install clean
