
SRCS+= clientapilib.c cwmpside.c
OBJS = $(SRCS:%.c=%.o)
HEADERS+ = clientapilib.h clientapi.h ../includes/rpc.h ../includes/paramTree.h
CFLAGS+=-g -Wall

OBIN=   clientapilib.a

all: $(OBJS) $(OBIN) tests

$(OBIN):  clientapilib.o $(HEADERS)
	$(LD) -r -o $(OBIN) clientapilib.o

clean:
	-rm -f *~ $(OBJS) $(OBIN)
	for n in tests; do $(MAKE) clean -C $$n || exit; done

cleanall:
	-rm -f *.o *.a

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)

tests: subdirs
subdirs:
	for n in tests; do $(MAKE) CFLAGS='$(CFLAGS)' -C $$n || exit; done