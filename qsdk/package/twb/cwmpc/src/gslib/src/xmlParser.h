
/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2006-2011 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : xmlParser.h
 * Description:	Parse xml syntax.
 *             .
 *----------------------------------------------------------------------*
 * $Revision: 1.5 $
 * $Id:
 *----------------------------------------------------------------------*/
#ifndef XML_PARSER_H_
#define XML_PARSER_H_
/*---------------------------------------------------------------------------
Notation:

	<tag attribute_name="attribute_value">data</tag>

Callbacks:

	XML_STATUS (*beginTag) (XMLTask *task, const char *tagName, size_t len);
	XML_STATUS (*endTag)   (XMLTask *task, const char *tagName, size_t len);
	XML_STATUS (*beginAttr)(XMLTask *task, const char *attrName, size_t len, int moreData);
	XML_STATUS (*attrValue)(XMLTask *task, const char *value, size_t len);
	XML_STATUS (*dataChunk)(XMLTask *task, const cahr *data, size_t len, int moreData);

	beginTag - called when a new tag is encountered. Passes the complete tag name.
	beginAttr - called when a new attribute is encountered. Passes the complete attribute name.
	attrValue - called with attribute value data. It could require multiple callbacks
		to complete the attribute value. moreData is set to 0 if no more callbacks are expected.
	dataChunk - called with element data. It could require multiple callbacks to complete the
		element data. moreData is set to 0 if no more callbacks for data are expected.
	endTag - called when a tag is closed. Even self closing tags (e.g. <tag/> receive
		both a tag_begin and a tag_end.

Conditions:
1) Tag and attribute names cannot be greater than NXML_MAX_NAME_SIZE. Larger
   names will be truncated without any warning.
2) You will receive one beginTag callback at the beginning of each tag, but before any attribute
   callbacks for that tag.
3) You may receive zero or more beginAttr callbacks after the beginTag but before
   the first data callback. After you receive a data callback, you cannot receive
   an beginAttr callback until a new tag is processed..
4) You can receive many dataChunk or attrValue callbacks for each tag or attribute.
   This is because the size of the data or attribute_value is not bounded. noMore is set to 0
   on the last dataChunk or attrValue callback.
5) If you have a tag that has attributes and that tag ends with a "/" (e.g. <tag attr="value" />),
   the endTag callback will not send you the tagName.
   It will be a non-NULL pointer but len == 0.

*
*--------------------------------------------------------------------------------*/


/************************************************************************/
/* parserSM defines ***************/

typedef enum {
    XML_STS_OK = 0,
    XML_STS_ERR,
    XML_STS_MEM
}XML_STATUS;

typedef enum {
    TOKEN_INVALID,
    TAGBEGIN,
    TAGEND,
    TAGDATA,
    ATTRIBUTE,
    ATTRIBUTEVALUE
}TOKEN_TYPE;

#define XML_MAX_NAME_SIZE 256

/* Node flags for xmlParseSM */
#define XML_SCAN_F  1

#define XMLFUNC(XX) static XML_STATUS XX (void *, const char *, TOKEN_TYPE, const char *)

typedef XML_STATUS (*XML_SET_FUNC)(void *userData, const char *name, TOKEN_TYPE ttype, const char *value);
/* userData is a pointer passed to the xmlOpen function by the routine calling */
/* the parser. It may point back to data used by the xml callbacks. */

#define MAX_DEPTH   20


typedef struct NameSpace {
    char    *rcvPrefix;     /* pointers to prefix names: set by each envelope */
                            /* xmlns: attribute list */
    char    *sndPrefix;     /* prefixs to use on sent msgs */
    char    *nsURL;         /* namespace URL for this application  */
                            /* */
} NameSpace;

#define iNULL       NULL
#define iDEFAULT    ((void*)(-1))

typedef struct XmlNodeDesc {
    NameSpace    *nameSpace;
    const char   *tagName;
    XML_SET_FUNC setXmlFunc;
    void        *leafNode;
} XmlNodeDesc;

 typedef enum {		/* scanner is looking for:                 */
	 eXMLSTART,		/* <? or < 									*/
	 eQUESTION,		/* ? following <				*/
	 eTAG_BEGIN,	/* <							*/
	 eTAG_NAME,		/* scan for whole name			*/
	 eTAG_END,		/* /							*/
	 eATTR_NAME,	/* attrib-name					*/
	 eATTR_EQUALS,	/* =							*/
	 eATTR_QUOTE,	/* "							*/
	 eATTR_VALUE,	/* value of attribute			*/
	 eTAG_COMM,		/* Comment <! found , looking for >*/
	 eXMLEND		/* >  (end-of-tag)				*/
 } eXMLState;

typedef struct XMLParser *pXMLParser;

typedef struct XMLScanCallBacks {
	XML_STATUS (*beginTag) (pXMLParser, const char *tagName, size_t len);
	XML_STATUS (*endTag)   (pXMLParser, const char *tagName, size_t len);
	XML_STATUS (*beginAttr)(pXMLParser, const char *attrName, size_t len);
	XML_STATUS (*attrValue)(pXMLParser, const char *value, size_t len, int moreData);
	XML_STATUS (*dataChunk)(pXMLParser, const char *data, size_t len, int moreData);
} XMLScanCallBacks;



typedef struct XMLParser {
	XMLScanCallBacks	scanCallBacks;
	void				*userPtr;		/* pointer back to user context data*/
	eXMLState			xmlState;		/* state of scanner */
	char 				nameCache[XML_MAX_NAME_SIZE];
	int 				nameCacheSize;
	int 				skipWS;			/* skip white space */
    int 				treeLevel;
    int 				nodeFlags;
	NameSpace   		*nameSpaces;    /* pointer to nameSpace tables */
    void (*scanSink)(TOKEN_TYPE ttype, const char *data);
    void (*embeddedDataSink)(const char *data, int len);
    void (*parse_error)(char *errfmt, ...);
    XmlNodeDesc     *node;      /* points to the current node */
    int             level;      /* xml level - starting at 0 */
    char            attrName[XML_MAX_NAME_SIZE];
    XmlNodeDesc     *nodeStack[MAX_DEPTH];  /* points at next higher node */
    XmlNodeDesc     *itemStack[MAX_DEPTH];  /* points at item used at node*/
    char	    	*valueptr;		/* Accumlated attr value */
    int		    	valuelth;		/* lth of accumlated attr value */
    char        	*dataptr;       /* Accumlated data */
    int         	datalth;        /* lth of accumlated data*/
}XMLParser;

/*
* Open an XML parsing task. The scanCallBacks must be set by the calling
* routine.
*/
XMLParser *xmlOpen(
	XML_STATUS (*beginTag) (pXMLParser, const char *, size_t),
	XML_STATUS (*endTag)   (pXMLParser, const char *, size_t),
	XML_STATUS (*beginAttr)(pXMLParser, const char *, size_t),
	XML_STATUS (*attrValue)(pXMLParser, const char *, size_t, int),
	XML_STATUS (*dataChunk)(pXMLParser, const char *, size_t, int),
	void *userData
    );

/* parse xml buffer. Buffer may be any length and the call may be made
* multiple times if the buffer is being refilled for the network or a file
*/
XML_STATUS xmlParse( XMLParser *tsk, char *buf, int len, char **xmlEnd );

/* close the xml parser task. Any allocated memory is released.
*/
XML_STATUS xmlClose( XMLParser *tsk);

/* xmlSetDefinition
* Set the definition tables of the xmlParserSM. Typically set to address
* of root of xml tables.
*/
XML_STATUS xmlSetContext(XMLParser *, XmlNodeDesc *);
/* xmlSetNameSpace
* Set the xml namespace table
*/
XML_STATUS xmlSetNameSpaces(XMLParser *, NameSpace *);

/* xmlSetErrorCB
* Set the xml error call back handler
*/
XML_STATUS xmlSetErrorCB( XMLParser *, void (*errorCB)(char *,...));

XML_STATUS xmlSetEmbeddedDataSink( XMLParser *p,
						void (*embeddedDataSink)(const char *data, int len));

XML_STATUS xmlSetXMLScanSink( XMLParser *p,
				       void (*scanSink)(TOKEN_TYPE, const char *data));


#endif /* XML_PARSER_H_ */
