
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : xmlParser.c
 * Description:	Parse xml syntax.
 *             .
 *----------------------------------------------------------------------*
 * $Revision: 1.4 $
 * $Id:
 *----------------------------------------------------------------------*/


#include <stdlib.h>
#include <string.h>
#include "xmlParser.h"
#include "utils.h"
//#define DEBUG
#ifdef DMALLOC

#include "dmalloc.h"
#endif
#ifdef DEBUG
#include <stdio.h>
#define DBGPRINT(X) fprintf X
#else
#define DBGPRINT(X)
#endif

XML_STATUS xmlSetContext( XMLParser *tsk, XmlNodeDesc *top)
{
	if (tsk) {
		tsk->node = tsk->nodeStack[0] = top;
		return XML_STS_OK;
	}
	return XML_STS_ERR;
}
/* xmlSetNameSpace
* Set the xml namespace table
*/
XML_STATUS xmlSetNameSpaces( XMLParser *p, NameSpace *nsp)
{
	if (p) {
		p->nameSpaces = nsp;
		return XML_STS_OK;
	}
	return XML_STS_ERR;

}

XML_STATUS xmlSetErrorCB( XMLParser *p, void (*errorCB)(char *,...))
{
	if (p) {
		p->parse_error = errorCB;
		return XML_STS_OK;
	}
	return XML_STS_ERR;
}
XML_STATUS xmlSetEmbeddedDataSink( XMLParser *p,
				void (*embeddedDataSink)(const char *data, int len))
{
	if (p) {
		p->embeddedDataSink = embeddedDataSink;
		return XML_STS_OK;
	}
	return XML_STS_ERR;
}
XML_STATUS xmlSetXMLScanSink( XMLParser *p,
				       void (*scanSink)(TOKEN_TYPE, const char *data))
{
	if (p) {
		p->scanSink = scanSink;
		return XML_STS_OK;
	}
	return XML_STS_ERR;
}

XMLParser *xmlOpen(
	XML_STATUS (*beginTag) (pXMLParser, const char *, size_t),
	XML_STATUS (*endTag)   (pXMLParser, const char *, size_t),
	XML_STATUS (*beginAttr)(pXMLParser, const char *, size_t),
	XML_STATUS (*attrValue)(pXMLParser, const char *, size_t, int),
	XML_STATUS (*dataChunk)(pXMLParser, const char *, size_t, int),
	void *userData )
{
	XMLParser *p;
    if ( (p= (XMLParser*)GS_MALLOC(sizeof (struct XMLParser))) != NULL) {
        memset(p,0, sizeof(XMLParser));
		p->scanCallBacks.beginTag = beginTag;
		p->scanCallBacks.endTag = endTag;
		p->scanCallBacks.beginAttr = beginAttr;
		p->scanCallBacks.attrValue = attrValue;
		p->scanCallBacks.dataChunk = dataChunk;
		p->userPtr = userData;
        p->nameCacheSize = 0;
        p->treeLevel = 0;
        p->skipWS = 1;
        p->xmlState = eXMLSTART;
        return p;
    }
    return NULL;
}

XML_STATUS xmlClose(XMLParser *p)
{
    GS_FREE(p);
	return XML_STS_OK;
}

#define WHITESPACE " \t\r\n"

/* Append data to the nameCache. This is used for tag and attribute names.
If the nocopy parameters are specified, then the caller regards the name
as complete, and if there's nothing already in the cache, then there's no
need for a copy. */
static void xmlAddToName(XMLParser *p, char *data, int len,
                               char **data_nocopy, unsigned *len_nocopy)
{
    /* if the nocopy parameters are supplied, and there's nothing in the cache,
    then don't copy. just pass them back. */
    if (data_nocopy && !p->nameCacheSize) {
        *data_nocopy = data;
        *len_nocopy = len;
    } else {
        if (len > XML_MAX_NAME_SIZE - p->nameCacheSize)
            len = XML_MAX_NAME_SIZE - p->nameCacheSize;
        if (len) {
            strncpy(&p->nameCache[p->nameCacheSize], data, len);
            p->nameCacheSize += len;
        }
        if (data_nocopy) {
            *data_nocopy = p->nameCache;
            *len_nocopy = p->nameCacheSize;
        }
    }
}
/*
* Returns: *endp to end of parsed data.
*   XML_STS_OK : OK - parse complete
 *  or XML_STS_ERR parsing error.
 *  XML_STS_MEM memory error.
 */

XML_STATUS xmlParse(XMLParser *xp, char *data, int len, char **endp)
{
	XML_STATUS xs = XML_STS_OK;
    int treeEnd=0;
    const char *enddata = data + len;

    DBGPRINT((stderr, " ---- xmlParser(): len %d\n",len));
    if (xp->xmlState == eXMLSTART) {
        char *p;
        DBGPRINT((stderr, " ---- skip <?xml header "));
		p = data + strspn(data,WHITESPACE);
        if ( !strncmp(p,"<?",2) ) {
            p = strstr( p, "?>");
            if (p)
                data = p+2;
        }
        xp->xmlState = eTAG_BEGIN;
    }

    while ((data < enddata) && (!treeEnd) && (xs==XML_STS_OK)) {
        char *s; /* temp value for capturing search results */

        /* skip whitespace */
        if (xp->skipWS) {
            s = data + strspn(data,WHITESPACE);
            if (s != data) {
                data = s;
                continue;
            }
        }

        switch (xp->xmlState) {
        case eTAG_BEGIN:
            s = strchr(data, '<');
            if (!s) {
                /* it's all data */
                if ( xp->scanCallBacks.dataChunk )
                    (*xp->scanCallBacks.dataChunk)(xp, data, enddata-data, 1/*maybe more data*/);
                *endp = data;
                return 0;
            } else if ( (data != s) && xp->scanCallBacks.dataChunk) {
                /* we have some data, then a tag */
                xs = (*xp->scanCallBacks.dataChunk)(xp, data, s-data, 0/*end of data*/);
            }
            /* skip over the tag begin and process the tag name */
            data = s+1;
            xp->xmlState = eTAG_NAME;
            xp->nameCacheSize = 0;
            ++(xp->treeLevel);
            break;

        case eXMLEND:
            /* we don't care about anything but the end of a tag */
            s = strchr(data, '>');
            if (!s) {
                *endp = data;
                return 0;  /* return for more data */
            }

            /* we found it, so start looking for the next tag */
            data = s+1;
            xp->xmlState = eTAG_BEGIN;
            if ( --(xp->treeLevel) <= 0 )
                treeEnd = 1;
            break;

		case eTAG_COMM:
            s = strchr(data, '>');
            if (!s) {
				/* out of data and no comment end */
				*endp = data;
				return 0; /* return for more data */
			}
			xp->xmlState = eTAG_BEGIN;
			data = s+1;
            if ( --(xp->treeLevel) <= 0 ) /* treeLevel is incremented on begin of comment*/
                treeEnd = 1;
			break;

        case eTAG_END:
            s = strpbrk(data, WHITESPACE ">");
            if (!s) {
                /* it's all name, and we're not done */
                xmlAddToName(xp, data, enddata-data, NULL, NULL);
                xp->skipWS = 0;
                *endp = data;
                return 0;             /* return for more data */
            } else {
                char *name;
                unsigned len;
                xmlAddToName(xp, data, s-data, &name, &len);
                xs = (*xp->scanCallBacks.endTag)(xp, name, len);
                xp->xmlState = eXMLEND;
                data = s;
                if (--(xp->treeLevel) <=0)
                    treeEnd = 1;
            }
            break;

        case eTAG_NAME:
		case eATTR_NAME:
			if (*data == '!') {
				/* assuming a <! comment start */
				xp->xmlState = eTAG_COMM;
				data ++;
				break;
			} else if (*data == '/') {
                /* this tag is done */
                if (xp->xmlState == eTAG_NAME && !xp->nameCacheSize) {
                    /* we can still parse the end tag name so that the uppperlevel app
                    can validate if it cares */
                    xp->xmlState = eTAG_END;
                    data++;
                    break;
                } else if (xp->xmlState == eATTR_NAME) {
                    /* we had an attribute, so this tag is just done */
                    xs = (*xp->scanCallBacks.endTag)(xp, xp->nameCache, xp->nameCacheSize);
                    xp->xmlState = eXMLEND;
                    data++;
                    break;
                }
            } else if (*data == '>') {
                xp->xmlState = eTAG_BEGIN;
                data++;
                break;
            }

            s = (char *)strpbrk(data, WHITESPACE "=/>");
            if (!s) {
                /* it's all name, and we're not done */
                xmlAddToName(xp, data, enddata-data, NULL, NULL);
                xp->skipWS = 0;
                *endp = data;
                return 0;	 /* return for more name */
            } else {
                /* we have the entire name */
                char *name;
                unsigned len;
                xmlAddToName(xp, data, s-data, &name, &len);

                if (xp->xmlState == eTAG_NAME) {
                    xs = (*xp->scanCallBacks.beginTag)(xp, name, len);
                    xp->xmlState = eATTR_NAME;
                } else {
                    (*xp->scanCallBacks.beginAttr)(xp, name, len);
                    xp->xmlState = eATTR_EQUALS;
                }
                xp->nameCacheSize = 0;
                data = s;
            }
            break;

        case eATTR_VALUE:
            s = (char *)strchr(data, '"');
            if (!s) {
                /* it's all attribute_value, and we're not done */
                xs = (*xp->scanCallBacks.attrValue)(xp, data, enddata-data, 1);
                xp->skipWS = 0;
                *endp = data;
                return 0;	   /* return for more attribute value */
            } else {
                /* we have some value data, then a tag */
                xs = (*xp->scanCallBacks.attrValue)(xp, data, s-data, 0);
            }
            /* skip over the quote and look for more attributes */
            data = s+1;
            xp->xmlState = eATTR_NAME;
            xp->nameCacheSize = 0;
            break;


        case eATTR_EQUALS:
            if (*data == '>') {
                xp->xmlState = eTAG_BEGIN;
                data++;
            } else if (*data == '=') {
                xp->xmlState = eATTR_QUOTE;
                data++;
            } else
                xp->xmlState = eATTR_NAME;
            break;

        case eATTR_QUOTE:
            if (*data == '"')
                data++;
            xp->xmlState = eATTR_VALUE;
            break;
        default:
            break;
        }
        xp->skipWS = 1;
    }  /* while () ... */
    *endp = data;
    if ( !treeEnd && xs == XML_STS_OK )
    	xs = XML_STS_ERR;		/* error is not end of xml tree */
    return xs;
}


