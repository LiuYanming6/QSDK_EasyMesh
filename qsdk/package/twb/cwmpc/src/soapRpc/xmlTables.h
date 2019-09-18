#ifndef XMLTABLES_H
#define XMLTABLES_H
/*----------------------------------------------------------------------*
 * Gatespace 
 * Copyright 2006 Gatespace. All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  : XMLTables.c
 *
 * Description: SOAP xmlTables and data structures 
 * $Revision: 1.3 $
 * $Id: xmlTables.h,v 1.3 2011/10/26 12:15:14 dmounday Exp $
 *----------------------------------------------------------------------*/
#include "../gslib/src/xmlParserSM.h"
extern NameSpace    nameSpaces[];
extern XmlNodeDesc  cwmpEnvelopeDesc[];

/* MACROS for referencing the above namespace */
/* strings from xml node description tables   */
/* must match initializations in xmlTables */
/* MACROS for sending namespace prefix */
#define nsSOAP       nameSpaces[0].sndPrefix
#define nsSOAP_ENC   nameSpaces[1].sndPrefix
#define nsXSD        nameSpaces[2].sndPrefix
#define nsXSI        nameSpaces[3].sndPrefix
#define nsCWMP       nameSpaces[4].sndPrefix

#endif 
