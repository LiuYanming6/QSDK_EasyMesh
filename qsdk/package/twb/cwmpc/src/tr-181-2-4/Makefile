CFLAGS= -I ../includes -I ../soapRpc
OBIN=CPEObjects.o
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))

$(OBIN): $(OBJS)
	$(LD) -r -o $(OBIN) $(OBJS)

clean:
	-rm *.o

cleanall:
	-rm *.o


