CFLAGS += -I$(PWD)/../../../../../wlan_modules/include
CFLAGS += -I$(PWD)/../../include
CFLAGS += -I$(PWD)
CFLAGS += -MMD
CFLAGS += -Wall -g
CFLAGS += -D_GNU_SOURCE

OBJ_OUT=acfgd.o
LIB_OUT=libacfg.a
LIB_TYPE=rcs
ACFGD_BINARY=acfgd

FILES=$(shell ls *.c)
OBJS=$(patsubst %.c,%.o,$(FILES))

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

export LIB_OUT ACFGD_BINARY

all: clean build install

build: $(OBJS)    
	$(CC) -Wall $(CFLAGS) $(FILES) -o $(ACFGD_BINARY) && \
    $(AR) $(LIB_TYPE) $(LIB_OUT) $(OBJS) 

install:
	cp -f $(ACFGD_BINARY) $(DESTDIR)/sbin/.

clean:
	rm -rvf *.o *.a *.d
	rm -rvf $(ACFGD_BINARY)
