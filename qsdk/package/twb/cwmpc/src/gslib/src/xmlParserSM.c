
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2004 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : xmlParserSM.c
 *
 * Description: xmlParser state machine
 *
 *
 * $Revision: 1.7 $
 * $Id: xmlParserSM.c,v 1.7 2011/10/26 10:42:52 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

#include "utils.h"
#include "xmlParserSM.h"

void cpeLog(int level, const char *fmt, ...);
void cpeVlog(int level, char *fmt, va_list ap);

#ifdef DMALLOC
#include "dmalloc.h"
#endif
//#define DEBUG
#ifdef DEBUG
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

#define WHITESPACE  "\t\n\r "
/* trim white space from beginning and end of buffer b */
/* returns pointer to first non-ws char and inserts \0 */
/* after last non-ws char in buffer. Input buffer is */
/* assumed to be null terminated */
static char *trimws(char *b, int lth)
{
    char *s, *e;
    int start;
    s = b+ (start = strspn(b,WHITESPACE));
    for ( e=b+lth-1; e>s; --e)
        if ( *e==' ' || *e=='\n' || *e=='\t' || *e=='\r')
            *e = '\0';
        else
            break;
    return s;
}

static int xmlnsPrefix(char *name)
{   char *s;
    if ( (s=strchr(name, ':')) )
        return !strncmp(name,"xmlns:",s-name+1);
    return 0;
}

/*
* searches the nameSpace table to see if we understand
* the urlvalue. If found then set the receive prefix
* and return 1;
* otherwise return 0
 * Assumes that the p->attrName is of the form
 * xmlns:prefix.
* In future we may want to add the unknow namespaces.
*/
static int cwmpXMLaddNameSpace(pXMLParser p, char *urlvalue)
{
    char *s;
    NameSpace *nsp;

    s = strchr(p->attrName, ':');
    if (s) {
        /* look for urlstring in namespaces table*/
        nsp = p->nameSpaces;
        while( nsp->nsURL ){
            if (!strcmp(nsp->nsURL, urlvalue)) {
                /* found namespace url */
                if (nsp->rcvPrefix)
                    GS_FREE(nsp->rcvPrefix);
                nsp->rcvPrefix = GS_STRDUP(s+1);
                return 1;
            }
            ++nsp;
        }
    }
    return 0;
}

/* name compare with prefix:xxx form checking */
/* This needs more thought--- The namespaces should */
/* be unchanging but this may not work with an */
/* ACS that is bent on messing up the CPE clients */
static int  nameCmp(char *name, XmlNodeDesc *node)
{
    char    *s;
    s = strchr(name,':');
    if (s ) {/* prefix is present*/
        if ( node->nameSpace
            && node->nameSpace!=iDEFAULT
            && node->nameSpace->rcvPrefix
            && strncmp(name, node->nameSpace->rcvPrefix, s-name+1)) {
            /*namespaces match */
                return strcmp(s+1,node->tagName);
        } else if (node->nameSpace==iDEFAULT){
            /* a universal prefix value in node */
            return strcmp(s+1,node->tagName);
        } else if (node->nameSpace==iNULL) {
            return strcmp(s+1,node->tagName);
        }
    } else {
        /* no prefix */
        /* if(node->nameSpace==iNULL) */
            return strcmp(name,node->tagName);
    }
    return -1; /* no match*/
}

static XML_STATUS xmlTagBegin( pXMLParser p, const char *tagName, size_t lth)
{
    XmlNodeDesc *item;
    char tag[XML_MAX_NAME_SIZE];
    XML_STATUS xs = XML_STS_OK;

    strncpy(tag, tagName,lth);
    tag[lth]='\0';
    DBGPRINT((stderr, "xmlTagBegin tag=%s lth=%d xmllevel=%d\n", tag,lth, p->level));
    if ( (p->nodeFlags & XML_SCAN_F) && p->scanSink)
        /* Scan mode is set write tag to data sink */
        p->scanSink(TAGBEGIN, tag );
    if ( (item = p->node)) {
        while (item != NULL && item->tagName) {
            if (nameCmp(tag, item)==0) {
                /* found node entry */
                if ( item->setXmlFunc )  /* null callbacks are ok */
                    if ((xs = item->setXmlFunc(p->userPtr, tag, TAGBEGIN, NULL)) != XML_STS_OK)
                        p->parse_error("Parse error at begin Tag = %s ", tag);
                p->level++;
                p->nodeStack[p->level] = p->node;
                p->itemStack[p->level] = item;
                p->node = item->leafNode;
                return xs;
            } else
                item++;
        }
    }
    /*if (p->node) */ /* only put out not found msg on high level tag */
       DBGPRINT((stderr, "xmlTagBegin tag=%s not found\n", tag));
    p->level++;
    p->nodeStack[p->level] = p->node;  /* this is so the parserSM will walk thru the tree */
    p->itemStack[p->level] = NULL;  /* and ignore unknown tags if the structure is correct*/
    p->node = NULL;
    return xs;
}
/*
* xmlTagEnd:
*     tagname lth may be 0 if the form is <tag attr=name />
*/
static XML_STATUS xmlTagEnd ( pXMLParser p, const char *tagName, size_t lth)
{
    XmlNodeDesc *item;
    char tag[XML_MAX_NAME_SIZE];
    XML_STATUS xs = XML_STS_OK;
    if (lth > 0)
        strncpy(tag, tagName,lth);
    tag[lth]='\0';
    /* clean up old accumlated value in case of error exit */
    if (p->valueptr )GS_FREE(p->valueptr);
    if (p->dataptr )GS_FREE(p->dataptr);
    p->valueptr = p->dataptr = NULL;
    p->valuelth = p->datalth = 0;

    if(p->level<=0){
        p->parse_error("Bad xml tree fromat tag=%s", tag);
        return XML_STS_ERR;
    }
    p->node = p->nodeStack[p->level];
    item = p->itemStack[p->level];
    p->level--;
    if ( item!=NULL) {
        if (lth>0){
            if (nameCmp(tag, item)==0) {
                /* found node item entry */
                if (item->setXmlFunc != NULL)
                    if ( (xs= item->setXmlFunc(p->userPtr, tag,TAGEND,NULL))!=XML_STS_OK)
                        /* error */
                        p->parse_error("XML parser callback function error at tag=%s ", item->tagName);
                DBGPRINT((stderr, "xmlTagEnd tag=%s found at level %d\n", tag, p->level));
                return xs;
            }
            DBGPRINT((stderr, "xmlTagEnd tag=%s internal nodepointer error level=%d\n", tag, p->level));
            return XML_STS_ERR;
        }
        else {
            DBGPRINT((stderr, "xmlTagEnd shortform tag=%s xmllevel=%d\n", item->tagName, p->level));
            if (item->setXmlFunc!=NULL)
                if ( (xs=item->setXmlFunc(p->userPtr, tag,TAGEND,NULL))!=XML_STS_OK)
                    p->parse_error("XML parser callback function error at tag=%s ", item->tagName);
            return xs;
        }
    }
    if (p->nodeFlags&XML_SCAN_F && p->scanSink) {
        /* Scan mode is set write tag to data sink */
        p->scanSink(TAGEND, tag);
    }
    /* this is an error */
    DBGPRINT((stderr, "xmlTagEnd tag=%s xml node not found at level %d\n", tag, p->level));
    return XML_STS_OK; /* allow most errors */
}

static XML_STATUS xmlAttr( pXMLParser p, const char *attrName, size_t lth)
{
    XmlNodeDesc *item;
    char attr[XML_MAX_NAME_SIZE];

    strncpy(attr, attrName,lth);
    attr[lth]='\0';

    if (p->nodeFlags & XML_SCAN_F && p->scanSink) {
        /* Scan mode is set write tag to data sink */
        p->scanSink(ATTRIBUTE, attr );
    }
    if (xmlnsPrefix(attr)) {
        /* save name and prefix for xmlValue callback*/
        strcpy(p->attrName, attr);
        return XML_STS_OK;
    }
    if ((item = p->node)) {
        if (p->level>0) {
            while (item->tagName) {
                if (nameCmp(attr, item)==0) {
                    /* found node entry */
                    strcpy(p->attrName, attr);
                    DBGPRINT((stderr, "xmlAttr attr=%s found at level %d\n", attr, p->level));
                    return XML_STS_OK;
                } else
                    item++;
            }
        }
    }
    if (p->node)   /* Suppress error on attributes of unknown tags */
        p->parse_error("Unknow attribute %s\n", attr);
    DBGPRINT((stderr, "xmlAttr attr=%s not found at level %d\n", attr, p->level));
    return XML_STS_OK;
}

static XML_STATUS xmlValue( pXMLParser p, const char *attrValue, size_t lth, int more)
{
    XmlNodeDesc *item;
    XML_STATUS xs = XML_STS_OK;
    char *value;
    if ((value = GS_REALLOC(p->valueptr, p->valuelth+lth+1))==NULL){
    	GS_FREE(p->valueptr);
    	p->parse_error("Parser value allocation buffer failure at %d", p->level);
    	p->valueptr = NULL;
        return XML_STS_MEM;
	}
    p->valueptr = value;
    memcpy(value+p->valuelth, attrValue, lth);
    p->valuelth += lth;
    if (more)
    	return XML_STS_OK;
    p->valueptr[p->valuelth]='\0';
    if (p->nodeFlags & XML_SCAN_F && p->scanSink) {
        /* Scan mode is set write tag to data sink */
        p->scanSink(ATTRIBUTEVALUE, value );
    }
    /* first check if attrname has an xmlns: prefix */
    if ( xmlnsPrefix(p->attrName)) {
        /* its a name space declaration */
        cwmpXMLaddNameSpace(p, value);
        GS_FREE(p->valueptr);
        p->valuelth =0; p->valueptr=NULL;
        return XML_STS_OK;
    }
    /* now lookup saved attrName in xml tables */
    if ( (item = p->node)) {
        if (p->level>0) {
            while (item->tagName) {
                if (nameCmp(p->attrName, item)==0) {
                    /* found node entry */
                    if (item->setXmlFunc!=NULL)
                        if ( (xs =item->setXmlFunc(p->userPtr, p->attrName,ATTRIBUTEVALUE, value))!=XML_STS_OK)
                            p->parse_error("Attribute value error for %s=\"%s\"\n", item->tagName, value);
                    DBGPRINT((stderr, "xmlValue attr=%s value=%s found at level %d\n", item->tagName,
                                        value, p->level));
        		    GS_FREE(p->valueptr);
        		    p->valueptr = NULL;
        		    p->valuelth =0;
                    return xs;
                } else
                    item++;
            }
        }
    }
    if(p->node)
        p->parse_error("Unknown attribute for value=%s at level %d\n", value, p->level);
    DBGPRINT((stderr, "xmlValue attr=%s not found at level %d\n", p->attrName, p->level));
    GS_FREE(p->valueptr);
    p->valueptr= NULL; p->valuelth=0;
    return XML_STS_OK;   /* allow extra attributes */
}

/* Callback from scanner for data between tags or attributes */
/* more flag indicates that a token terminated data, 0 more data value */
static XML_STATUS xmlData( pXMLParser p, const char *data, size_t lth, int more)
{
    XmlNodeDesc *item;
    XML_STATUS xs = XML_STS_OK;
    char *dp;
    if ((dp = GS_REALLOC(p->dataptr, p->datalth+lth+1))==NULL){
    	GS_FREE(p->dataptr);
    	p->parse_error("Parser data allocation buffer failure at %d", p->level);
    	p->dataptr = NULL;
    	return XML_STS_MEM;
	}
    p->dataptr = dp;
    memcpy(dp+p->datalth, data, lth);
    p->datalth += lth;
    if (more)
        return XML_STS_OK;
    p->dataptr[p->datalth]='\0';
    /* if using scansink -- needs to calback to it here */
    /* otherwise just call xmlSetFunc with TAGDATA */
    if ( (item = p->itemStack[p->level])) {
        if (item->setXmlFunc!=NULL){
            dp = trimws(dp,p->datalth);
            if ( (xs = item->setXmlFunc(p->userPtr, item->tagName,TAGDATA, dp))!=XML_STS_OK)
                p->parse_error("Tag data error for %s=\"%s\"", item->tagName, dp);
        }
        DBGPRINT((stderr, "xmlDATA tag=%s data=%s found at level %d\n", item->tagName,
                            dp, p->level));
        GS_FREE(p->dataptr);
        p->dataptr = NULL;
        p->datalth =0;
        return xs;
    }
    if(p->node)
        p->parse_error("Unknown data=%s at level %d", dp, p->level);
    DBGPRINT((stderr, "xmlDATA at level %d\n",p->level));
    GS_FREE(p->dataptr);
    p->dataptr= NULL; p->datalth=0;
    return XML_STS_OK;
}

/*----------------------------------------------------------------------*/
static void parseError(char *errfmt, ...) {
    va_list ap;

    va_start(ap, errfmt);
    cpeVlog(LOG_ERR, errfmt, ap);
    va_end(ap);
    return;
}

/*******************************************************************/
/*----------------------------------------------------------------------*
 * parse from file or in-memory data
 *   xmlParseGeneric("/xyz", NULL, 0, ...)   parses content in file /xyz
 *   xmlParseGeneric(NULL, ptr, size, ...)      parses content pointed to by ptr
 *   xmlParseGeneric("/xyz", ptr, size, ...)    error, illegal usage return PARSE_ERROR
 */
XML_STATUS xmlParseGeneric(void *context, char *path, char *memory, int size, ParseHow *parseHow )
{
    char *buf=NULL;
    int done;
    int file=0;
    XMLParser 	*parser;
    char 		*xmlEnd;
    XML_STATUS	xs = XML_STS_OK;
    //XML_STATUS error = PARSE_ERROR;

    if (path != NULL && memory != NULL) {
        cpeLog(LOG_ERR, "parser: %s", "internal error: parse_generic() can not parse both from file and memory\n");
        return XML_STS_ERR;
    }
	parser = xmlOpen( xmlTagBegin, xmlTagEnd, xmlAttr, xmlValue, xmlData, context);
    if ( parser ) {
		xmlSetContext(parser, parseHow->topLevel);
		xmlSetNameSpaces(parser, parseHow->nameSpace);
		xmlSetErrorCB(parser, parseError);

        if (path != NULL) {
            if ( (buf = (char *)GS_MALLOC(BUFSIZE)) == NULL){
            	cpeLog(LOG_ERR, "Parser: failed to allocate file buffer");
            	xmlClose(parser);
            	return XML_STS_ERR;
            }
            if ( (file = open(path, O_RDONLY, 0 ))== -1){
                cpeLog(LOG_ERR, "Parser:Could not open file %s", path);
				xmlClose(parser);
				GS_FREE(buf);
                return XML_STS_ERR;
            }
			buf[BUFSIZE-1]='0'; /* xmlParser needs a null terminated buffer */
			do {
				if (path != NULL) {
					/* from file */
					size_t len = read(file, buf, BUFSIZE-1);
					done = len < BUFSIZE;
					if ( (xs = xmlParse(parser, buf, len, &xmlEnd))!=XML_STS_OK) {
						cpeLog(LOG_ERR, "Parser: failed to parse XML in file %s",
							path);
					}
				}
			} while ( xs == XML_STS_OK && !done );
			GS_FREE(buf);
			close(file);
        } else {
			/* from memory */
			if ( (xs = xmlParse(parser, memory, size, &xmlEnd))!=XML_STS_OK) {
				cpeLog(LOG_ERR, xs==XML_STS_MEM?
						"parser: failed to allocate memory":
						"Parser: failed parse of XML");
				/* need line number of error here */
			}
        }
		xmlClose(parser);
	}

    return xs;
}



