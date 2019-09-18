#CFLAGS+=-DUSE_IPv6
DEBUG=true
SRCS = timertest.c memtest.c testParser.c  digesttest.c
SRCS+= testXMLWriter.c test2XMLWriter.c
SRCS+= dnstest.c
TESTS = testParser testXMLWriter
HEADERS =   
OBJS = $(SRCS:%.c=%.o)
AUXOBJS=../auxsrc/cpelog.o ../auxsrc/dns_lookup.o ../auxsrc/memtestwrapper.o ../../soapRpc/xmlTables.o
CFLAGS+=-g
SYSLIBS+=/usr/lib/libdmalloc.a

LIBS = ../gslib.a

test: $(OBJS)
	$(CC) $(CFLAGS)$(CPPFLAGS) digesttest.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o  digesttest
	$(CC) $(CFLAGS)$(CPPFLAGS) testXMLWriter.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o testXMLWriter
	$(CC) $(CFLAGS)$(CPPFLAGS) memtest.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o memtests
	$(CC) $(CFLAGS)$(CPPFLAGS) timertest.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o timertest
	$(CC) $(CFLAGS)$(CPPFLAGS) test2XMLWriter.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o test2XMLWriter
	$(CC) $(CFLAGS)$(CPPFLAGS) testParser.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o testParser
	$(CC) $(CFLAGS)$(CPPFLAGS) dnstest.o $(AUXOBJS) $(LIBS) $(SYSLIBS) -o dnstest
clean :
	-rm -f $(OBJS)
	-rm $(TESTS)



