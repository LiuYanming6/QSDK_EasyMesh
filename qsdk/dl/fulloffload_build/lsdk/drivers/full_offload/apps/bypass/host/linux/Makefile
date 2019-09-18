CFLAGS += -I$(PWD)/../../include
CFLAGS += -I$(PWD)/../../../../../wlan_modules/include
CFLAGS += -I$(PWD)
CFLAGS += -MMD
CFLAGS += -Wall -g
CFLAGS += -D_GNU_SOURCE

OBJ_OUT=byp.o
LIB_OUT=byp
LIB_TYPE=rcs
BYP_BINARY=byp_cli
BYP_LIB=libbyp.a
ACFG_LIB=acfg
LIBRARY_PATH=$(PWD)/../../../../../../apps/acfg/src/

FILES=$(shell ls *.c)
OBJS=$(patsubst %.c,%.o,$(FILES))
LIBOBJS=byp_lib.o 

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

export LIB_OUT BYP_BINARY

all: clean build install

build: $(OBJS) lib   
	$(CC) $(CFLAGS) $(FILES) -o $(BYP_BINARY) -L$(LIBRARY_PATH) -l$(ACFG_LIB)

lib: $(LIBOBJS)
	$(AR) $(LIB_TYPE) $(BYP_LIB) $(LIBOBJS)

install:
	cp -f $(BYP_BINARY) $(DESTDIR)/sbin/.

clean:
	rm -rvf *.o *.a *.d
	rm -rvf $(BYP_BINARY) $(BYP_LIB)
