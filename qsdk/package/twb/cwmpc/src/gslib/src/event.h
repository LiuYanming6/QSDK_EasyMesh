/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2002 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : event.h
 *
 * Description:
 *   Event mechanism that supports simultaneous dispatching of events
 *   from several input sources and timers.
 *----------------------------------------------------------------------*
 * $Revision: 1.5 $
 *
 * $Id: event.h,v 1.5 2012/06/13 11:07:12 dmounday Exp $
 *
 * $Log: event.h,v $
 * Revision 1.5  2012/06/13 11:07:12  dmounday
 * Added stopAllTimers() and fixed bug in closeAllFds(). This was to support the STOPCWMPc feature.
 *
 * Revision 1.4  2011/10/26 10:30:57  dmounday
 * Removed extraneous revision comments.
 *
 * Revision 1.3  2009/09/03 14:50:32  dmounday
 * Added tests of results of all GS_MALLOC() () function calls.
 * The default build condition is to not use dynamic memory allocation for the event and timer management functions. The events, timers and registered callback functions are allocated from static array entries. The array entries are maintained as linked lists of generic items. The linked lists must be initialized by a call to the function initGSLib().
 *
 * The internal allocation statistics are returned by a call to the conditionally compiled function gsLibGetUsage().
 *
 * Revision 1.2  2006/12/12 17:22:16  dmounday
 * General updates, spelling errors, etc.
 *
 * Revision 1.1.1.1  2006/08/18 17:18:55  dmounday
 * Initial Import cwmp sources
 *
 *
 *----------------------------------------------------------------------*/

#ifndef EVENT_H
#define EVENT_H

/**********************************************************************
 * typedefs
 *********************************************************************/

/*----------------------------------------------------------------------
 * type of listener, see select() for more information
 */
typedef enum {
  iListener_Read,
  iListener_Write,
  iListener_Except,
  iListener_ReadWrite
} tListenerType;


/*
 * listeners are all on form
 *    void f(void *) {}
 */
typedef void (*EventHandler)(void*);

/**********************************************************************
 * public functions
 *********************************************************************/

void stepTime(struct timeval *tv);
void stopTimer (EventHandler func, void *handle);
int  checkTimer(EventHandler func, void *handle);
int  setTimer  (EventHandler func, void *handle, int ms);

void stopListener(int fd);
void setListener (int fd, EventHandler func, void* handle);
void setListenerType(int fd, EventHandler func, void* handle, tListenerType type);

void stopCallback(void *target, EventHandler func, void *handle);
void setCallback(void *target, EventHandler func, void *handle);
void notifyCallbacks(void *target);
void notifyCallbacksAndWait(void *target);
void eventLoop(void);
void closeAllFds(void);
void closeListenerFD(int fd);
void stopAllTimers(void);
void initGSLib(void);

#endif
