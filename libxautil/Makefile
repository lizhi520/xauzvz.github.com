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

SRCFILES =  $(shell ls *.c)
OBJFILES =  $(patsubst %.c,%.o,$(SRCFILES))

CFLAGS += -I. -fPIC -shared
LDFLAGS += -lm -lpthread 

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)	 

all: $(TARGET) 

clean: 
	@rm -f *.o
	@rm -f *.out
	@rm -f $(TARGET)
	@rm -rf $(TARGET).*

INSTALL_INCLUDE = $(PREFIX)/include/libxautil
INSTALL_LIB     = $(PREFIX)/lib 

install: all
	sudo mkdir -p $(PREFIX)/include/libxautil
	sudo cp llz_print.h llz_clock.h $(INSTALL_INCLUDE)
	sudo cp $(TARGET) $(INSTALL_LIB)

.PHONY : all install clean
