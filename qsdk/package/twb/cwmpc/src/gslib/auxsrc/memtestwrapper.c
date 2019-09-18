/*
 * memtests.c
 *
 * This contains the memory boundary test functions for malloc(), realloc() and strdup().
 * The macros in utils.h are redefined to reference these functions.
 *  Created on: Aug 24, 2009
 *      Author: dmounday
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/utils.h"

typedef struct MemHdr{
	size_t	alloc_size;
	int		alloc_seq;
	char	align_pad[8];
	char	user_buf[0];
}MemHdr;
#define HDRSZ sizeof(struct MemHdr)

/*
 * fail conditions
 *
 */
typedef struct SeqFail{
	int		startSeq;
	int		endSeq;
}SeqFail;
typedef struct TotalFail{
	size_t	startLimit;			/* Fail at this  total requested */
	int		numberAlloc;		/* number of times after startLimit */
	int		fails;
}TotalFail;
typedef struct CurTotal{
	size_t 	hiLimit;			/* fail when cur_total higher */
	size_t	loLimit;			/* reset fail when cur_total lower than */
	int		hiFailOn;			/* hi fail is on. fail until for low limit */
}CurTotal;
typedef struct ModSeq{
	int		denom;           /* fail when sequence%denom == remainder */
	int		remainder;
	size_t	hiLimit;		/* if hiLimit>0 and cur_total>hiLimit  */
	size_t	request;		/* and if request!=0 if size > request */
}ModSeq;
typedef struct SizeRequest{
	int		startSeq;        /* start checking at sequence */
	size_t	hiLimit;         /* if cur_total is > hiLimit  */
	size_t	request;         /* and size is > request fail      */
	int		recovery;        /* until fail count is == recovery */
	int		failCnt;
}SizeRequest;

#define MAXSEQFAILS 20
SeqFail seqFails[MAXSEQFAILS];
int	numSeqFails;
TotalFail reqTotal;
CurTotal curTotal;
ModSeq   modSeq;
SizeRequest sizeReq;


static int	  sequence;
static size_t cur_total;
static size_t total_requested;
static size_t total_freed;

static int	num_mallocs;
static int	num_reallocs;
static int 	num_frees;
static int	num_strdups;
static int  num_strndups;

/*
 * read test conditions. One line per condition with following format:
 * One T line, multiple S lines allowed.

T	35000  2            Fail 2 time when total request is more than 35000
S   20   5              Fail 5 times when allocation sequence is 20.
S   100  1
S   200  10
C	350000 345000       Fail when total current is 350000, stops when down to 345000
M   100  0  hiLimit  [size]     Fail on sequence%100 == 0 if hiLimit>0 && cur_total>hiLimit size >0 and requested size > size.
Z   <seq> <curtotal> <request>	<recovery> Fail when  seqence is > seq and total current is > 350000 and request is >1000.
                                           until failures equal recover count.
*/
void readConditions(void){
	FILE 	*f;
	char	line[100];
	char	condition;
	int		f1;
	int		f2;
	int		xx;
	int		yy;

	if ( (f=fopen("mem_testrules","r")) ){
		fprintf(stdout,"Memory test wrapper file: mem_testrules found !!!!!!!!!!\n");
		while ( fgets(line, sizeof(line),f)!=NULL){
			xx = yy = 0;
			if ( sscanf(line, "%c %d %d %d %d", &condition, &f1, &f2, &xx, &yy) >= 3 ){
				fprintf(stdout, "Test Condition: %c %d %d %d %d\n", condition, f1, f2, xx, yy);
				if ( condition=='T'){
					reqTotal.startLimit = f1;
					reqTotal.numberAlloc = f2;
				} else if ( condition == 'S' && numSeqFails<MAXSEQFAILS){
					seqFails[numSeqFails].startSeq = f1;
					seqFails[numSeqFails].endSeq = f1 + f2;
					numSeqFails++;
				} else if ( condition == 'C'){
					curTotal.hiLimit = f1;
					curTotal.loLimit = f2;
				} else if ( condition == 'M'){
					modSeq.denom = f1;
					modSeq.remainder = f2;
					modSeq.hiLimit = xx;
					modSeq.request = yy;
				} else if ( condition == 'Z'){
					sizeReq.startSeq = f1;
					sizeReq.hiLimit = f2;
					sizeReq.request = xx;
					sizeReq.recovery = yy;
				}

			}
		}
		fclose(f);
	}
}

static void seqTest(void){
	if ( sequence==0 )
		readConditions();
	++sequence;
}

static int fail(size_t sz){
	int result = 0;
	int i;
	if ( reqTotal.startLimit > total_requested ){
		if ( reqTotal.fails++ < reqTotal.numberAlloc )
			result = 1;
	}
	if ( curTotal.hiLimit > cur_total || curTotal.hiFailOn ){
		curTotal.hiFailOn = 1;
		if ( cur_total <= curTotal.loLimit ){
			curTotal.hiFailOn = 0;
		} else
			result = 1;
	}
	if ( modSeq.denom != 0 ){
		if ( sequence%modSeq.denom == modSeq.remainder ){
			if ( modSeq.hiLimit>0 && cur_total>modSeq.hiLimit ){
				if ( modSeq.request !=0 ){
					if (sz > modSeq.request )
						result = 1;
				} else
					result= 1;
			} else {
				if ( modSeq.request !=0 ){
					if (sz > modSeq.request )
						result = 1;
				}
			}
		}
	}
	for (i=0; i<numSeqFails; ++i){
		if ( sequence >= seqFails[i].startSeq
		 && sequence <seqFails[i].endSeq )
			result = 1;
	}
	if ( sizeReq.hiLimit>0){
		if ( sequence >=sizeReq.startSeq
		   && sz > sizeReq.request
		   && cur_total > sizeReq.hiLimit){
			++sizeReq.failCnt;
			if (sizeReq.recovery && sizeReq.failCnt>sizeReq.recovery){
				sizeReq.hiLimit = 0;   /* disable condition */
			} else {
				result = 1;
			}
		}
	}
	if ( result == 1 )
		fprintf(stdout, "Memory allocation failed at sequence %d cur_total=%d size=%d\n", sequence, cur_total, sz );
	return result;
}

static void inc_totals( size_t s){
	cur_total += s;
}

static void dec_totals( size_t s){
	cur_total -= s;
	total_freed += s;
}
static void* get_buffer( size_t size ){
	MemHdr *ptr;
	if ( fail(size) )
		return NULL;
	if ( (ptr=(MemHdr *) malloc(size+sizeof( MemHdr )+16))!=NULL ){
		ptr->alloc_size = size;
		ptr->alloc_seq = sequence;
		inc_totals( size );
		return (void *)(ptr->user_buf);
	}
	return NULL;
}
void *gs_malloc_wrapper(size_t size){
	++num_mallocs;
	seqTest();
	total_requested += size;
	return get_buffer(size);
}

void *gs_realloc_wrapper(void *ptr, size_t size){
	void *rptr;
	MemHdr *h;
	size_t oldsize;
	++num_reallocs;
	seqTest();
	if ( ptr!=NULL){
		h = (MemHdr *)(ptr-HDRSZ);
		oldsize = h->alloc_size;
		dec_totals(oldsize);
		if ( size==0 ){
			free(ptr);
			rptr = NULL;
		} else {
			total_requested += size;
			if ( fail(size) )
				rptr = NULL;
			else {
				rptr = realloc( (void *)h, size+HDRSZ+16);
				((MemHdr *)rptr)->alloc_size = size;
				rptr += HDRSZ;
				inc_totals(size);
			}
		}
	} else {
		total_requested += size;
		rptr = get_buffer(size);
	}
	return rptr;
}

char *gs_strdup_wrapper(const char *str){
	char *ptr;
	int lth = strlen(str)+1;
	++num_strdups;
	seqTest();
	total_requested += lth;
	if ( (ptr = get_buffer(lth)))
		memcpy(ptr, str, lth);
	return ptr;
}

/*
 * The strndup() function copies at most n characters. If str is longer than n, only  n
 * characters are copied, and a terminating NUL is added.
 *
 */
char *gs_strndup_wrapper(const char *str, size_t n){
	char *ptr;
	int	 lth;

	lth = strlen(str)+1;
	if ( n+1 < lth )
		lth = n+1;
	++num_strndups;
	seqTest();
	total_requested += (lth);
	ptr = get_buffer(lth);
	if ( ptr ) {
		memcpy(ptr, str, lth-1);
		*(ptr+lth-1) = '\0';
	}
	return ptr;
}

void gs_free_wrapper(void *ptr){
	MemHdr *h;
	if ( ptr != NULL){
		h = ptr-HDRSZ;
		++num_frees;
		dec_totals( h->alloc_size);
		free((void* )h );
	}
	return;
}



