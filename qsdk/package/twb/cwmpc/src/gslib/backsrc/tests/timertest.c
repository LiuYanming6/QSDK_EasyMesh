/*
 * timertest.c
 *
 *  Created on: Aug 26, 2009
 *      Author: dmounday
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/utils.h"
#include "../src/event.h"

void timerFired2( void *handle){
	int 	tnum = (int)handle;
	fprintf(stdout, "Timer2 %d expired\n", tnum);
}

void timerFired( void *handle ){
	int 	tnum = (int)handle;
	int		ticks;
	fprintf(stdout, "Timer %d expired\n", tnum);
	if ( (ticks = rand()%50)< 30) {/* restart some of them */
		fprintf(stdout, "ReSet Timer %d @ %d\n", tnum, ticks);
		if (! setTimer( timerFired2, (void *)tnum, ticks*1000	))
			fprintf(stdout, "settimer %d failed\n", tnum);

	}
}

int main(int argc, char** argv){

	int		i;
	int		tt[10];
	int		ticks;
	int		any;

	srand(1);
	initGSLib();
	for(i=1; i<=15; ++i){
		ticks = rand()%50;
		fprintf(stdout, "Set Timer %d @ %d\n", i, ticks);
		if (! setTimer( timerFired, (void *)i, ticks*1000	))
			fprintf(stdout, "settimer %d failed\n", i);
	}
	eventLoop();
	any = 0;
	for (i=1; i<10; ++i){
		any |= checkTimer(timerFired, (void *)i);
	}
	if ( any )
		fprintf(stdout, "Timers still running\n");
	else
		fprintf(stdout, "Timers expired\n");
	return 0;
}
