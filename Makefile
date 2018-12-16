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



include Makefile.include

libxautil.so:
	cd ./libxautil && make

xatools: libxautil.so
	cd ./xatools && make

all: libxautil.so xatools

clean : 
	cd ./libxautil && make clean
	cd ./xatools && make clean

install :
	cd ./libxautil && make install
	cd ./xatools && make install

.PHONY : all install clean 
