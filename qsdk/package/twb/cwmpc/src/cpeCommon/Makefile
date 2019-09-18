

GENERIC_SRCS+= cpeFaultMessages.c CPETransfer.c\
      dhcpVendorOptions.c sysWrapper.c sysutils.c \
      cpelisten.c configsave.c
ifneq (,$(findstring CONFIG_RPCCHANGEDUSTATE, $(CFLAGS)))
GENERIC_SRCS+=cpeSMM.c
endif
SRCS=${GENERIC_SRCS} ${NON_IGD_SRCS} 
GOBJS = $(GENERIC_SRCS:%.c=%.o)
N_IGD_OBJS=$(NON_IGD_SRCS:%.c=%.o)
OBJS=${GOBJS} ${N_IGD_OBJS}
HEADERS+= ../includes/CPEWrapper.h
CPEBIN=cpeCommon.o
all: cpeCommon.o

$(OBIN):  $(OBJS) $(HEADERS)
	echo $(OBJS)
	$(LD) -r -o $(OBJS)

$(CPEBIN):  $(GOBJS) $(HEADERS)
	$(LD) -r -o $(CPEBIN) $(OBJS) $(FOBJS)
	
clean :
	-rm -f *~ $(OBJS) $(OBIN) $(CPEBIN) cpeSMM.o

.c.o:
	$(CC) ${XFLAGS} ${CFLAGS}${CPPFLAGS} -c $< -o $(<:%.c=%.o)




