ifneq (,$(findstring CONFIG_RPCKICK, $(CFLAGS)))
SRCS = kickHandler.c
endif
ifneq (,$(findstring CONFIG_RPCCHANGEDUSTATE, $(CFLAGS)))
SRCS+= smm.c
HEADERS = smm.h
endif
SRCS += transfer.c xmlTables.c rpcUtils.c rpcMethods.c
SRCS += cwmpSession.c acsconnreq.c notify.c
OBJS = $(SRCS:%.c=%.o)
HEADERS =xmlTables.h rpcUtils.h acsconnreq.h

OBIN=   soapRpc.o

$(OBIN):        $(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

test:
	$(LD) -r -o $(OBIN) $(OBJS)

clean :
	-rm -f *~ $(OBJS) $(OBIN) kickHandler.o smm.o

.c.o:
	$(CC) $(CFLAGS)$(CPPFLAGS) -c $< -o $(<:%.c=%.o)


