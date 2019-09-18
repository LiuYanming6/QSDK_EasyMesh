#ifndef _NOTIFY_H_
#define _NOTIFY_H_
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : notify.h
 * Description:	Notification implementation
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 *
 * $Id: notify.h,v 1.3 2009/11/20 14:49:14 dmounday Exp $
 *----------------------------------------------------------------------*/

/* paramCopy call-back update parameter mask */
#define	UPDATE_COPY	0x1		/* update the copy of the parameter data */
#define	INIT_NOTIFY	0x2		/* copy the default Notify bit values    */

void cwmpInitNotifyTracking(int mask);
int  cwmpCheckNotification( XMLWriter *xp, int *paramCnt, int update);
int cwmpResetActiveNotification(void);
int cwmpAnyActiveNotifications(void);
void cwmpSaveNotifyAttributes(void);
void cwmpRestoreNotifyAttributes(void);


#endif /* _NOTIFY_H_ */
