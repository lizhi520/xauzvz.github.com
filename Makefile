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

#check_dir:
	#$(shell test -d $(INSTALL_INCLUDE) || mkdir -p $(INSTALL_INCLUDE))
	#$(shell test -d $(INSTALL_LIB) || mkdir -p $(INSTALL_LIB))
	#$(shell test -d $(INSTALL_BIN) || mkdir -p $(INSTALL_BIN))

libxautil.so: check_dir
	cd $(TOPDIR)/libxautil && make

xatools: libxautil.so
	cd $(TOPDIR)/xatools && make

all: libxautil.so xatools

clean: 
	cd $(TOPDIR)/libxautil && make clean
	cd $(TOPDIR)/xatools && make clean
	@rm -rf build

install_libs: 
	cd $(TOPDIR)/libxautil && make install

install: install_libs
	cd $(TOPDIR)/xatools && make install

.PHONY: check_dir libxautil.so xatools all install install_libs clean 
