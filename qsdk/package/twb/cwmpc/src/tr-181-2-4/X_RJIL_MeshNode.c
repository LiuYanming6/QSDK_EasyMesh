/*******************************************************************************
 *        Copyright (c) 2018 TWin Advanced Technology Corp., Ltd.
 *        All rights reserved.
 *
 *       THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
 *       PROPRIETARY AND TRADESECRET INFORMATION OF T&W INCORPORATED.
 *       ACCESS TO THIS WORK IS RESTRICTED TO (I) T&W EMPLOYEES WHO HAVE A
 *       NEED TO KNOW TO PERFORM TASKS WITHIN THE SCOPE OF THEIR ASSIGNMENTS
 *       AND (II) ENTITIES OTHER THAN T&W WHO HAVE ENTERED INTO APPROPRIATE
 *       LICENSE AGREEMENTS.  NO PART OF THIS WORK MAY BE USED, PRACTICED,
 *       PERFORMED, COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED,
 *       ABBRIDGED, CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
 *       TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF T&W.
 *       ANY USE OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD
 *       SUBJECT THE PERPERTRATOR TO CRIMINAL AND CIVIL LIABILITY.
 ******************************************************************************/
 /**
 * @file WiFi.c
 * @brief
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 */

/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2011, 2012 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description: Auto-generated getter/setter stubs file.
 *----------------------------------------------------------------------*
 * $Revision: 1.2 $
 *
 * $Id: stubgenPre,v 1.2 2012/05/10 17:38:08 dmounday Exp $
 *----------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include "paramTree.h"
#include "rpc.h"
#include "gslib/src/utils.h"
#include "gslib/src/utils_hyd.h"
#include "soapRpc/rpcUtils.h"
#include "soapRpc/rpcMethods.h"
#include "soapRpc/cwmpSession.h"

#include "X_RJIL_MeshNode.h"

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/

/**@param X_RJIL_MeshNode_ParentAPMAC                    **/
CPE_STATUS getX_RJIL_MeshNode_ParentAPMAC(Instance *ip, char **value)
{
    XRJILMeshNode *p = (XRJILMeshNode *)ip->cpeData;
    if ( p ){
        int ret;
        int dev = 0;
        hyd_wifison_dev son_dev[12] = {0};
        if((ret = ubox_get_qca_wifison_dev_num(&dev, 1)) != 0)
        {
            DBG_MSG("get_qca_wifison_dev_num error");
        }
        else
        {
            //hyd_wifison_dev *son_dev = malloc(dev+1 * sizeof(hyd_wifison_dev)); //TODO init structure
            ubox_get_qca_wifison_dev_topology(dev+1, son_dev, 0);
            if (NULL != son_dev[0].pmac )
            {
                if (0 == strncmp("00:00:00:00:00:00",  son_dev[0].pmac , strlen("00:00:00:00:00:00")))
                {
                    COPYSTR(p->parentAPMAC , "0" );
                }
                else
                {
                    COPYSTR(p->parentAPMAC , son_dev[0].pmac );
                }
            }
            //strcpy(addr, son_dev[0].pmac);
            //free(son_dev);
        }

        if ( p->parentAPMAC )
            *value = GS_STRDUP(p->parentAPMAC);

    }
    return CPE_OK;
}
/**@endparam                                               **/

/**@param X_RJIL_MeshNode_CAPMODE                   **/
CPE_STATUS getX_RJIL_MeshNode_CAPMODE(Instance *ip, char **value)
{
    XRJILMeshNode *p = (XRJILMeshNode *)ip->cpeData;
    if ( p )
    {
        char cmd[128]={0};
        char cmd_result[128]={0};
        const char default_role[] = "CAP";

        memset(cmd , 0x0 , sizeof(cmd));
        memset(cmd_result , 0x0 , sizeof(cmd_result));

        sprintf(cmd, "uci get repacd.repacd.DeviceRole 2>&1");
        cmd_popen(cmd , cmd_result );
        
        if (NULL != cmd_result && strncmp(cmd_result, default_role, strlen(default_role)) == 0) 
            p->cAPMODE = 1 ;
        else        
            p->cAPMODE = 0 ;

        *value = GS_STRDUP(p->cAPMODE? "1": "0");
    }
    return CPE_OK;
}

/**@endparam   */


CPE_STATUS  initX_RJIL_MeshNode(CWMPObject *o, Instance *ip)
{
    /* add instance data */
    XRJILMeshNode *p = (XRJILMeshNode *)GS_MALLOC( sizeof(struct XRJILMeshNode));
    memset(p, 0, sizeof(struct XRJILMeshNode));
    ip->cpeData = (void *)p;
    return CPE_OK;
}


