
SRCS+= stun.c tr111p2.c
OBJS = $(SRCS:%.c=%.o)
HEADERS+ = ../includes/CPEWrapper.h
CFLAGS+=-g -Wall

OBIN=   tr111p2.a

$(OBIN):  $(OBJS) $(HEADERS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean:
	-rm -f *~ $(OBJS) $(OBIN)
	-rm stuntest

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)

stuntest: $(SRCS) $(HEADERS)
	-rm stuntest $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -DUSE_IPv6 -DTESTSTUN stuntest.c $(SRCS) \
	        ../gslib/gslib.a ../gslib/auxsrc/dns_lookup.o ../gslib/auxsrc/cpelog.o \
	        -lssl -lcrypto -o stuntest 

