################################################################################
#        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
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
#    Project : twbencode
#    File    :  Makefile
#    Abstract: Build instructions
#
#    Modification History:
################################################################################
CFLAGS	= -I./include -I../../include/  $(EXTRA_CFLAGS)
LDFLAGS  = $(EXTRA_LDFLAGS)

PROG	= twbencode
SRC		= $(wildcard *.c)
OBJS	= $(SRC:.c=.o)


all: clean build

build: .depend $(PROG)

$(PROG): $(OBJS)
	@echo 'CC $@ $?'
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o

clean:
	rm -rf *~ *.o $(PROG) .depend

.depend: Makefile $(OBJS:.o=.c)
	$(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@


################################################################################
### << END >> ###
#################
