/*
 * memtest.c
 *
 *  Created on: Aug 24, 2009
 *      Author: dmounday
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/utils.h"

int main(int argc, char** argv){

	int		i;
	void *ptr[100];
	char	teststr[]= "teststring to dup";
	char *sp[100];;
	/* memory functions called */
	for (i=0; i<100; ++i){
		if ( (ptr[i]=gs_malloc_wrapper(i*100))==NULL){
			fprintf(stdout,"Allocation failed %d s=%d\n", i, i*100);
		}
		if ( (sp[i]=gs_strdup_wrapper(teststr))==NULL){
			fprintf(stdout,"strdup failed %d\n", i);
		}
		if ( ptr[i]!= NULL ){
			if ( (ptr[i]=gs_realloc_wrapper(ptr[i], i*100+10)) == NULL)
				fprintf(stdout, "realloc failed %d  s=%d\n", i, i*100+10);
		}
		if ( sp[i] ) {
			gs_free_wrapper(sp[i]);
			sp[i] = NULL;
		}
	}
	for (i=0; i<100; ++i){
		gs_free_wrapper(ptr[i]);

	}

}

