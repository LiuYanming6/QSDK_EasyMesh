################################################################################
#        Copyright (c) 2018 TWin Advanced Technology Corp., Ltd.
#        All rights reserved.
#
#       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
#       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
#       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
#       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
#       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
#       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
#       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
#       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
#       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
#       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
#       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
#------------------------------------------------------------------------------
#    Project : libtwbox
#    File    :  Makefile
#    Abstract:
#
## TODO(SOMEONE): check why we need .so and .a?
## TODO(SOMEONE): check __ECHO2CONSOLE__=1
#
#    Modification History:
################################################################################
CFLAGS	= -I./ -I./include/ $(EXTRA_CFLAGS)
LDFLAGS = $(EXTRA_LDFLAGS)

LIBSO	= libtwbox.so
LIBA	= libtwbox.a
SRC	= $(wildcard *.c)
OBJS	= $(SRC:.c=.o)

ifeq ($(IS_PKG_PCRE),y)
SRC += pcre/ubox_regx.c
CFLAGS += -lpcre
endif

ifeq ($(IS_IPV6),y)
SRC += ipv6/ubox_ipv6.c
endif

#ifeq ($(USE_CRYPTO),)
#SRC += crypto/twbox_crypto.c
#SRC += crypto/base64.c
#endif

all: clean build

build: .depend $(LIBSO)

$(LIBSO): $(OBJS)
	@echo 'CC $@ $?'
	$(CC) $(LDFLAGS) -shared -o $@ $(OBJS)

#$(LIBA): $(OBJS)
#	@echo 'AR $@ $?'
#	$(AR) rcs $@ $(OBJS)
#	$(RANLIB) $@

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o

clean:
	rm -rf *~ *.o *.d $(LIBSO) $(LIBA) pcre/*.o pcre/*.d ipv6/*.o ipv6/*.d .depend

.depend: Makefile $(OBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@
