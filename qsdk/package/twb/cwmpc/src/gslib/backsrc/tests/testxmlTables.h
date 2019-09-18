#ifndef XMLTABLES_H
#define XMLTABLES_H
/*----------------------------------------------------------------------*
 * Gatespace 
 * Copyright 2006 Gatespace. All Rights Reserved.
 *----------------------------------------------------------------------*
 * File Name  : XMLTables.c
 *
 * Description: SOAP xmlTables and data structures 
 * $Revision: 1.1 $
 * $Id: testxmlTables.h,v 1.1 2011/11/16 13:56:37 dmounday Exp $
 *----------------------------------------------------------------------*/
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
