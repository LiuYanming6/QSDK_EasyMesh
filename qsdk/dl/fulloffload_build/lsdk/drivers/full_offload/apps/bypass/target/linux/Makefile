CFLAGS += -I$(PWD)/../../../../../wlan_modules/include
CFLAGS += -I$(PWD)/../../include
CFLAGS += -I$(PWD)
CFLAGS += -MMD
CFLAGS += -Wall -g
CFLAGS += -D_GNU_SOURCE

ifeq ($(strip $(OFFLOAD_GMAC_8021Q)),1)
CFLAGS    += -DGMAC_8021Q_SUPPORT
endif


OBJ_OUT=byp.o
LIB_OUT=byp
LIB_TYPE=rcs
BYP_BINARY=bypd
ACFG_LIB_PATH=$(PWD)/../../../acfg/target/linux/
ACFG_LIB=acfg

FILES=$(shell ls *.c)
OBJS=$(patsubst %.c,%.o,$(FILES))

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

export LIB_OUT BYP_BINARY

all: clean build install

build: $(OBJS)    
	$(CC) -Wall $(CFLAGS) $(FILES) -L$(ACFG_LIB_PATH) -l$(ACFG_LIB) -o $(BYP_BINARY)

install:
	cp -f $(BYP_BINARY) $(DESTDIR)/sbin/.

clean:
	rm -rvf *.o *.a *.d
	rm -rvf $(BYP_BINARY)
