

GENERIC_SRCS+= CPEWrapper.c \
      diagPing.c netIPState.c diagTraceRoute.c

SRCS=${GENERIC_SRCS} ${NON_IGD_SRCS} 
GOBJS = $(GENERIC_SRCS:%.c=%.o)
N_IGD_OBJS=$(NON_IGD_SRCS:%.c=%.o)
OBJS=${GOBJS} ${N_IGD_OBJS}
HEADERS+= ../includes/CPEWrapper.h

all: deviceCommon.o

DEVICEBIN= deviceCommon.o

$(OBIN):  $(OBJS) $(HEADERS)
	echo $(OBJS)
	$(LD) -r -o $(OBJS)

$(DEVICEBIN):  $(GOBJS) $(HEADERS)
	$(LD) -r -o $(DEVICEBIN) $(OBJS) $(FOBJS)
	
clean :
	-rm -f *~ $(OBJS) $(OBIN) $(DEVICEBIN)

.c.o:
	$(CC) ${XFLAGS} ${CFLAGS}${CPPFLAGS} -c $< -o $(<:%.c=%.o)




