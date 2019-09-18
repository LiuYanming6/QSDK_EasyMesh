/*----------------------------------------------------------------------*
 * Gatespace Networks, Inc.
 * Copyright 2008, 2011 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : CPETransfer.c
 *
 * Description: Sample code for various Download and Upload callback
 *              functions. These functions are called from within the
 *              CWMPC framework to implement the data transfer functions.
 *
 *              Specific to a Linux OS.
 *
 * $Revision: 1.13 $
 * $Id: CPETransfer.c,v 1.13 2012/05/10 17:38:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include	<string.h>
#include 	<time.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<fcntl.h>
#include	<unistd.h>
#include <errno.h>
#include <ctype.h>

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "../includes/sys.h"
#include "../gslib/src/utils.h"
#include "../gslib/src/event.h"
#include "../gslib/src/protocol.h"
#include "../includes/paramTree.h"
#include "../includes/CPEWrapper.h"
#include "../soapRpc/acsconnreq.h"
#include "../soapRpc/rpcUtils.h"
#include "../soapRpc/notify.h"
#include "../soapRpc/cwmpSession.h"

#include "targetsys.h"


#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

extern void *acsSession;
extern CWMPObject CWMP_RootObject[];
extern CWMPObject Services_Objs[];
extern CPEState	cpeState;

typedef enum {
	XIDLE = 0,
	XPENDING,
	XSTARTED,
	XDONE
} eREQSTATE;
#define MAX_XPENDING	4
static int	nPending = 0;
static int  nXfer = 0;

typedef struct PendingXfer{
	eRPCMethods	rpc;
	eREQSTATE	state;
	DownloadMsg	dlMsg;
} PendingXfer;

static PendingXfer pendingXfer[MAX_XPENDING];

static FILE *fp;
/*
 * Find the dlQActive request that has just finished and
 * update its status.
 * If the CPE requires a reboot before continuing the
 * updated cpeState information must be saved.
 */
static void setActiveXferStatus( PendingXfer *px ){
	RPCRequest *r = cpeState.dlQActive;
	DownloadMsg *d;
	while ( r ){
		d = &r->ud.downloadReq;
		if ( strcmp( px->dlMsg.URL, d->URL)== 0 ){ /* found request */
			d->dlStatus = px->dlMsg.dlStatus;
			d->dlFaultMsg = px->dlMsg.dlFaultMsg;
			d->dlStartTime = px->dlMsg.dlStartTime;
			d->dlEndTime = px->dlMsg.dlEndTime;
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
			// If using ScheduleDownload RPC then stop window timers
			// associated with the request.
			if (r->rpcMethod == eScheduleDownload )
				cwmpStopDownloadWindowTimers(r);
#endif
		}
		r= r->next;
	}
}

static void startOne(void);
static int  rebootFlag = 0;
/*
 * The following are used for testing the WindowMode for ScheduledDownload
 */
#ifdef CONFIG_RPCSCHEDULEDOWNLOAD
void endWait( void * handle){
	cwmpDownloadWindowResume();
}
/*
 * The cpeDownloadWindowStart is called when the ScheduleDownload
 * time window is active. On some CPE devices this function would
 * interface with the native configuration management or local UI
 * to determine if the transfer should proceed.
 *
 * This function may be called multiple times for
 * each ScheduleDownload request.
 * Returns:
 * 		eSTARTTRANSFER: The download is started.
 *
 * 		eWAIT:			Indicates the transfer functions to wait for a call to
 * 						cwmpDownloadWaitResume() to start the transfer in this time window.
 *
 *
 * 		eSKIPWINDOW: 	The current time window is skipped. If the current
 * 						window is the last window a TransferComplete event is
 * 						scheduled. If the next time window is present the
 * 						download request is placed on the download queue
 * 						to wait on the next time window.
 *
 * 		eABORT:			The download request is aborted and the request is
 * 						marked as complete/aborted (9020) and a TransferComplete is
 * 						scheduled.
 *
 * The UserMessage contents is used to automate testing.
 */
WINDOWACTION cpeDownloadWindowStart( DownloadMsg *r) {
	TimeWindow *twp = &r->timeWindow[r->timeIndex];  /* get current TimeWindow */
	DBGPRINT((stderr, "cpeDownloadWindowStart index=%d mode=%s userMsg=%s\n",
			r->timeIndex, twp->windowMode, twp->userMessage));
	// UserMessage is tested for type of windowAction to return for automated testing....
	if ( twp->userMessage ) {
		if ( strstr( twp->userMessage, "wait")){
			free(twp->userMessage);
			twp->userMessage = strdup("start"); // Change wait to start.
			setTimer(endWait, NULL, 10*1000);   // wait 10 sec.
			return eWAIT;
		}
		if ( strstr( twp->userMessage, "abort")){
			return eABORT;
		}
		if ( strstr( twp->userMessage, "skip")){
			return eSKIPWINDOW;
		}

	}
	return eSTARTTRANSFER;
}
/**
 * cpeDownloadWindowEnd is called when the current TimeWindow has expired.
 * This is not called if the transfer completes before the window expires.
 */
void cpeDownloadWindowEnd( DownloadMsg *r) {
	TimeWindow *twp = &r->timeWindow[r->timeIndex];  /* get current TimeWindow */
	DBGPRINT((stderr, "cpeDownloadWindowEnd index=%d mode=%s userMsg=%s\n",
				r->timeIndex, twp->windowMode, twp->userMessage));
	stopTimer(endWait,NULL);		// stop the endWait timer if its running
}
#endif
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * The following functions are used to setup for non-CWMP
 * transfered data. For the purposes of this test code the
 * wget utility is used to transfer the files from the
 * servers.
 */
static void stopXfer(void *handle){
	int i;
	DBGPRINT((stderr, "stopXfer\n"));
	/* stop the callback from the CWMP framework */
	stopListener(fileno(fp));
	/* close the pipe */
	pclose(fp);
	fp = 0;

	nXfer++;
	if ( nXfer < nPending ){
		/* if there is a pending transfer then start it */
		startOne();
	} else {
		/* All completed. Typical steps here may be */
		/* 1. Verify that all down loaded files are valid */
		/* 2. update the Active queued transfer requests with
		 *    the current status.
		 * 3. signal the cwmp to begin a new inform session
		 *    Or just flash and reboot.
		 */

		/* For this test we update the queued transfer
		 * requests status information.
		 */
		for ( i= 0; i< nPending; ++i ){
			setActiveXferStatus( &pendingXfer[i]);
		}

		/* Save the updated CPE state information with the xfer status*/
		cpeSaveCPEState();

		/* If reboot then exit -- this is one of the test scripts */
		if ( rebootFlag )
			exit(0);

		/* Initiates a new Inform session to deliver
		 * the TransferComplete RPC for the completed transfers.
		 * There is no need to call this function if the CPE is
		 * rebooted following the transfer completion.
		 */
		cwmpSendXfersComplete();
		nXfer = nPending = 0;
	}
}
/*
 * Reads the stdout and stderr that is piped from
 * the wget program.
 * Reads one line at a time and stops on EOF.
 */
static void doReadWget(void *handle)
{
	char buf[1024];

	if ( fgets(buf, 1024, fp) == NULL ) {
		/* EOF on pipe indicates the wget program has completed */
		pendingXfer[nXfer].dlMsg.dlEndTime = time(0);
		stopXfer(NULL);
	} else if (pendingXfer[nXfer].state != XDONE ){
		DBGPRINT((stderr, "buf=>%s<", buf));
		if ( strstr(buf, "Length: ")  ) {
			// xfer complete
			pendingXfer[nXfer].dlMsg.dlStatus = 0;
			pendingXfer[nXfer].dlMsg.dlFaultMsg = FAULTNONE;
			pendingXfer[nXfer].state = XDONE;
		} else if ( strstr(buf, "Resolving") && strstr(buf, "failed")){
			/* unknown host xxxxxxxxx */
			pendingXfer[nXfer].dlMsg.dlStatus = 9015;
			pendingXfer[nXfer].dlMsg.dlFaultMsg = FAULT9015;
			pendingXfer[nXfer].state = XDONE;
		} else if ( strstr(buf, "ERROR") && strstr(buf, "Not Found")  ) {
			/* unable to access file */
			pendingXfer[nXfer].dlMsg.dlStatus = 9016;
			pendingXfer[nXfer].dlMsg.dlFaultMsg = FAULT9016;
			pendingXfer[nXfer].state = XDONE;
		} else if ( strstr(buf, "Authorization failed")){
			/*  */
			pendingXfer[nXfer].dlMsg.dlStatus = 9019;
			pendingXfer[nXfer].dlMsg.dlFaultMsg = FAULT9019;
			pendingXfer[nXfer].state = XDONE;
		}
		 /* other failures, noise, .....*/
	} else {
		/* continue until EOF on pipe */
		DBGPRINT((stderr, "buf=>%s<", buf));
	}
}
/*
 * For testing purposes the wget program is used to transfer
 * files from the remote server to the local CPE. The stderr and
 * stdout streams are piped to a socket that is opened by popen().
 * The doReadWget() function reads the streams and parses the
 * data for errors and completion status.
 */
static void startOne(void){
	char cmd[256];
	char auth[256];
	int	 fd;
	if ( nXfer < nPending ) {
		auth[0] = '\0';
		pendingXfer[nXfer].state = XDONE;
		pendingXfer[nXfer].dlMsg.dlStatus = 9002;
		pendingXfer[nXfer].dlMsg.dlFaultMsg = FAULT9002;
		/*
		 * Note that your wget may have different options than the ones used
		 * here.
		 */
		if ( pendingXfer[nXfer].dlMsg.userName && pendingXfer[nXfer].dlMsg.passWord )
			snprintf(auth , sizeof(auth), " --http-user=%s --http-passwd=%s ",
					pendingXfer[nXfer].dlMsg.userName, pendingXfer[nXfer].dlMsg.passWord);
		snprintf(cmd, sizeof(cmd), "/usr/bin/wget %s %s  2>&1\n",
				                    auth, pendingXfer[nXfer].dlMsg.URL);

		/* the 2>&1 also writes stderr into the stdout pipe */

		fprintf(stderr, "Start Xfer: %s", cmd);
		pendingXfer[nXfer].dlMsg.dlStartTime = time(0);
		if ((fp = popen(cmd, "r")) == NULL) {
			cpeLog(LOG_ERR, "Could not start >%s<", cmd);
			return;
		}
		/* find the socket for the popen pipe */
		if ((fd = fileno(fp)) < 0) {
			cpeLog(LOG_ERR, "Could not fileno popen stream %d(%s)",
				 errno, strerror(errno));
			return;
		}
		pendingXfer[nXfer].state = XSTARTED;
		/* Direct the CWMPC framework to call the doReadWget() function
		 * whenever there is data present on the fd socket.
		 */
		setListener(fd, doReadWget, (void*)fd);

		/*
		 * For testing purposes if the URL has the string "reboot" in it
		 * then the reboot flag is set for this test code.
		 */
		if ( strstr(pendingXfer[nXfer].dlMsg.URL, "reboot")!= NULL )
			rebootFlag = 1;

	}
}
/*
 * This function is called when there are no more transfer
 * requests waiting on their Delay timers.
 *
 * Start the transfers that
 * are queued up in the pendingXfer table. This code
 * starts one transfer at a time.
 * Currently only supports downloads from server. No upload to server.
 */
static void startXfer( void *handle){
	DBGPRINT((stderr, "----- xtartXfer\n"));
	stopCallback(&cpeState.dlQActive, startXfer, NULL); /* stop callback */
	nXfer = 0;
	startOne();
	return;
}
/*
 * This example expects the ACS to issue multiple Download
 * requests that are to be run together and their TransferComplete
 * statusus returned in the same session. This function saves the
 * DownloadMsg in an array until the last item has been received.
 * The startXfer function above is called by the CWMPc framework to
 * indicate there are no more waiting Download requests and that the
 * downloads may be started by the CPE utility functions.
 */
static void setPendingXfer( DownloadMsg *r){
	/* hold the requests for transfers
	 * make copy of necessary data from request.
	 */
	DownloadMsg *dl = &pendingXfer[nPending].dlMsg;
	pendingXfer[nPending].rpc = eDownload;
	COPYSTR( dl->URL, r->URL);
	if ( r->userName )
		COPYSTR( dl->userName, r->userName )
	else
		FREESTR(dl->userName);
	if ( r->passWord )
		COPYSTR( dl->passWord, r->passWord)
	else
		FREESTR(dl->passWord);
	if ( r->targetFileName )
		COPYSTR( dl->targetFileName, r->targetFileName)
	else
		FREESTR( dl->targetFileName );
	/*
	 * Direct the CWMP framework to call the startXfer() function
	 * when there are no more transfer requests on the delay
	 * queue.
	 */
	if ( nPending== 0)
		setCallback( &cpeState.dlQActive, startXfer,
				NULL );  /* last argument passed to startXfer() if needed */
	nPending++;
}
/* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 * The above functions are used to transfer data with non-CWMP
 * entities.
 * ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 */

#define DOWNLOADFILEPATHFORMAT DOWNLOADDIRPATH "/%s%s"
/*
 * Optional dowloadBufferHandler function that is called by
 * the CWMPc built-in downloader when the buffer specified in
 * the cwmpSetDLBufferHandler() function is filled.
 * buf may point into the buffer passed by cwmpSetDLBufferHandler.
 * The lth is greater than 0.
 * Returns
 *       0 : Continue.
 * 		-1 : Error - CWMPc will stop network transfer. The
 *           lth parameter to cpeDownloadComplete will be 0.
 *
 */
static char dlBuf[4096];
static int imageF;

static int cpeDLBufHandler( DownloadMsg *r, char *buf, int lth){
	int n;
	//DBGPRINT((stderr, "cpeDLBufHander() called: %d\n", lth));
	n = write(imageF, buf, lth);
	return (n!=lth)? -1: 0;
}

/*
* Prepare for a parameter file or flash image download.
* cpeDownloadSetup is called after the Delay time specified
* in the Download request expires.
*
* Return 1 to continue with download of the data.
*        2 indicate to the framework that the data to
*          be transfered is pending and the transfer
*          will be performed by another entity.
*        0 to abort download of the data.
*9000-9019 Abort download with DownloadResponse status set to
*          the returned value.
*/

int  cpeDownloadSetup( DownloadMsg *r)
{
	char fname[256];
	DBGPRINT((stderr, "cpeDownloadSetup() called: %s\n", r->URL));
	/*
	 * verify that the URL and fileType are present.
	 */
	if ( r->URL==NULL || r->fileType==NULL )
		return 9003;       /* invalid arguments */
	/*
	 * This code uses the interface to non-cwmpc transfers if the
	 * file type is for "X-000000 Vendor Specific ID". This is only
	 * included for testing and demonstration purposes and may not be
	 * required by a specific CPE environment.
	 */
	if ( strstr(r->fileType, "X-000000")!=NULL ) {
		/* found Vendor specific type. Setup to use transfer method
		 * external to the CWMPc.
		 */
		setPendingXfer( r );
		return 2; /* indicate to CWMPC that something else will */
				  /* transfer the data */
	}
	/* CWMPC will transfer the data using http or https */
	/* At this point we have the option of providing a buffer that will
	 * received data as it is transfered from the network or letting the
	 * CWMPc framework allocate a buffer to hold the downloaded data.
	 * The callback function parameter and the dlBuf may not be NULL.
	 * The dlBuf length must be greater than 0.
	 * If this call is not made the CWMPc allocates the buffer.
	 */
	/* For demonstration if Firmware type is specified the built-in
	 * down-loader uses CPE supplied buffer. Could be any type of file. */
	if ( strstr( r->fileType, "1 Firmware")!=NULL ) {
		/*
		 * The following is test code to force return of status values based on
		 * target file name. No data transfered.
		 */
		if ( !EMPTYSTR(r->targetFileName) && strstr(r->targetFileName,"CPE_9019"))
			return CPE_9019;
		else if ( !EMPTYSTR(r->targetFileName) && strstr(r->targetFileName,"CPE_VNDR_END"))
			return CPE_VNDR_END;
		/*
		 * The following sets a buffer hander function to be used by the download
		 * transfer function. This is optional.
		 */
		cwmpSetDLBufferHandler((void*) cpeDLBufHandler, dlBuf, sizeof(dlBuf) );

		/* open a file to save the image in, The cpeDLBufHandler function writes to this file. */
		snprintf(fname, sizeof(fname), DOWNLOADFILEPATHFORMAT,
				EMPTYSTR(r->targetFileName)? "cwmpdownloadfile": r->targetFileName, "");
		if ( (imageF=open(fname, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU|S_IRWXO|S_IRWXO))== -1)
			return 9002;
	}
	return 1;
}
/*
 * Download completed by CWMPC:
 * if cwmpSetDLBufferHandler is defined:
 *   *buf is NULL
 *   lth is total length of transfer.
 * otherwise:
 *   *buf points to the downloaded image.
 *   lth  is the length of the image or of a Parameter file.
 *
 * Return 1 if the file or image has been down loaded correctly.
 * 		 == 0 : general fault.
 *       >9000 : if the image is invalid or other error (See TR-069 spec).
 *        Vendor defined status start at 9800.
 *
 * If download was unsuccessful:
 *   lth  is 0.
 *
 * If cwmpDownloadSetBufferHandler was used to specify a callback function
 * and a buffer to use:
 *    lth is total bytes read.
 *
 * The cpeDownloadComplete() call is made to allow the
 * cpeWrapper to clean up following
 * the failed download. The wrapper should return with a 0 return
 * value.
 *
 * If the flash image is valid the CPE may proceed with
 * the flash of the image. A return to CWMPC is not necessary.
 * If control is returned to CWMPC following the flash, CWMP will
 * schedule an Inform with a TRANSFER_COMPLETE event.
 *
 * If the download was an parameter file then inspect the contents
 * of the DownloadMsg structure for the details. A return will
 * schedule an Inform with a TRANSFER_COMPLETE event.
 */

int cpeDownloadComplete(DownloadMsg *r, char *buf, int lth) {
	int f;
#if 0
	int i;
	int j;
	int ugcount =0;
	int timeout = 360;
	int c;
#endif
	ssize_t n = 0;
	int rs;
	char fname[256];
	char suffix[10];
    char cmd[256];
    char cmd_result[256];

	if ( lth==0 ) {
		// download failed. any clean up or process restart here
		return 0;
	}
	fprintf(stderr, "cpeDownloadComplete() called, lth=%d\n", lth);

	/* close the file that was opened in cpeDownloadSetup() */
	if ( strstr( r->fileType, "1 Firmware")!=NULL ) {
		close( imageF);
        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));
        sprintf(cmd,"fwhandle -x /tmp/cwmpdownloadfile| grep -r \"FWHANDLE, DECODE\" | awk '{print $3}'");
        cmd_popen(cmd,cmd_result);

        if (!strncmp(cmd_result,"FAIL",4))
        {
            fprintf(stderr, "file decode error");
            rs = CPE_9018;
            return rs;
        }
#if 0
        while(timeout != 0)
        {
            ugcount = 0;
            for (i = 0 ; i<= 2 ; i++)
            {
                for (j = 1 ; j < 4 ; j = j+2)
                {
                    memset(cmd,0x0,sizeof(cmd));
                    memset(cmd_result,0x0,sizeof(cmd_result));
                    sprintf(cmd, "wlanconfig ath%s list | awk '(NR==2){print $1}'"   , get_topology_iface_name(j));
                    cmd_popen(cmd, cmd_result);
                    c = strlen(cmd_result);
                    if( cmd_result[c-1] == '\n')
                        cmd_result[c-1] ='\0';

                    if(strlen(cmd_result) == 0)
                        ugcount++;

                    if(ugcount == 6)
                        goto upgrade;
                }
                sleep(5);
                timeout --;
                DBGLOG((DBG_TRANSFER, "Fw upgrade debug status %d", ugcount));
                DBGLOG((DBG_TRANSFER, "Timeout countdown: %d",timeout));
            }
        }
        if (timeout <=0)
        {
            rs = CPE_9019;
            return rs;
        }
upgrade:
#endif
        system("led_ctrl LED_FW_UPGRADE");
        system("echo \"sysupgrade -d 30 -c /tmp/cwmpdownloadfile\" > /tmp/cwmpc_active.sh");
        system("chmod 777 /tmp/cwmpc_active.sh;/tmp/cwmpc_active.sh &");
        rs = 1; /* ok */

	} else {
		/* write out the memory image of the data */
		if (strstr(r->fileType, "2 Web") != NULL)
			strcpy(suffix, ".2");
		else if (strstr(r->fileType, "3 Vendor") != NULL)
			strcpy(suffix, ".3");
		else
			strcpy(suffix, ".X");
		snprintf(fname, sizeof(fname), DOWNLOADFILEPATHFORMAT,
				EMPTYSTR(r->targetFileName) ? "cwmpdownloadfile"
						: r->targetFileName, ""/*suffix*/);
		if ((f = open(fname, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU | S_IRWXO
				|S_IRWXO)) != -1) {
			n = write(f, buf, lth);
			close(f);
			if (n != lth) {
				fprintf(stderr, "Error writing image n=%zd\n", n);
				rs = CPE_9010;
			} else {
                memset(cmd , 0x0 , sizeof(cmd));
                memset(cmd_result , 0x0 , sizeof(cmd_result));
                sprintf(cmd,"fwhandle -x /tmp/cwmpdownloadfile| grep -r \"FWHANDLE, DECODE\" | awk '{print $3}'");
                cmd_popen(cmd,cmd_result);
                if (!strncmp(cmd_result,"FAIL",4))
                {
                    fprintf(stderr, "file decode error");
                    rs = CPE_9018;
                    return rs;
                }
				rs = 1; /* ok */
				/* for testing if the fileType contains the substr 'Reboot' */
				/* The CWMPc will exit. The saved state can be inspected. */
				/* A restart of the CWMPc should send the TransferComplete event. */
				system("echo \"sysupgrade -d 30 -r /tmp/cwmpdownloadfile\" > /tmp/cwmpc_active.sh");
				system("chmod 777 /tmp/cwmpc_active.sh;/tmp/cwmpc_active.sh &");
				//if ( strstr(r->fileType, "Reboot"))
				//	exit(0);
			}
		} else
			rs = CPE_9010; /* error on file open */
	}
	return rs;
}

/*
 * Upload callbacks from CWMP framework.
 */
#ifdef CONFIG_RPCUPLOAD
/* cpeUploadSetup
 *  This function is called to have the CPE verify that a file or other
 *  data is ready to upload to the URL specified in the DownloadMsg.
 *  Return 0: Aborts Upload and returns a fault to the ACS.
 *         1: Continue with Upload.
 *    >=9000: Abort upload and set returned status for the
 *            pending TransferComplete.
 *        2: Queue upload request for non-cwmpc download.
 *           The CPE must provide download of the file and
 *           set status in the cpeState when complete.
 */
static int fup;
int  cpeUploadSetup( DownloadMsg *r){
	/*
	 * verify that the URL and fileType are present.
	 */
	if ( r->URL==NULL || r->fileType==NULL )
		return 9003;       /* invalid arguments */

    system("sysupgrade -b /tmp/cwmpuploadfile");

    /* figure out which file to upload based on the DownloadMsg file type here */
    /* For testing we'll just upload the cpestate.xml file */
        if ( (fup=open("/tmp/cwmpuploadfile", O_RDONLY) )) {
            fprintf(stderr, "cpeUploadSetup() opening %s status = %d\n", "/tmp/cwmpuploadfile", fup);
            r->content_type = "application/octet-stream";		/* optional content-type */
            return 1;
        }
    return 0;
}
/* cpeGetUploadData:
* This function is called following a return of 1 from the cpeUploadSetup function.
* It is called one time by the CWMPC framework to obtain an image buffer of the
* upload data. This function must set the *buf value to point to the buffer containing
* the upload data and the *lth value to the length of that data.
* The return value must be >0 for the upload to continue.
* The value of *buf is passed in the call to cpeUploadComplete at the completion of
* the upload action. The CPE must manage the buffer( alloc and free).
* Returns:
*       0 - End of data. *buf value is ignored.
*      >0 - Data has been read into buffer.
*      <0 - Some error. Abort transfer.
*/
int  cpeGetUploadData(DownloadMsg *r, char **buf, int *lth)
{
    struct stat fstatd;
    int     sz;

    fprintf(stderr, "cpeGetUploadData() ");
    fstat( fup, &fstatd );
    *lth = fstatd.st_size;
    *buf = GS_MALLOC(fstatd.st_size);
    if (*buf) {
        if ( (sz=read(fup, *buf, fstatd.st_size))>0 ){
            fprintf(stderr, " data length = %d read=%d\n", *lth, sz);
            return sz;         /* data */
        } else {
            GS_FREE (*buf );
            buf = NULL;
        }
    }
    fprintf(stderr, "cpeGetUploadData() failure err=%s\n", strerror(errno));
    return -1;
}
/*
* cpeUploadComplete
* called following a return of 0 or -1 by cpeGetUploadData.
*/
void  cpeUploadComplete( DownloadMsg *r, char *buf)
{
    fprintf(stderr, "cpeGetUploadComplete()\n");
    GS_FREE(buf);
    close(fup);
    return;

}
#endif
