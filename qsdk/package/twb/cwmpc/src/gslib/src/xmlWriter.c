/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : xmlWriter.h
 * Description:	XML Write utility functions.
 *----------------------------------------------------------------------*
 * $Revision: 1.6 $
 * $Id:
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <syslog.h>
#include <ctype.h>

#include "utils.h"

#ifdef DMALLOC
	#include "dmalloc.h"
#endif
#include "xmlWriter.h"
extern void cpeLog( int level, const char *fmt, ...);

static void  pushTag(XMLWriter *xp, char *tagBuf, int tagFlag)
{
	XMLTagStackItem *s;
	s = GS_MALLOC( sizeof(XMLTagStackItem));
	if (s) {
		int sz = strcspn(tagBuf, " ");	 /* isolate tag name only */
		s->next = xp->tagStack;
		if (xp->tagStack)
			xp->tagStack->tagFlag |= CHILDTAG;
		xp->tagStack = s;
		++xp->tagDepth;
		s->tagFlag = tagFlag;
		s->tag = (char *)GS_STRNDUP(tagBuf, sz);
	}
	return;
}

XMLWriter *xmlOpenWriter( size_t initSize, int flags)
{
	XMLWriter *xp= (XMLWriter *)GS_MALLOC(sizeof(XMLWriter));
	if (xp) {
		memset(xp,0, sizeof(XMLWriter));
		xp->flags = flags;
		xp->bufSize = initSize==0? XML_DFLT_MALLOCSZ: initSize;
		xp->buf = GS_MALLOC(xp->bufSize);
		if ( xp->buf == NULL){
			GS_FREE(xp);   /* buf allocation failed -- clean up*/
			cpeLog(LOG_ERR, "xmlOpenWrite buffer allocation failed");
			xp = NULL;
		}
	}
	return xp;
}
/*
 * Flush any pushed tag groups.
 */
static void flushTagGroups(XMLWriter *xp){
	XMLTagStackItem *s = xp->tagStack;
	XMLTagStackItem *next;
	while (s) {
		next = s->next;
		GS_FREE(s->tag);
		GS_FREE(s);
		s = next;
	}
	xp->tagStack = NULL;
}

void xmlCloseWriter( XMLWriter *xp)
{
	if (xp) {
		flushTagGroups(xp);
		if (xp->buf) GS_FREE(xp->buf);
		GS_FREE (xp);
	}

}
/*
* xmlSaveBufPtr
* Return buffer pointer to caller and close xmlWriter.
*/
char *xmlSaveCloseWriter(XMLWriter *xp)
{
	char *p;
	if (xp) {
		flushTagGroups(xp);
		p = xp->buf;
		xp->buf = NULL;
		GS_FREE(xp);
	} else
		p = NULL;
	return p;
}
/*
* return pointer to xml buffer
*/
char   *xmlGetBufPtr(XMLWriter *xp)
{
	if (xp)
		return xp->buf;
	return NULL;
}
/*
* Return current buffer content size.
*/
int		xmlGetMsgSize(XMLWriter *xp)
{
	return xp? xp->index: 0;
}
/*
* xmlResetBuf:
 * Return n: Size of buffer.
 * Reset the current index to zero and the number of characters
 * left to the current buffer size. The tag depth and indent
 * is left unchanged.
 *
 */

int xmlResetBufIndex( XMLWriter *xp )
{
	xp->index = 0;
	return xp->bufSize;
}

int xmlMemIndent(XMLWriter *xp) {
	int i, left, n;
	char ibuf[512];

	if (xp->buf) {
		/* if active buffer does not end with \n then just return*/
		if (xp->index > 0 && (*(xp->buf + xp->index - 1) != '\n'))
			return xp->index;
		ibuf[0] = '\0';
		for (i = 0; i < xp->tagDepth; ++i) {
			strcat(ibuf, INDENT_STR);
		}
		left = xp->bufSize - xp->index;
		if ((n = strlen(ibuf)) >= left ) {
			if ( !(xp->flags & XML_NOREALLOC)) {
				/* enlarge buffer size  */
				char *buf;
				if ((buf = GS_REALLOC(xp->buf, xp->bufSize + REALLOC_MEM_CHUNK))
						==NULL) {
					cpeLog(LOG_ERR, "xmlMemIndent: out of memory");
					GS_FREE(xp->buf);
					xp->buf = NULL;
					return 0;
				}
				xp->buf = buf;
				xp->bufSize += REALLOC_MEM_CHUNK;
			} else {
				/* fixed size buffer overflow. just clean up */
				GS_FREE(xp->buf);
				xp->buf = NULL;
				return 0;
			}
		};
		strcpy(xp->buf + xp->index, ibuf);
		xp->index += n;
		return xp->index;
	}
	return 0;
}
/*----------------------------------------------------------------------*
* xml memory print with expanding buffer.
* Arguments:  *XMLWriter - open XMLWriter structure
*             *fmt: format string.
*            ...: optional args determined by fmt string.
* Returns: 0 - if buffer is full and can not be expanded.
*          >0 - current index into buffer (active bytes from writes
*/

int xmlMemPrintf(XMLWriter *xp, const char *fmt, ...) {
	int n;
	va_list ap;
	int left;

	if ((xp->flags & XML_NOINDENT) != XML_NOINDENT)
		xmlMemIndent(xp);
	if (xp->buf) {
		left = xp->bufSize - xp->index;
		va_start(ap, fmt);
		n = vsnprintf(xp->buf + xp->index, left, fmt, ap);
		va_end(ap);
		if ( n+1 >= left ) {
			/* vsnprintf overflowed buffer */
			if ( !(xp->flags&XML_NOREALLOC)){
				/* alloc more to buffer  */
				char *buf;
				int allocSz;
				/* allocate integral number of chunk sizes */
				allocSz = ((n+1+REALLOC_MEM_CHUNK-1)/REALLOC_MEM_CHUNK)*REALLOC_MEM_CHUNK;
				if ((buf = GS_REALLOC(xp->buf, xp->bufSize + allocSz))==NULL) {
					cpeLog(LOG_ERR, "xmlMemPrintf: out of memory");
					GS_FREE(xp->buf);
					xp->buf = NULL;
					return 0;
				}
				xp->buf = buf;
				xp->bufSize += allocSz;
				left += allocSz;
				va_start(ap, fmt);
				n = vsnprintf(xp->buf + xp->index, xp->bufSize - xp->index,	fmt, ap);
				va_end(ap);
				xp->index += n;
				return xp->index;
			} else {
				/* fixed sized buffer is full -- clean up */
				GS_FREE(xp->buf);
				xp->buf = NULL;
				return 0;
			}
		} else {
			return xp->index +=n;
		}
	}
	return 0;
}

static int putParentEnding(XMLWriter *xp)
{
	char *parentEnd;
	if (xp->tagStack==NULL)
		parentEnd = NULL;
	else if ( (xp->tagStack->tagFlag & STARTTAG) ){
		parentEnd = xp->flags&XML_SURPRESS_LF? ">": ">\n";
		xp->tagStack->tagFlag &= ~STARTTAG;   /* only need to close one time */
	} else
		parentEnd = " ";
	if (parentEnd)
		return xmlMemPrintf(xp, "%s", parentEnd);
	return xp->index;
}

/*
 * xmlStartTagGrp pushes the tag-name onto the tag stack and
 * generates the string.
 *
 *   <tag-name attribute1="value" ...
 * in the buffer.
 * The argument list contains the
 * namespace reference if supplied and optional attribute arguments.
 *   xmlStartTag(xp, "Envelope attr=\"%s\"", valuearg);
 * The < are added by the xmlOpenTag function.
 * LF and indents are generated as optioned.
 */

int xmlStartTagGrp(XMLWriter *xp, const char *tagFmt, ... )
{
	va_list ap;
	char	tagBuf[1024];

	va_start(ap, tagFmt);
	vsnprintf(tagBuf, sizeof(tagBuf), tagFmt, ap);
	va_end(ap);

	putParentEnding(xp);
	pushTag(xp, tagBuf, STARTTAG );
	return xmlMemPrintf(xp,"<%s ",tagBuf);
}
/*
* xmlEndTag is used to close a xmlStartTag by generating the string
*    />
* The indent is adjusted if enabled.
* Returns: size of buffer contents.
*
*/
int     xmlEndTagGrp(XMLWriter *xp)
{
	size_t n = xp->index;
	char *tp;

	XMLTagStackItem *s = xp->tagStack;
	if (s) {
		xp->tagStack = s->next;
		if ( (tp = s->tag)!=NULL ){
			/* if pushTag allocation failed the tag is null */
			if ( s->tagFlag & CHILDTAG )
				n = xmlMemPrintf(xp,"</%s>%s",tp, xp->flags & XML_SURPRESS_LF? "": "\n");
			else
				n = xmlMemPrintf(xp,"/>%s",xp->flags & XML_SURPRESS_LF? "": "\n");
			GS_FREE(tp);
		}
		GS_FREE(s);
		xp->tagDepth--;
	}
	return n;
}
/*
 * xmlOpenTagGrp pushes the tag-name onto the tag stack and
 * generates the string.
 *
 *   <ns:tag-name>
 * in the buffer.
 * The argument list contains the
 * namespace reference.
 *   xmlOpenTag(xp, "%s:Envelope", nsSOAP);
 * The <> are added by the xmlOpenTag function.
 * LF and indents are generated as optioned.
 */

int xmlOpenTagGrp(XMLWriter *xp, const char *tagFmt, ... )
{
	va_list ap;
	char	tagBuf[1024];

	va_start(ap, tagFmt);
	vsnprintf(tagBuf, sizeof(tagBuf), tagFmt, ap);
	va_end(ap);

	pushTag(xp, tagBuf, OPENTAG);
	return xmlMemPrintf(xp,"<%s>%s", tagBuf, xp->flags&XML_SURPRESS_LF? "": "\n");
}
/*
* xmlCloseTag pops the tag-name from the tag stack and
* generates the string
*    </tag-name>
* Returns: size of buffer contents.
*
*/
int     xmlCloseTagGrp(XMLWriter *xp)
{
	size_t n = xp->index;
	char *tp;

	XMLTagStackItem *s = xp->tagStack;
	if (s) {
		xp->tagStack = s->next;
		if ( (tp = s->tag) ){
			n = xmlMemPrintf(xp,"</%s>%s",tp, xp->flags&XML_SURPRESS_LF? "": "\n");
			GS_FREE(tp);
		}
		GS_FREE(s);
		xp->tagDepth--;
	}
	return n;
}

int xmlPrintf(XMLWriter *xp, const char *s)
{
	size_t n=0;
	if (s)
		for (; *s; s++) {
			switch (*s) {
			case '&':
				n= xmlMemPrintf(xp, "&amp;");
				break;
			case '<':
				n= xmlMemPrintf(xp, "&lt;");
				break;
			case '>':
				n= xmlMemPrintf(xp, "&gt;");
				break;
			case '"':
				n= xmlMemPrintf(xp, "&quot;");
				break;
			case 9:
			case 10:
			case 13:
				n= xmlMemPrintf(xp, "&#%d;", *s);
				break;
			default:
				if (isprint(*s))
					n= xmlMemPrintf(xp, "%c", *s);
				else
					n= xmlMemPrintf(xp, " ");
				break;
			}
		}
	return n;
}

/*
* xmlPrintTaggedData
 * Write open tag, data, and close tag
 *     <tag>DATA DATA DATA</tag>
*/

int xmlPrintTaggedData( XMLWriter *xp, const char *tag, char *fmt, ...)
{
	int	flagSave;
	size_t n;
	va_list	ap;
	char	tagBuf[1024];

	va_start(ap, fmt);
	vsnprintf(tagBuf, sizeof(tagBuf), fmt, ap);
	va_end(ap);

	flagSave = xp->flags;
	xp->flags |= XML_SURPRESS_LF;
	xmlOpenTagGrp(xp, tag);
	xp->flags = flagSave;
	xmlMemPrintf(xp,"%s",tagBuf);
	n = xmlCloseTagGrp(xp);
	return n;
}

int xmlPrintTaggedAttrib( XMLWriter *xp, const char *tag, char *fmt, ...)
{
	size_t n;
	va_list	ap;
	char	attrBuf[512];

	if (xp->tagStack)
		xp->tagStack->tagFlag |= CHILDTAG;
    if ( (xp->flags&XML_NOINDENT)!= XML_NOINDENT )
        xmlMemIndent(xp);
	va_start(ap, fmt);
	vsnprintf(attrBuf, sizeof(attrBuf), fmt, ap);
	va_end(ap);
	putParentEnding(xp);
	n = xmlMemPrintf(xp,"<%s %s/>%s",
					  tag, attrBuf, xp->flags & XML_SURPRESS_LF? "": "\n");
	return n;

}
