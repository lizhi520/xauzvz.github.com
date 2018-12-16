include ../Makefile.include

CFLAGS += -I. \
		  -I../libxautil

LDFLAGS += -L../libxautil \
		   -lxautil \
		   -lm -lpthread

#all tools objs
TEST_OBJS = test.o test_opt.o

test: $(TEST_OBJS)
	$(CC) -o test $(TEST_OBJS) $(LDFLAGS)	 

all: test

clean : 
	@rm -f *.o
	@rm -f *.out
	@rm -f test 

install:
	cp test $(INSTALL_BIN)



