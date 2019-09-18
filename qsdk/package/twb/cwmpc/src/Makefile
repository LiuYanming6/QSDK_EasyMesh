#
# The "testbox" make target will build a standalone program that can be started from
# a shell. The DATAMODEL should be defined below or specified on the command line.
# i.e. make DATAMODEL=tr181-2-2 DATAMODELTYPE=IGD testbox
#
# Other targets:
#	  testobj:   default if no target specified. Linkable object module for
#                linking to user supplied startup framework (no main function,
#				 USE_CWMP_MAIN macro is undefined).
#     evalrelease
#			Creates in the next higher directory of the target directory
#			sources and the linkable object from the framework.
# 	  evalbuild
#			Creates testbox runnable code from the evalrelease.
##################################################################
# Choose Data model directory:
ifndef DATAMODEL
DATAMODEL=tr-181-2-4
#DATAMODEL=tr-098-1-4-0
#DATAMODEL=tr-098-1-2-0
endif
##################
ifndef DATAMODELTYPE
ifeq (181,$(findstring 181, $(DATAMODEL)))
# If DATAMODEL is a "Device." type of model then set
DATAMODELTYPE=Device
else
ifeq (tr-098-1-2-0, $(DATAMODEL))
# if early tr098 !!!!!!!!!! Use this with tr-098-1-2-0
DATAMODELTYPE=transition
else
ifeq (098, $(findstring 098, $(DATAMODEL)))
# if a InternetGatewayDevice then set
DATAMODELTYPE=IGD
endif
endif
endif
endif

################################################################
# Definitions for common code modules.
# 
#   IGDCOMMON: directory containing support functions common to
#              most IGD data model types of CPE devices. 
#              Typically functions is this 
#              directory are modified to complete the implementation
#              for the target CPE device.
IGDCOMMON=igdCommon
#
################################################################
#	DEVICECOMMON: directory containing common functions to support
#				data model Device:2.x.
DEVICECOMMON=deviceCommon
#
##################################################################
# Special definitions added for TR-196 and TR-104 Services
# Uncomment this to add the TR-196 service with TR-104.
#TR104TR196SERVICE=tr196
###################################################################
# Define Services directories. Only TR-104 is currently defined as
# a valid "Services" object to be included in the tr098-1-2 
# build. 
# This is only a valid definition if tr098-1-2 is also defined.
#SERVICES=tr104
###################################################################
# Specifiy toolchain components to use:
#
# X86 is Linux workstation environment.
#TARGET_CPU=X86
#TARGET_CPU=fusiv
#TARGET_CPU=STB
#
##################################################################
# Define DEBUG to generate diagnostic output to stderr.
export DEBUG=true
#
#####################################################################
# TR-069 RPC and CWMP framework build options
# Set the CWMP namespace identifier: As of TR-069 Amendment 3
# the current protocol version is v1.2. The default build will
# build with v1.2. To override and force v1.0 or v1.1 remove 
# the associated comment below.
#
#CFLAGS+=-DCONFIG_CWMP_V1_0
#CFLAGS+=-DCONFIG_CWMP_V1_1
# To ALLOW automatic fallback from version v1.1 or v1.2 to v1.0 when
# a Post of the Inform fails, define CONFIG_CWMP_FALLBACK.
# See section 3.2.1.1. Should be commented out if V1.0 is
# chosen above.
CFLAGS+=-DCONFIG_CWMP_FALLBACK
#
# Set defines for implementation of Optional RPCs.
#
# Note: The DownloadRPC is optional since the specification has
# a footnote that indicates it is only required if the 
# hardware supports it, even though it is in the required 
# list of RPCs.
# 
# If RPC is not to be supported than leave undefined.
#
CFLAGS+=-DCONFIG_RPCDOWNLOAD

CFLAGS+=-DCONFIG_RPCUPLOAD

CFLAGS+=-DCONFIG_RPCFACTORYRESET

# GetQueuedTransfers is considered DEPRECATED in V1.1.
#ALSO requires UPLOAD or DOWNLOAD
CFLAGS+=-DCONFIG_RPCGETQUEUEDTRANSFERS

CFLAGS+=-DCONFIG_RPCSCHEDULEINFORM

CFLAGS+=-DCONFIG_RPCREQUESTDOWNLOAD
#Causes generation of the RPC RequestDownloadResponse processing */
CFLAGS+=-DCONFIG_RPCSCHEDULEDOWNLOAD
# CFLAGS+=-DCONFIG_RPCKICK

# The SetVouchers configuration also includes the GetOptions RPC.
#CFLAGS+=-DCONFIG_RPCSETVOUCHERS

#
# The following were added to the CWMP V1.1 version
#ALSO requires UPLOAD or DOWNLOAD
CFLAGS+=-DCONFIG_RPCGETALLQUEUEDTRANSFERS
#
CFLAGS+=-DCONFIG_RPCAUTONOMOUSTRANSFERCOMPLETE
################################################################
# The following were added to the CWMP V1.2 version. If these are
# chosen the CONFIG_CWMP_V1_0 AND CONFIG_CWMP_V1_1 above should
# be commented out.
################################################################
# The following transfer RPCs require CONFIG_RPCDOWNLOAD be defined.
CFLAGS+=-DCONFIG_RPCSCHEDULEDOWNLOAD
CFLAGS+=-DCONFIG_RPCCANCELTRANSFER
#################################################################
# The following includes the software module management (SMM) defined
# in TR-157. Only available in CWMP V1.2 or later. If this is 
# defined the data model directory must include a definition header
# file SoftwareModules.h (profile SM_Baseline:1 or more) in order to
# build the sample quick start code.
ifneq (transition, $(DATAMODELTYPE))
CFLAGS+=-DCONFIG_RPCCHANGEDUSTATE
endif
#
###############################################################
# TR-111 Part 2. (also: Admendment 1, Annex G. Connection Request via
#                 NAT Gateway).  
# This flag forces the definition of TR106PROFILE_UDPCONNREQ_1
# when used with a TR-106 device.
CFLAGS+=-DCONFIG_TR111P2
#
################################################################
#
# IPv6  Support:
# Enable this option to generate the framework with IPv6 and 
# IPv4 support.
# If disabled only IPv4 is supported. 
CFLAGS+=-DUSE_IPv6 
#

################################################################
# SSL support:
# Comment the following line to remove SSL from the build.
#
export USE_SSL=true
#
# Comment the following line to remove use of sever and client certificates.
# The file path names of the certificates are defined in ${DATAMODEL}/targetsys.h.
export USE_CERTIFICATES=true
#
# Remove comment from the following to disable exact common name (CN) matching
# of the ACS host name and the server certificate's CN.
#CFLAGS+=-DDISABLE_CN_MATCHING
#
ifeq (${USE_SSL},true)
	CFLAGS+=-DUSE_SSL
	ifeq (${USE_CERTIFICATES},true)
		CFLAGS+=-DUSE_CERTIFICATES
	endif
	#ifeq ($(strip $(DEBUG)),true)
	#	CFLAGS+=-DDEBUGSSL
	#endif
endif
################################################################
# CPE Client API support:
#
# Builds the clientapilib and includes the API server code into
# the CWMPc build. 
GENERATE_CLIENTAPI=true
#

################################################################
# The following sections link in the toolchain into the build process. 
# Definition is toolchain dependent.
#################################################################
# STB target device.
ifeq ($(strip $(TARGET_CPU)),STB)
CROSS_COMPLE=/opt/STM/STLinux-2.2/devkit/sh4/bin/sh4-linux-
CC=$(CROSS_COMPLE)gcc
LD=$(CROSS_COMPLE)ld
AR=$(CROSS_COMPLE)ar
export CC LD AR
endif
#
# fusiv target device
ifeq ($(strip $(TARGET_CPU)),fusiv)
#Set BASEDIR to the full path of the release directory.
BASEDIR=/home/ikanos/LX_2618_RG_5_3_00r5_SRC
PACKAGEDIR=$(BASEDIR)/buildroot/package
OPENSSLDIR=$(PACKAGEDIR)/openssl/openssl-0.9.8a
CROSS_COMPLE=$(BASEDIR)/buildroot/build_mips_nofpu/staging_dir/bin/mips-linux-
CC=$(CROSS_COMPLE)gcc
LD=$(CROSS_COMPLE)ld
AR=$(CROSS_COMPLE)ar
export CC LD AR
# The following is prepended to any file paths for the fusiv enviornment.
ifeq (${USE_SSL},true)
		SYSLIBS+=-L$(OPENSSLDIR) -lssl -lcrypto
		CFLAGS+=-I$(OPENSSLDIR)/include		
endif
endif

#
# X86 Linux host enviroment
ifeq ($(strip $(TARGET_CPU)),X86)
CFLAGS+=-DTEST_TARGET
SSLUPDATED=true
ifeq (${SSLUPDATED},true)
ifeq (${USE_SSL},true)
		SYSLIBS+=-L/usr/local/ssl/lib/
		SYSLIBS+= -ldl -lssl -lcrypto
		CFLAGS+=-I/usr/local/ssl/include
endif
else
ifeq (${USE_SSL},true)
		SYSLIBS+=-L/usr/lib -L/usr/local/ssl/lib
		SYSLIBS+= -ldl -lssl -lcrypto
		CFLAGS+=-I/usr/include -I/usr/local/ssl/include
endif
endif
endif

################################################################
#	/* Connection Request URL formation.
#	 * Add code here to generate the random path that is unique toz
#	 * the CPE device. See section 3.2.2.
#	 * The length must be no more than CRPATH_SZ and must begin
#	 * with a '/'.
#	 */
CFLAGS+=-DGEN_CONNREQ_PATH
######################################################################
#
ifeq ($(strip $(DEBUG)),true)
CFLAGS+=-g 
CFLAGS+=-DDEBUGLOG
#CFLAGS+=-DUSE_GSMEMWRAPPER
#SYSLIBS+=gslib/auxsrc/memtestwrapper.o
#CFLAGS+=-DDMALLOC 
#SYSLIBS+=/usr/lib/libdmalloc.a
#	CFLAGS+=-DDEBUG
endif
################################################################
#
# TR-069 CWMP client executable name.
PROG=cwmpc
#
# 
###############################################################
# Library and directories for specific DATAMODELTYPE s
###############################################################
TOP=$(PWD)
ifeq ($(strip $(DATAMODELTYPE)),Device)
# Target is a "Device." type of model.
LIBS=${DATAMODEL}/CPEObjects.o ${DEVICECOMMON}/deviceCommon.o cpeCommon/cpeCommon.o
DIRS=${DATAMODEL} ${DEVICECOMMON} cpeCommon
EVLIBS=${DATAMODEL}/CPEObjects.o cpeCommon/cpeCommon.o
EVDIRS=${DATAMODEL} cpeCommon
#include DATAMODEL directory for header file includes
CFLAGS+=-I${TOP}/src/${DATAMODEL} -I../includes -I ../ -I ../soapRpc -I ../deviceCommon -I ../cpeCommon

else
ifeq ($(strip $(DATAMODELTYPE)),IGD)
# Target is TR-098 amendment 3.
LIBS+=${DATAMODEL}/CPEObjects.o ${IGDCOMMON}/igdCommon.o \
     cpeCommon/cpeCommon.o
DIRS+=${DATAMODEL} ${IGDCOMMON} cpeCommon
EVLIBS=${DATAMODEL}/CPEObjects.o cpeCommon/cpecommon.o
EVDIRS=${DATAMODEL} cpeCommon
#include DATAMODEL directory for header file includes
CFLAGS+=-I${TOP}/${DATAMODEL} -I../includes -I ../ -I ../soapRpc -I ../igdCommon -I ../cpeCommon
else
# Target is early TR-098 amendment.
LIBS+=${DATAMODEL}/CPEObjects.o \
     cpeCommon/cpeCommon.o igdCommon/CPEWrapper.o
DIRS+=${DATAMODEL} cpeCommon igdCommon
EVLIBS=${DATAMODEL}/CPEObjects.o cpeCommon/cpecommon.o igdCommon/CPEWrapper.o
EVDIRS=${DATAMODEL} cpeCommon igdCommon deviceCommon
#include DATAMODEL directory for header file includes
CFLAGS+=-I${TOP}/${DATAMODEL} -I../includes -I ../ -I ../soapRpc -I ../cpeCommon
endif
endif

#
##################################################################
# Include the STUN client code
ifneq (,$(findstring CONFIG_TR111P2, $(CFLAGS)))
DIRS+=tr111
LIBS+=tr111/tr111p2.a -lssl
endif
##################################################################
# Include client api code
ifeq ($(strip $(GENERATE_CLIENTAPI)),true)
DIRS+=clientapi
CFLAGS+=-DCONFIG_CLIENTAPI
LIBS+=clientapi/cwmpside.o
endif
##################################################################
# pthreads are used with SMM
ifneq (,$(findstring CONFIG_RPCCHANGEDUSTATE, $(CFLAGS)))
CFLAGS+= -pthread
LIBS+= -pthread
endif
##################################################################
# Include the directories for the CWMPc framework 
#
DIRS+= soapRpc main
DIRS+= gslib

LIBS+=soapRpc/soapRpc.o
LIBS+=main/main.o 
LIBS+=gslib/gslib.a gslib/auxsrc/dns_lookup.o
CFLAGS+=-g -Wall -Wstrict-prototypes --no-strict-aliasing $(EXTRA_CFLAGS) $(EXTRA_LDFLAGS)
#LDFLAG+=-Wl,-M 
# generate the CPE test pgms 
DIRS+=testCases

OBJS = $(SRCS:%.c=%.o)
# 
# object file target is first, generates linkable object
testobj: subdirs
	$(LD) -r $(LIBS) $(SYSLIBS) -o $(PROG).a

# testbox target object - generates executable with main()
testbox:  CFLAGS+=-DUSE_CWMP_MAIN
testbox: subdirs
	$(CC) $(CFLAGS) $(LIBS) $(SYSLIBS) ${LDFLAG} -o $(PROG)

subdirs:
	echo $(DATAMODEL) type is $(DATAMODELTYPE)
	echo CFLAGS=${CFLAGS}
	echo DIRS=$(DIRS)
	echo LIBS=$(LIBS)
	for n in $(DIRS); do $(MAKE) CFLAGS='$(CFLAGS)' -C $$n || exit; done

$(PROG): $(LIBS)
	$(CC) $(CFLAGS) $(LIBS) $(SYSLIBS) -o $(PROG)
	

clean:
	-rm -f *.o $(PROG)
	-rm cwmpc.a cwmpc
	for n in $(DIRS) testCases; do $(MAKE) clean -C $$n || exit; done

evalsubdirs:
	echo $(EVDIRS)
	for n in $(EVDIRS); do $(MAKE) CFLAGS='$(CFLAGS)' -C $$n || exit; done

evalbuild:  evalsubdirs
	$(CC) $(CFLAGS) $(LIBS) $(SYSLIBS) -o $(PROG)

	
SMMFILELIST=cwmp/gslib/gslib.a cwmp/gslib/src/*.h cwmp/gslib/auxsrc cwmp/includes \
	cwmp/main cwmp/soapRpc/*.h cwmp/soapRpc/soapRpc.o cwmp/deviceCommon \
	    cwmp/tr181-2-2 cwmp/testCases cwmp/clientapi/cwmpside.o \
	    cwmp/tr111/tr111p2.a cwmp/tr111/*.h \
	    cwmp/Makefile cwmp/cpestate-default.xml
smmsubdirs:
	SMMDIRS=${IGDCOMMON} ${DATAMODEL}
	for n in $(SMMDIRS); do $(MAKE) CFLAGS='$(CFLAGS)' -C $$n || exit; done
	
smmbuild: 
	$(CC) $(CFLAGS) $(LIBS) $(SYSLIBS) -o $(PROG)

smmrelease: CFLAGS+=-DUSE_CWMP_MAIN
smmrelease: subdirs
	cd ..; tar cvfz smmeval.tgz $(SMMFILELIST) cwmp/VERSION	

smminstall:
	mkdir /tmp/smm
	
	
EVALFILELIST=cwmp/gslib/gslib.a cwmp/gslib/src/*.h cwmp/gslib/auxsrc cwmp/includes \
	cwmp/main cwmp/soapRpc/*.h cwmp/soapRpc/soapRpc.o cwmp/deviceCommon \
	    cwmp/tr098-1-2 cwmp/tr181-2-2 cwmp/testCases \
	    cwmp/tr111/tr111p2.a cwmp/tr111/*.h \
	    cwmp/Makefile cwmp/cpestate-default.xml

ifeq ($(strip $(GENERATE_CLIENTAPI)),true)
	EVALFILELIST+=cwmp/clientapi/tests
	EVALFILELIST+=cwmp/clientapi/clientapilib.a cwmp/clientapi/clientapilib.h \
	cwmp/clientapi/clientapi.h cwmp/clientapi/cwmpside.o
endif

evalrelease: CFLAGS+=-DUSE_CWMP_MAIN
evalrelease: subdirs
	cd ..; tar cvfz cwmpeval.tgz $(EVALFILELIST) cwmp/VERSION
	    
	    
cleanall:
	for n in testCases gslib tr-098-1-2-0 tr-098-1-4-0 igdCommon deviceCommon \
	clientapi cpeCommon tr111 main soapRpc \
	tr-181-2-4 testCases;\
	do $(MAKE) clean -C $$n || exit; done
	-rm cwmpc cwmpc.a


export BASEDIR
export DATAMODEL
export SERVICES
export TARGET_CPU
export TR104TR196SERVICE
