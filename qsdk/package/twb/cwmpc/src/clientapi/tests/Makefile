SRCS+=testgpv.c testspv.c testadd.c testdelete.c testcommit.c runtest.c
OBJS = $(SRCS:%.c=%.o)
BINS = $(SRCS:%.c=%)

HEADERS+ = ../clientapilib.h ../clientapi.h ../../includes/rpc.h ../../includes/paramTree.h
CFLAGS+=-g -Wall -I..

APILIB=../clientapilib.a

all: $(OBJS) $(APILIB) tests

clean:
	-rm -f *~ $(OBJS) $(BINS)

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)

tests: testgpv testspv testadd testdelete testcommit runtest

testgpv: testgpv.c $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o testgpv $(APILIB) testgpv.c 

testspv: testspv.c $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o testspv $(APILIB) testspv.c 

testadd: testadd.c  $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o testadd $(APILIB) testadd.c 

testdelete: testdelete.c $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o testdelete $(APILIB) testdelete.c 

testcommit: testcommit.c $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o testcommit $(APILIB) testcommit.c
	
runtest: runtest.c $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o runtest $(APILIB) runtest.c
