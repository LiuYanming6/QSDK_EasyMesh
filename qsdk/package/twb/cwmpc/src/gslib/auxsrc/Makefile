SRCS= dns_lookup.c cpelog.c memtestwrapper.c
HDRS= $(SRCS:%.c=%.h)
CFLAGS+=-g
OBJS = $(SRCS:%.c=%.o)
DIR=../

.PHONY : allobj
allobj : $(OBJS) ;

.PHONY : clean
clean:
	-rm *.o
