#
# Top level Makefile to build gslib.a and gsauxlib.a
#
#
#CFLAGS+=-DUSE_IPv6
ifeq ($(strip $(TARGET_CPU)),X86)
CFLAGS+=-g 
#CFLAGS+=-DDMALLOC 
#SYSLIBS+=-ldmalloc
#DIRS= tests
endif

#
# 
DIRS= src auxsrc

ifeq (${USE_SSL},true)
	ifeq ($(strip $(TARGET_CPU)),X86)
		SYSLIBS+=-L/usr/local/ssl/lib -lssl -lcrypto
		SYSLIBS+=/usr/lib/libdl.a
		CFLAGS+=-I/usr/local/ssl/include
	else
		SYSLIBS+=-L$(OPENSOURCE_DIR)/openssl -lssl -lcrypto
		CFLAGS+=-I$(OPENSOURCE_DIR)/openssl/include
	endif
	CFLAGS+=-DUSE_SSL
endif

dummy: 
	for n in $(DIRS); do $(MAKE) CFLAGS='$(CFLAGS)' SYSLIBS='$(SYSLIBS)' -C $$n || exit; done

all:
	for n in $(DIRS); do $(MAKE) CFLAGS='$(CFLAGS)' SYSLIBS='$(SYSLIBS)' -C $$n || exit; done

tests:
	for n in $(DIRS); do $(MAKE) CFLAGS='$(CFLAGS)' SYSLIBS='$(SYSLIBS)' -C $$n || exit; done

clean:
	for n in $(DIRS); do $(MAKE) clean -C $$n || exit; done

