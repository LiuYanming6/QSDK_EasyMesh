

GENERIC_SRCS+= CPEWrapper.c \
      diagPing.c netIPState.c diagTraceRoute.c
ifeq (,$(findstring CONFIG_CLIENTAPI, $(CFLAGS)))
GENERIC_SRCS+=cpelisten.c
endif

NON_IGD_SRCS+=diagTraceRoute.c
SRCS=${GENERIC_SRCS} ${NON_IGD_SRCS} 
GOBJS = $(GENERIC_SRCS:%.c=%.o)
N_IGD_OBJS=$(NON_IGD_SRCS:%.c=%.o)
OBJS=${GOBJS} ${N_IGD_OBJS}
HEADERS+= ../includes/CPEWrapper.h


IGDBIN= igdCommon.o

$(IGDBIN):  $(GOBJS) $(HEADERS)
	-$(LD) -r -o $(IGDBIN) $(GOBJS) $(FOBJS)
	
clean :
	-rm -f *~ $(OBJS) $(OBIN) $(IGDBIN)

.c.o:
	-$(CC) ${XFLAGS} ${CFLAGS}${CPPFLAGS} -c $< -o $(<:%.c=%.o)




