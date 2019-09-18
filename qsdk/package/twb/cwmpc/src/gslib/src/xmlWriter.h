#ifndef _XMLWRITER_H_
#define _XMLWRITER_H_
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : xmlWriter.h
 * Description:	XML Write utility functions.
 *----------------------------------------------------------------------*
 * $Revision: 1.3 $
 * $Id:
 *----------------------------------------------------------------------*/
#include <sys/types.h>

#define	XML_SURPRESS_LF 0x02
#define	XML_NOINDENT	0x04
#define XML_NOREALLOC	0x08


#define INDENT_STR	"  "
#define REALLOC_MEM_CHUNK 4096

#ifndef DMALLOC
#define XML_DFLT_MALLOCSZ 1024
#else
#define XML_DFLT_MALLOCSZ 64000
#endif

#define STARTTAG  0x00000001   /* this is a xmlStartTagGrp (not a OpenTagGrp)*/
#define OPENTAG   0x00000002   /* xmlOpenTagGrp */
#define CHILDTAG  0x00000004   /* a child tag was stacked onto this tag - use </tag> to close */

struct XMLTagStackItem;
typedef struct XMLTagStackItem {
	struct XMLTagStackItem *next;
	char   *tag;
	int    tagFlag;  /* set if item was xmlStartTagGrp, 0 is xmlOpenTagGrp*/
} XMLTagStackItem;

typedef struct XMLWriter {
	char           	*buf;	   /* current buffer */
	int            	index;	   /* index of next write */
	int		       	bufSize;	   /* current size of buffer */
	int				maxSize;	   /* max size buffer allowed to grow */
	int            	tagDepth;   /* initially zero */
	XMLTagStackItem *tagStack;	/* stack of current tags */
	int            	flags;	   /* */
}XMLWriter;

XMLWriter *xmlOpenWriter( size_t initSize, int flags);
void       xmlCloseWriter( XMLWriter *);

int		xmlResetBufIndex( XMLWriter *xp );
int     xmlOpenTagGrp(XMLWriter *, const char *tagName, ...);
int     xmlCloseTagGrp(XMLWriter *);
int     xmlStartTagGrp(XMLWriter *, const char *tagName, ...);
int     xmlEndTagGrp(XMLWriter *);
int     xmlMemPrintf(XMLWriter *, const char *fmt, ...);
int     xmlPrintf(XMLWriter *, const char *);
char    *xmlGetBufPtr(XMLWriter *);
int		xmlGetMsgSize(XMLWriter *);
char 	*xmlSaveCloseWriter(XMLWriter *xp);
int 	xmlPrintTaggedAttrib( XMLWriter *xp, const char *tag, char *fmt, ...);
int     xmlPrintTaggedData( XMLWriter *xp, const char *tag, char *fmt, ...);

#endif /* _XML_WRITER_H */

