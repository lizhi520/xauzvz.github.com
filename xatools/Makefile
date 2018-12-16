include ../Makefile.include

CFLAGS += -I.
LDFLAGS += -L../libxautil \
		   -lm -lpthread -lxautil

#all tools objs
TEST_OBJS = test.o test_opt.o

test: $(TEST_OBJS)
	$(CC) -o test $(TEST_OBJS) $(LDFLAGS)	 

all: test

clean : 
	@rm -f *.o
	@rm -f *.out
	@rm -f test 

install :



