#
# Copyright Copyright 2018-2019, twinA Corporation
# All Rights Reserved.
#
# Makefile for the enap.fcgi
#
# $Id$
#

CFLAGS += -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/twbox -I$(STAGING_DIR)/usr/include/json-c $(TARGET_CFLAGS) -fstack-protector -Werror

LDFLAGS+= -L$(STAGING_DIR)/usr/lib -ljson-c -luci -lfcgi -lssl -lcrypto -ltwbox -lssp

all: cgibox

cgibox: cgibox.o 
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f cgibox *.o
