PROG=cpereq
SRC=cpereq.c

cpereq: cpereq.o
	$(CC) $(CFLAGS) $(SRC) $(LIBS) $(SYSLIBS) -o $(PROG)
readdhcpoptions: readdhcpoptions.o
	$(CC) -g $(CFLAGS) readdhcpoptions.c ../cpeCommon/dhcpVendorOptions.o $(SYSLIBS) -o readdhcpoptions
clean:
	-rm ${PROG} ${PROG}.o readdhcpoptions.o readdhcpoptions



