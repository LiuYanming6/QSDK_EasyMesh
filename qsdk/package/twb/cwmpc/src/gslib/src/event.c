/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2002 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : event.c
 * Description: (see event.h)
 *----------------------------------------------------------------------*
 * $Revision: 1.9 $
 *
 * $Id: event.c,v 1.9 2012/06/14 13:10:23 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>

/* #define USE_SYSINFO */
#if defined(linux) && defined(USE_SYSINFO)
	#include <sys/sysinfo.h>
#endif
#include <syslog.h>

#include "utils.h"
#include "event.h"

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
/*#define DEBUG*/
#undef DEBUG
/*#define DEBUG_TIMER */
/*#define DEBUG_LISTENER*/
/*#define DEBUG_IDLE*/
/*#define DEBUG_CALLBACK */

#ifdef DEBUG
	#ifndef DEBUG_TIMER
		#define DEBUG_TIMER
	#endif
	#ifndef DEBUG_LISTENER
		#define DEBUG_LISTENER
	#endif
	#ifndef DEBUG_CALLBACK
		#define DEBUG_CALLBACK
	#endif
	#ifndef DEBUG_IDLE
		#define DEBUG_IDLE
	#endif
#endif




/**********************************************************************
 * internal data structures
 **********************************************************************/
typedef struct Timer {
	struct Timer* next;
	EventHandler func;
	void* handle;
	struct timeval expire;
} Timer;

typedef struct Listener {
	struct Listener* next;
	int fd;
	EventHandler func;
	void* handle;
	tListenerType type;
} Listener;

typedef struct Callback {
	struct Callback* next;
	int deleted;
	EventHandler func;
	void* handle;
} Callback;

typedef struct Target {
	struct Target* next;
	void *target_addr;
	Callback *callbacks;
} Target;

typedef struct ActiveTarget {
	struct ActiveTarget* next;
	void *target_addr;
} ActiveTarget;

//#define USE_DYNAMIC_MEMORY_ALLOC
#ifdef USE_DYNAMIC_MEMORY_ALLOC
/*******************************************************************
 * This following macros provide linkage to the dynamic memory
 * allocation functions. The limits the event structure usage
 * is dependent on the heap memory available.
 */
#define ALLOC_TIMER()  (Timer *)GS_MALLOC(sizeof(Timer))
#define FREE_TIMER(X) GS_FREE((X))
#define ALLOC_LISTENER()  (Listener *)GS_MALLOC(sizeof (Listener))
#define FREE_LISTENER(X) GS_FREE((X))
#define ALLOC_CALLBACK() (Callback *)GS_MALLOC(sizeof (Callback))
#define FREE_CALLBACK(X) GS_FREE((X))
#define ALLOC_TARGET()	(Target *)GS_MALLOC(sizeof (Target))
#define FREE_TARGET(X)	GS_FREE((X))
#define ALLOC_ACTIVETARGET()	(ActiveTarget *)GS_MALLOC(sizeof (ActiveTarget))
#define FREE_ACTIVETARGET(X)	GS_FREE((X))
#else
/********************************************************************
 * The following structures provide a common template for allocating
 * the above event items from fixed arrays. Thus avoiding the use
 * of dynamic memory allocation functions and providing more
 * predictable operation for some system. The sizes of the tables
 * are fixed and must be tuned to the requirements of the application.
 *
 */
/*#define GSLIB_COLLECT_EVENT_USAGE */

#define MAX_TIMERS	15          /* MAXimum number of outstanding timer requests */
#define MAX_LISTENERS	15		/* Maximum number of outstanding listeners      */
#define MAX_CALLBACKS	20		/* Maximum number of registered call-back functions   */
#define MAX_TARGETS		20		/* Maximum number of registered target functions      */
#define MAX_ACTIVETARGETS 10	/* Maximum number of target function active at once	*/

typedef struct VoidItem {
	void *next;
}VoidItem;
typedef struct VoidTable {
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct VoidItem voidCommon[];

}VoidTable;
/*
 * Timer tables and allocation macros
 */
typedef struct TimerTable {
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct	Timer timer[MAX_TIMERS];
}TimerTable;

static TimerTable timerTable = {sizeof(struct Timer),MAX_TIMERS};

#define ALLOC_TIMER()  allocItem((VoidTable *)&timerTable)
#define FREE_TIMER(X) freeItem((VoidTable *)&timerTable, ((VoidItem *)X))

/*
 * Listener tables and allocation macros
 */
typedef struct ListenerTable{
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct	Listener listener[MAX_LISTENERS];
}ListenerTable;
static ListenerTable listenerTable = {sizeof(struct Listener), MAX_LISTENERS};

#define ALLOC_LISTENER()  (Listener *)allocItem((VoidTable *)&listenerTable)
#define FREE_LISTENER(X) freeItem((VoidTable *)&listenerTable, ((VoidItem *)X))

/*
 * Callback tables and allocation macros
 */
typedef struct CallbackTable{
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct	Callback callback[MAX_CALLBACKS];
}CallbackTable;
static CallbackTable callbackTable = {sizeof(struct Callback), MAX_CALLBACKS};

#define ALLOC_CALLBACK()  (Callback *)allocItem((VoidTable *)&callbackTable)
#define FREE_CALLBACK(X) freeItem((VoidTable *)&callbackTable, ((VoidItem *)X))
/*
 * Target tables and allocation macros
 */
typedef struct TargetTable{
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct	Target target[MAX_TARGETS];
}TargetTable;
static TargetTable targetTable = {sizeof(struct Target), MAX_TARGETS};

#define ALLOC_TARGET()  (Target *)allocItem((VoidTable *)&targetTable)
#define FREE_TARGET(X) freeItem((VoidTable *)&targetTable, ((VoidItem *)X));

/*
 * ActiveTarget tables and allocation macros
 */
typedef struct ActiveTargetTable{
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	int		hiWater;
	int		allocated;
	struct	ActiveTarget activeTarget[MAX_ACTIVETARGETS];
}ActiveTargetTable;
static ActiveTargetTable activeTargetTable = {sizeof(struct ActiveTarget), MAX_ACTIVETARGETS};

#define ALLOC_ACTIVETARGET()  (ActiveTarget *)allocItem((VoidTable *)&activeTargetTable)
#define FREE_ACTIVETARGET(X) freeItem((VoidTable *)&activeTargetTable, ((VoidItem *)X));

void initTable(VoidTable *tp){
	int		i;
	void *item = tp->voidCommon;
	VoidItem *last = NULL;
	i = tp->entryCount;
	while ( i-- >0 ) {
		((VoidItem *)item)->next = last;
		last = item;
		item += tp->itemSZ;
	}
	tp->freeList = last;
	tp->hiWater=tp->allocated = 0;
}

void *allocItem(VoidTable *tp){
	VoidItem *item = tp->freeList;
	if ( item ){
		tp->freeList = item->next;
		item->next = NULL;
	}
	if ( ++(tp->allocated) > tp->hiWater)
		tp->hiWater = tp->allocated;

	return item;
}

void freeItem(VoidTable *tp, VoidItem *item){
	item->next = tp->freeList;
	tp->freeList = item;
	--(tp->allocated);
}

#ifdef GSLIB_COLLECT_EVENT_USAGE
#define GUBUFSZ 512
static int countFree( VoidTable *t){
	int i = 0;
	VoidItem *ip = t->freeList;
	while ( ip ){
		++i;
		ip = ip->next;
	}
	return i;
}
char *gsLibGetUsage(){
	char	format[]="%s: size=%d hiwater=%d allocated=%d freeListSize=%d\n";
	static char	buf[GUBUFSZ];
	char	*bp = buf;
	VoidTable	*tp;
	tp = (VoidTable*)&timerTable;
	bp += snprintf(bp, GUBUFSZ, format,     "Timer       ", tp->entryCount, tp->hiWater, tp->allocated, countFree(tp));
	tp = (VoidTable*)&listenerTable;
	bp += snprintf(bp, GUBUFSZ-(buf-bp), format, "Listener    ", tp->entryCount, tp->hiWater, tp->allocated, countFree(tp));
	tp = (VoidTable*)&callbackTable;
	bp += snprintf(bp, GUBUFSZ-(buf-bp), format, "callback    ", tp->entryCount, tp->hiWater, tp->allocated, countFree(tp));
	tp = (VoidTable*)&targetTable;
	bp += snprintf(bp, GUBUFSZ-(buf-bp), format, "target      ", tp->entryCount, tp->hiWater, tp->allocated, countFree(tp));
	tp = (VoidTable*)&activeTargetTable;
	bp += snprintf(bp, GUBUFSZ-(buf-bp), format, "activeTarget", tp->entryCount, tp->hiWater, tp->allocated, countFree(tp));
	return buf;
}
#endif

#endif

/**********************************************************************
 * Globals
 *********************************************************************/

extern void cpeLog( int level, const char *fmt, ...);

static Timer* timers;
static Listener* listeners;
static Target* targets;
static ActiveTarget* scheduled_targets;
static int callback_deleted;

/**********************************************************************
 * Utilities
 *********************************************************************/

#if defined(DEBUG_TIMER) || defined(DEBUG_LISTENER) || defined(DEBUG_CALLBACK) || defined(DEBUG_IDLE)
static struct timeval ts;

static void sub(struct timeval *t0, struct timeval *t1, struct timeval *res)
{
	res->tv_sec = t1->tv_sec - t0->tv_sec;
	res->tv_usec = t1->tv_usec - t0->tv_usec;
	if (res->tv_usec < 0) {
		res->tv_sec--;
		res->tv_usec += 1000000;
	}
}

static void stop_timer(const char *tag);

static void start_timer(const char *tag, void *address)
{
#ifdef DEBUG_IDLE
	if (tag)
		stop_timer(NULL);
#endif
	gettimeofday(&ts, NULL);
	if (address)
		fprintf(stderr, ">>>%s @%p ...\n", tag, (void *) address);
}

static void stop_timer(const char *tag)
{
	struct timeval res, te;

	gettimeofday(&te, NULL);
	sub(&ts, &te, &res);
	if (tag)
		fprintf(stderr, "<<<%s (%ld.%06ld)\n", tag, res.tv_sec, res.tv_usec);
	else if (res.tv_sec > 0 || res.tv_usec > 10000)
		fprintf(stderr, "===idle (%ld.%06ld)\n",  res.tv_sec, res.tv_usec);
#ifdef DEBUG_IDLE
	if (tag)
		start_timer(NULL, NULL);
#endif
}
#endif

static void gettime(struct timeval *tm)
{
#if defined(linux) && defined(USE_SYSINFO)
	struct sysinfo si;
#endif

	gettimeofday(tm,0);

#if defined(linux) && defined(USE_SYSINFO)
	/* NB: This little trick prevents the timer queue from breaking
	 *     if someone does a settimeofday, for instance an ntpd.
	 *     The worst case is that timers get displaced one second.
	 */
	sysinfo(&si);
	tm->tv_sec=si.uptime;
#endif
}

#ifdef DEBUG_CALLBACK
/*--------------------*/
static void dump_callbacks(const char *tag, Target *t)
{
	if (t != NULL) {
		Callback *cb;

		fprintf(stderr, "DEBUG: %s ++++++++++ target=%p\n", tag, (void *) t);
		for (cb = t->callbacks; cb != NULL; cb = cb->next) {
			fprintf(stderr, "  %s %p (%p,%p)\n",
					cb->deleted ? " " : "+",
					(void *) cb, (void *) cb->func, (void *) cb->handle);
		}
		fprintf(stderr, "DEBUG: %s +++++++++++++++++++\n", tag);
	} else {
		fprintf(stderr, "DEBUG: %s ++++++++++ target=NULL\n", tag);
	}
}
#endif

/*--------------------*/
static void callCallbacks(void *target)
{
	Target *t = targets;

	while (t != NULL && t->target_addr != target) {
		t = t->next;
	}

#ifdef DEBUG_CALLBACK
	dump_callbacks("callCallbacks", t);
#endif

	if (t != NULL) {
		Callback *cb = t->callbacks;
		while (cb != NULL) {
			if (!cb->deleted) {
#ifdef DEBUG_CALLBACK
				start_timer("callback", (void *) cb->func);
#endif
				cb->func(cb->handle);
#ifdef DEBUG_CALLBACK
				stop_timer("callback");
#endif
			}
			cb = cb->next;
		}
	}
}

/*--------------------*/
static void remove_deleted_callbacks(void) {
	Target *t, **th = &targets;

	while ((t = *th) != NULL) {
		Callback *cb, **cbh = &t->callbacks;
		while ((cb = *cbh) != NULL) {
			if (cb->deleted) {
				*cbh = cb->next;
				FREE_CALLBACK(cb);
			} else
				cbh	= &cb->next;
		}
		if (t->callbacks == NULL) {
			*th = t->next;
			FREE_TARGET(t);
		} else
			th = &t->next;
	}
}

/*--------------------*/
static int checkCallbacks(ActiveTarget **ath)
{
	if (*ath == NULL)
		return 0;
	callback_deleted = FALSE;
	if ((*ath)->next != NULL)
		checkCallbacks(&((*ath)->next));
	callCallbacks((*ath)->target_addr);
	FREE_ACTIVETARGET(*ath);
	*ath = NULL;
	if (callback_deleted)
		remove_deleted_callbacks();
	return 1;
}

/**********************************************************************
 * Timers
 *********************************************************************/

/*--------------------*/
static void callbackTimer(void *handle)
{
	ActiveTarget *active_targets = scheduled_targets;
	scheduled_targets = NULL;
	checkCallbacks(&active_targets);
}

/**********************************************************************
 * Public functions
 *********************************************************************/

/*--------------------*/
void stepTime(struct timeval *tv) {
	Timer *t;
	for (t = timers; t; t = t->next) {
		t->expire.tv_sec += tv->tv_sec;
		t->expire.tv_usec += tv->tv_usec;
		if (t->expire.tv_usec > 1000000) {
			t->expire.tv_sec++;
			t->expire.tv_usec -= 1000000;
		}
	}
}

/*--------------------*/
void stopTimer(EventHandler func, void *handle)
{
	Timer* t;
	Timer** th = &timers;

	while ((t = *th)) {
		if (t->func == func && t->handle == handle) {
			*th = t->next;
			FREE_TIMER(t);
		} else {
			th =& t->next;
		}
	}
}

void stopAllTimers(void)
{
	Timer* t;

	while ( timers!=NULL ) {
		t = timers->next;
		FREE_TIMER(timers);
		timers = t;
	}
}
/*--------------------*/
int checkTimer(EventHandler func, void *handle)
{
	Timer* t;

	for (t = timers; t; t = t->next) {
		if (t->func == func && t->handle == handle) {
			return 1;
		}
	}
	return 0;
}

/*--------------------*/
/* returns: 0 - unable to malloc timer structure */
/*          1 - timer queued */
int setTimer(EventHandler func, void *handle, int ms)
{
	Timer* t;
	Timer* t1;
	Timer** th = &timers;
	struct timeval tm;

	stopTimer(func, handle);

	gettime(&tm);
	addMs(&tm, &tm, ms);

	while ((t = *th)) {
		if (cmpTime(&tm, &t->expire) < 0) {
			break;
		} else {
			th = &t->next;
		}
	}

	if ( (t1 = ALLOC_TIMER())==0)
		return 0;
	t1->func = func;
	t1->handle = handle;
	t1->expire.tv_sec = tm.tv_sec;
	t1->expire.tv_usec = tm.tv_usec;

	t1->next = t;
	*th = t1;
	return 1;
}

void stopListener(int fd)
{
	Listener* l;
	Listener** lh=&listeners;

	while ((l=*lh)) {
		if (l->fd==fd) {
			*lh=l->next;
			FREE_LISTENER(l);
		} else {
			lh=&l->next;
		}
	}
}

/*--------------------*/
void setListener(int fd, EventHandler func, void* handle)
{
	setListenerType(fd, func, handle, iListener_Read);
}

/*--------------------*/
void setListenerType(int fd, EventHandler func, void* handle, tListenerType type)
{
	Listener* l;

	stopListener(fd);

	if ( (l = ALLOC_LISTENER()) ){
		l->fd = fd;
		l->func = func;
		l->handle = handle;
		l->next = listeners;
		l->type = type;
		listeners = l;
	}
}

/*--------------------*/
static Callback *lookup_callback(void *target, EventHandler func, void *handle)
{
	Target *t;

	for (t = targets; t != NULL; t = t->next) {
		if (t->target_addr == target) {
			Callback *cb;
			for (cb = t->callbacks; cb != NULL; cb = cb->next) {
				if (cb->func == func && cb->handle == handle) {
					return cb;
				}
			}
		}
	}
	return NULL;
}

/*--------------------*/
void stopCallback(void *target, EventHandler func, void *handle)
{
	Callback *cb;

	if ((cb = lookup_callback(target, func, handle)) != NULL) {
		cb->deleted = TRUE;
		callback_deleted = TRUE;
	}
}

/*--------------------*/
void setCallback(void *target, EventHandler func, void *handle)
{
	Target *t;
	Callback *cb;

	/* lookup callback */
	if ((cb = lookup_callback(target, func, handle)) != NULL) {
		/* callback already exists, ignore */
		cpeLog(LOG_DEBUG,"setCallback(), callback %p/%p aready exists (ignore).\n", (void *) func, handle);
		return;
	}

#ifdef DEBUG_CALLBACK
	fprintf(stderr, "DEBUG: +++ setCallback(%p, %p, %p)\n", target, (void *) func, handle);
#endif

	t = targets;
	if ( (cb = ALLOC_CALLBACK())){
		cb->deleted = FALSE;
		cb->handle = handle;
		cb->func = func;
		while (t != NULL) {
			if (t->target_addr == target)
				break;
			t = t->next;
		}
	} else
		return; /* no memory just return */

#ifdef DEBUG_CALLBACK
	dump_callbacks("PRE setCallback", t);
#endif

	if (t == NULL) {
		if ( (t = ALLOC_TARGET() )){
			t->target_addr = target;
			t->callbacks = NULL;
			t->next = targets;
			targets = t;
		} else {
			FREE_CALLBACK(cb); /* no memory, free cb and return */
			return;
		}
	}
	cb->next = t->callbacks;
	t->callbacks = cb;

#ifdef DEBUG_CALLBACK
	dump_callbacks("POST setCallback", t);
#endif
}

/*--------------------*/
void notifyCallbacks(void *target) {
	ActiveTarget *at = ALLOC_ACTIVETARGET();
	if (at) {
		at->target_addr = target;
		at->next = scheduled_targets;
		scheduled_targets = at;
		if (!checkTimer(callbackTimer, NULL))
			setTimer(callbackTimer, NULL, 0);
	}
}

void notifyCallbacksAndWait(void *target)
{
	ActiveTarget *at = ALLOC_ACTIVETARGET();
	if ( at ){
		at->target_addr = target;
		at->next = NULL;
		checkCallbacks(&at);
	}
}
#if 0
static int validateListener( Listener *l)
{
	struct stat buf;
	if ( fstat( l->fd, &buf)==0 )
		return 1;
	cpeLog(LOG_CRIT, "Invalid listener in listeners list( fd=%d)", l->fd);
	return 0;
}
#endif
/*--------------------*/
void eventLoop(void)
{
	while (timers || listeners) {
		int n;
		int res;
		fd_set rfds, wfds, efds;
		struct timeval tm;
		Listener* l;

		n = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_ZERO(&efds);
		for (l = listeners; l; l = l->next) {
			switch (l->type) {
			case iListener_Read:
				FD_SET(l->fd, &rfds);
				break;
			case iListener_Write:
				FD_SET(l->fd, &wfds);
				break;
			case iListener_ReadWrite:
				FD_SET(l->fd, &rfds);
				FD_SET(l->fd, &wfds);
				break;
			case iListener_Except:
				FD_SET(l->fd, &efds);
				break;
			default:
				fprintf(stderr, "Impossible error: eventLoop(): illegal listener type (%d)\n", l->type);
				break;
			}
			if (n <= l->fd) {	   /* update value of n for select(n, ... */
				n = l->fd + 1;
			}
		}

		if (timers) {
			gettime(&tm);
			subTime(&tm, &timers->expire);
			if (tm.tv_sec < 0) {		  /* if timer has expired then remove from timers queue*/
				Timer* t = timers;			/* and call t->func */
				EventHandler func = t->func;
				void* handle = t->handle;

				timers = t->next;
				FREE_TIMER(t);
#ifdef DEBUG_TIMER
				start_timer("timer", (void *) func);
#endif
				func(handle);
#ifdef DEBUG_TIMER
				stop_timer("timer");
#endif
				continue;
			}
			/*fprintf(stderr, "select %d t=%d.%d [", n,tm.tv_sec,tm.tv_usec);*/
			res = select(n, &rfds, &wfds, &efds, &tm);
		} else {
			/*fprintf(stderr, "select %d [",n);*/
			res = select(n, &rfds, &wfds, &efds, 0);
		}

		if (res < 0 && errno != EINTR) {
			cpeLog(LOG_CRIT, "Error in select (%d=%s)", errno, strerror(errno));
			continue; /*??????????*/
			/*return;*/
		}

		/* the select results are processed */
		/* res is decremented for each fd that appears in one or more sets */
		/* if an fd appears in only one of the sets then res is the loop count */
		/* otherwise we stop at the end of the listener list */
		for (l = listeners; l != NULL && res>0; ) {
			if (FD_ISSET(l->fd, &rfds) || FD_ISSET(l->fd, &wfds) || FD_ISSET(l->fd, &efds)) {
				/* Clear the fd in the fdset so we don't call the func again */
				FD_CLR(l->fd, &rfds); FD_CLR(l->fd,&wfds); FD_CLR(l->fd,&efds);

				l->func(l->handle);	   /* l is invalid following this call */
				l = listeners;		   /* callback may have changed listeners list, need */
									   /* to start over at the beginning */
				--res;
			} else
				l = l->next;
		}
	}
}

/*--------------------*/

void closeListenerFD( int fd )
{
	stopListener(fd);
	close (fd);
	return;
}

void closeAllFds(void)
{
	while (listeners!=NULL){
		closeListenerFD(listeners->fd);
	}
}

void initGSLib(void){
#ifndef USE_DYNAMIC_MEMORY_ALLOC
	initTable((VoidTable*)&timerTable);
	initTable((VoidTable*)&callbackTable);
	initTable((VoidTable*)&listenerTable);
	initTable((VoidTable*)&targetTable);
	initTable((VoidTable*)&activeTargetTable);
#endif
}
