SRCS+=smmTest.c
TRDIR = tr-181-2-4
BINS = $(SRCS:%.c=%)
CFLAGS+=-DCONFIG_RPCCHANGEDUSTATE -pthread
CFLAGS+=-g -Wall -I.. -I../../deviceCommon -I ../../includes
OBJS = ../../gslib/src/event.o ../../gslib/src/utils.o ../../gslib/src/md5.o \
      ../../gslib/auxsrc/cpelog.o ../smm.o ../../cpeCommon/cpeSMM.o \
      ../../gslib/src/www.o ../../cpeCommon/cpeFaultMessages.o \
      ../../$(TRDIR)/SoftwareModules.o
OBJS+= ../../soapRpc/rpcUtils.o 

TOBJS = ../../gslib/src/utils.o ../../gslib/src/md5.o \
      ../../gslib/auxsrc/cpelog.o \
      ../../gslib/src/www.o ../../cpeCommon/cpeFaultMessages.o \
      ../../$(TRDIR)/SoftwareModules.o ../../$(TRDIR)/DeviceInfo.o \
      ../../$(TRDIR)/DNS.o ../../$(TRDIR)/LANConfigSecurity.o \
      ../../$(TRDIR)/InterfaceStack.o ../../$(TRDIR)/Device.o
TOBJS+= ../../soapRpc/rpcUtils.o

FOBJS = ../../gslib/src/utils.o ../../gslib/src/md5.o \
      ../../gslib/auxsrc/cpelog.o  ../../soapRpc/rpcUtils.o
      
SYSLIBS+=-L/usr/lib -L/usr/local/ssl/lib
SYSLIBS+= -ldl -lssl -lcrypto
CFLAGS+=-I/usr/include -I/usr/local/ssl/include 
     
smmTest:
	$(CC) $(CFLAGS)$(CPPFLAGS) $(SYSLIBS) smmTest.c $(OBJS) $< -o smmTest

findTests: findTests.c
	$(CC) $(CFLAGS)$(CPPFLAGS) findTests.c $(FOBJS) -o findTests
	
tableTest:
	$(CC) $(CFLAGS)$(CPPFLAGS) tableTest.c $(TOBJS) $< -o tableTest	
	
offsets: offsets.c
	$(CC) $(CFLAGS)$(CPPFLAGS) $< -o offsets	


clean:
	-rm -f *~  $(BINS) 
