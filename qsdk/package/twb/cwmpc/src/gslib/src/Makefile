SRCS= ipv4ipv6.c event.c md5.c protocol.c utils.c wget.c www.c xmlParser.c\
 xmlParserSM.c xmlWriter.c utils_hyd.c 
HDRS= $(SRCS:%.c=%.h)
CFLAGS+=-g
OBJS = $(SRCS:%.c=%.o)
ODIR=../
OBIN = $(ODIR)gslib.a

$(OBIN): $(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

.PHONY : allobj
allobj : $(OBJS) ;


.PHONY : clean
clean:
	-rm *.o
	-rm $(OBIN)

