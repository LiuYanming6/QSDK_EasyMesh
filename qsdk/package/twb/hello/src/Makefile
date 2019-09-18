#
# Copyright Copyright 2018-2019, twinA Corporation
# All Rights Reserved.
#
# Makefile for the hello example package
#
# $Id$
#

CFLAGS += $(TARGET_CFLAGS)

all: hello

hello: main.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f hello *.o
