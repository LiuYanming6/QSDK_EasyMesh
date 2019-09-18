/*
 * allocItem.c
 *
 *  Created on: Aug 25, 2009
 *      Author: dmounday
 *
 *  This implements allocation and release of items in an array. The items are a
 *  structure containing at least a *next pointer link and a allocation status cell at
 *  the beginning of each structure.
 *
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/utils.h"

#define USE_ARRAYS
#define MAX_TIMERS  10

/*
 *   #define ALLOC_TIMER  GS_MALLOC(sizeof Timer)
 *   #define FREE_TIMER(X) GS_FREE(X)
 *   or
 *   #define ALLOC_TIMER  allocItem((TimerTable *)&timerTable)
 *   #define FREE_TIMER(X) freeItem((TimerTable *)&timerTable, (void *)X);
 */
 #define ALLOC_TIMER()  allocItem((VoidTable *)&timerTable)
 #define FREE_TIMER(X) freeItem((VoidTable *)&timerTable, (void *)X);
typedef struct Timer {
	struct Timer* next;
	void* func;
	void* handle;
	struct timeval expire;
} Timer;


typedef struct VoidItem {
	void *next;
	char	data[];
}VoidItem;
typedef struct VoidTable {
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	struct VoidItem voidCommon[];

}VoidTable;

typedef struct TimerTable {
	int		itemSZ;
	int		entryCount;
	void	*freeList;
	struct	Timer timer[MAX_TIMERS];
}TimerTable;



static Timer *unusedTimers;
static Timer *timers;

void initTable(VoidTable *tp){
	int		i;
	void *item = tp->voidCommon;
	VoidItem *last = NULL;
	i = tp->entryCount;
	while ( i-- >0 ) {
		((VoidItem *)item)->next = last;
		last = item;
		item += tp->itemSZ;
		fprintf(stdout, "Item at %p\n", item);
	}
	tp->freeList = last;
}

void *allocItem(VoidTable *tp){
	VoidItem *item = tp->freeList;
	if ( item ){
		tp->freeList = item->next;
		item->next = NULL;
	}
	return item;
}

void freeItem(VoidTable *tp, VoidItem *item){
	item->next = tp->freeList;
	tp->freeList = item;
}

void *initLibTables(void){
	initTable((VoidTable*)&timerTable);
}



int main(int argc, char** argv){

	int		i;
	Timer	*tList[MAX_TIMERS+1];
	Timer	*tp;

	initLibTables();

	i = 0;
	do {
		//tp = allocItem( (VoidTable *) &timerTable );
		tp = ALLOC_TIMER();
		tList[i++] = tp;
	} while ( tp!= NULL);
	--i;
	fprintf(stdout,"%d timers allocated\n", i);
	if ( timerTable.freeList == NULL )
		fprintf(stdout, "  FreeList is NULL\n");
	else
		fprintf(stdout, "  Error: freelist is not NULL");
	while ( i ){
		tp = tList[--i];
		//freeItem( (VoidTable *)&timerTable, (VoidItem *)tp);
		FREE_TIMER( tp );
	}
	fprintf(stdout, "timers freed\n");
	i = 0;
	tp = timerTable.freeList;
	while ( tp ){
		++i;
		tp = tp->next;
	}
	fprintf(stdout, "  FreeList has %d items\n", i);
}



