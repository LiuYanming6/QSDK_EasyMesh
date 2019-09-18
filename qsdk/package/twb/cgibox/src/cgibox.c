/*******************************************************************************
 *        Copyright (c) 2017 TWin Advanced Technology Corp., Ltd.
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
 * @file cgibox.c
 * @brief ...
 * @author T&W Technology co., Ltd.
 * @bug No known bugs
 *
 * main
 * -> enap_parser
 *   -> parse_action_from_uri()
 */
/*-------------------------------------------------------------------------*/
/*                        INCLUDE HEADER FILES                             */
/*-------------------------------------------------------------------------*/
#include <cgibox.h>
#include <string.h>
#include <fcgi_stdio.h>

#include <stdio.h>
#include <json-c/json.h>
#include <ctype.h>


#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <assert.h>


/*-------------------------------------------------------------------------*/
/*                        INCLUDE C FILES                                  */
/*-------------------------------------------------------------------------*/
#include "cc/app_Login.cc"
#include "cc/app_Admin.cc"
#include "cc/app_Device.cc"
#include "cc/app_Wifi_AP.cc"
#include "cc/app_WiFiSON.cc"
#include "cc/app_Client.cc"

/*-------------------------------------------------------------------------*/
/*                           DEFINITIONS                                   */
/*-------------------------------------------------------------------------*/
#define LOGFILE     "/dev/console"
#define DBG_MSG(fmt, arg...) do { FILE *log_fp = fopen(LOGFILE, "w"); \
                                     fprintf(log_fp, "%s:%s:%d:" fmt "\n", __FILE__, __func__, __LINE__, ##arg); \
                                     fclose(log_fp); \
                                     } while(0)

#define MAX_BUFFER_LENGTH 1024


struct gerneric_key_t {
    int id;
    char feature_nm[128];
    char field_nm[128];
    char field_format[128];
    //int field_type;
    int token_module_nm;
    char token_elememt_nm[128];
    char struct_nm[128];
    int (*init_cb)();	
    int (*apply_cb)();
    int (*get_cb)();
    int (*set_cb)();

    int x_max;
    int y_max;

    int (*get_x_max_cb)();
    int (*get_y_max_cb)();
} table[] = {
    { 1, "Login",   "Login",                            "Login",                            0,"", "", NULL, NULL, get_Login,                           set_Login,            0, 0, NULL,                                                        NULL},
   
    { 2, "Admin",   "Admin_Name",                       "Admin_Name",                       0,"", "", NULL, NULL, get_Admin_Name,                      set_Admin_Name,       0, 0, NULL,                                                        NULL},
    { 2, "Admin",   "Admin_Password",                   "Admin_Password",                   0,"", "", NULL, NULL, get_Admin_Password,                  set_Admin_Password,   0, 0, NULL,                                                        NULL},
  
    { 3, "WiFi_AP", "WiFi_AP_Numbers",                  "WiFi_AP_Numbers",                  0,"", "", NULL, NULL, get_WIFI_AP_VAP_NUM,                 NULL,                 0, 0, NULL, NULL},
    { 3, "WiFi_AP", "WiFi_AP_x_Enable",                 "WiFi_AP_%d_Enable",                0,"", "", NULL, NULL, get_WIFI_AP_VAP_STAT,                NULL,                 0, 0, get_WiFi_AP_max,                                             NULL},
    { 3, "WiFi_AP", "WiFi_AP_x_InternetAccessOnly",     "WiFi_AP_%d_InternetAccessOnly",    0,"", "", NULL, NULL, get_WIFI_AP_GUEST,                   NULL,                 0, 0, get_WiFi_AP_max,                                             NULL},
    { 3, "WiFi_AP", "WiFi_AP_x_SSID",                   "WiFi_AP_%d_SSID",                  0,"", "", NULL, NULL, get_WIFI_AP_VAP_SSID,                set_WIFI_AP_VAP_SSID, 0, 0, get_WiFi_AP_max,                                             NULL},
    { 3, "WiFi_AP", "WiFi_AP_x_Password",               "WiFi_AP_%d_Password",              0,"", "", NULL, NULL, get_WIFI_AP_VAP_PSK,                 set_WIFI_AP_VAP_PSK,  0, 0, get_WiFi_AP_max,                                             NULL}, 

    { 4, "Client",  "Client_Numbers",                   "Client_Numbers",                   0,"", "", NULL, NULL, get_Client_Numbers,                  NULL,                 0, 0, NULL,                                                        NULL},
    { 4, "Client",  "Client_x_MAC",                     "Client_%d_MAC",                    0,"", "", NULL, NULL, get_Client_MAC,                      NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_SignalStrength",          "Client_%d_SignalStrength",         0,"", "", NULL, NULL, get_Client_SignalStrength,           NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_LinkRateTx",              "Client_%d_LinkRateTx",             0,"", "", NULL, NULL, get_Client_LinkRateTx,               NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_LinkRateRx",              "Client_%d_LinkRateRx",             0,"", "", NULL, NULL, get_Client_LinkRateRx,               NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_Interface",               "Client_%d_Interface",              0,"", "", NULL, NULL, get_Client_Interface,                NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_ConnectionType",          "Client_%d_ConnectionType",         0,"", "", NULL, NULL, get_Client_ConnectionType,           NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_IPv4Address",             "get_Client_%d_IPv4Address",        0,"", "", NULL, NULL, get_Client_IPv4Address,              NULL,                 0, 0, get_Client_MAC_max,                                          NULL},
    { 4, "Client",  "Client_x_IPv6Address",             "get_Client_%d_IPv6Address",        0,"", "", NULL, NULL, get_Client_IPv6Address,              NULL,                 0, 0, get_Client_MAC_max,                                          NULL},

    { 5, "Device",  "Device_ModelNumber",               "Device_ModelNumber",               0,"", "", NULL, NULL, get_Device_ModelNumber,              NULL,                 0, 0, NULL,                                                        NULL},
    { 5, "Device",  "Device_NickName",                  "Device_NickName",                  0,"", "", NULL, NULL, get_Device_NickName,                 set_Device_NickName,  0, 0, NULL,                                                        NULL},
    { 5, "Device",  "Device_SN",                        "Device_SN",                        0,"", "", NULL, NULL, get_Device_SN,                       NULL,                 0, 0, NULL,                                                        NULL},
    { 5, "Device",  "Device_HWVersion",                 "Device_HWVersion",                 0,"", "", NULL, NULL, get_Device_HWVersion,                NULL,                 0, 0, NULL,                                                        NULL},
    { 5, "Device",  "Device_FWVersion",                 "Device_FWVersion",                 0,"", "", NULL, NULL, get_Device_FWVersion,                NULL,                 0, 0, NULL,                                                        NULL},

    { 6, "WiFiSON", "WiFiSON_Number",                   "WiFiSON_Number",                   0,"", "", NULL, NULL, get_WiFiSON_Number,                  NULL,                 0, 0, NULL,                                                        NULL},
    { 6, "WiFiSON", "WiFiSON_x_MAC",                    "WiFiSON_%d_MAC",                   0,"", "", NULL, NULL, get_WiFiSON_x_MAC,                   NULL,                 0, 0, get_WiFiSON_Number_MAX,                                      NULL},
    { 6, "WiFiSON", "WiFiSON_x_ParentAPMAC",            "WiFiSON_%d_ParentAPMAC",           0,"", "", NULL, NULL, get_WiFiSON_x_ParentAPMAC,           NULL,                 0, 0, get_WiFiSON_Number_MAX,                                      NULL},
    { 6, "WiFiSON", "WiFiSON_x_SignalStrength",         "WiFiSON_%d_SignalStrength",        0,"", "", NULL, NULL, get_WiFiSON_x_SignalStrength,        NULL,                 0, 0, get_WiFiSON_SignalStrength_Number_MAX,                       NULL},
    { 6, "WiFiSON", "WiFiSON_x_BackhaulLinkIndicator",  "WiFiSON_%d_BackhaulLinkIndicator", 0,"", "", NULL, NULL, get_WiFiSON_x_BackhaulLinkIndicator, NULL,                 0, 0, get_WiFiSON_BackhaulLinkIndicator_Number_MAX,                NULL},
    { 6, "WiFiSON", "WiFiSON_NickName",                 "WiFiSON_NickName"                , 0,"", "", NULL, NULL, get_WiFiSON_NickName,                set_WiFiSON_NickName, 0, 0, NULL,                                                        NULL},
    { 0, "",        "",                                 "",                                 0,"", "", NULL, NULL, NULL,                                NULL,                 0, 0, NULL,                                                        NULL},
};

struct reload_time_t {
    int id;
    char feature_nm[128];
    int restart_time;
    int (*init_cb)();
    int (*apply_cb)();
} reload_table[] = {
    { 1, "LAN",                     0, NULL,                            NULL},
    { 2, "WiFi_AP",                 120, init_App_Wifi_AP,   apply_App_Wifi_AP},
    { 3, "WiFi_Client",             0,  NULL,                            NULL},
    { 4, "Client",                  0,  init_Client,                     NULL},
    { 5, "Schedule",                0,  NULL,                            NULL},
    { 6, "Login",                   0,  NULL,                            NULL},
    { 7, "Admin",                   0,  NULL,                            NULL},
    { 8, "WPS",                     0,  NULL,                            NULL},
    { 9, "SiteSurvey",              0,  NULL,                            NULL},
    { 10, "Wizard",                 0,  NULL,                            NULL},
    { 11, "Admin",                  0,  NULL,                            NULL},
    { 12, "FactoryDefault_Reboot",  240,NULL,                            NULL},
    { 13, "Device",                 0,  NULL,                            NULL},
    { 14, "CAPTCHA",                0,  NULL,                            NULL},
    { 15, "Statistic",              0,  NULL,                            NULL},
    { 16, "Time",                   0,  NULL,                            NULL},
    { 17, "WiFiSON",                0, init_App_WiFiSON,                NULL},
    { 0,  "",                       0,                                       },
};
/*-------------------------------------------------------------------------*/
/*                           VARIABLES                                     */
/*-------------------------------------------------------------------------*/

char *pcontent = NULL;
char g_content[MAX_BUF_SIZE+1] = {0};
char *g_buf = NULL;

json_object *g_json = NULL;
char app_uid[USER_SESSION_ID_LENGTH+1] = {0};

struct gerneric_key_t *pEntry;
struct reload_time_t *rEntry;

char featureList[1024] = {0};
int reload_time = 0;


/*-------------------------------------------------------------------------*/
/*                           FUNCTIONS                                     */
/*-------------------------------------------------------------------------*/


int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    int ret = 0;
	
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
    BIO_write(bio, buffer, length);
    ret = BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    ret = BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    *b64text=(*bufferPtr).data;
	
    return (0); //success
}


int genRandomID(char *buf, int length)
{
    unsigned char randomID[64] = {0};
    char *base64EncodeOutput = NULL;

    if(RAND_bytes(randomID, length) == 0)
    {
        return FALSE;
    }

    Base64Encode(randomID, sizeof(randomID), &base64EncodeOutput);
    strncpy(buf, base64EncodeOutput, length);

    return TRUE;
}

int cmd_popen(char *cmd, char *cmd_result)
{
    FILE * fp;
    char buffer[MAX_BUFFER_LENGTH] = {0};
    fp = popen(cmd, "r");
    if (fp == NULL )
        return 0;

    while(fgets(buffer, sizeof(buffer), fp))
    {
        strcat(cmd_result, buffer);
        memset(buffer, 0x0, sizeof(buffer));
    }
    pclose(fp);

    return 0;
}

char hex2int(unsigned char hex)
{
    hex = hex - '0';
    if (hex > 9) {
        hex = (hex + '0' - 1) | 0x20;
        hex = hex - 'a' + 11;
    }
    if (hex > 15)
        hex = 0xFF;

    return hex;
}

int buffer_urldecode_internal(char *str, int is_query)
{
    unsigned char high, low;
    const char *src;
    char *dst;

    if (!str) return -1;

    src = (const char*) str;
    dst = (char*) str;

    while ((*src) != '\0') {
        if (is_query && *src == '+') {
            *dst = ' ';
        } else if (*src == '%') {
            *dst = '%';

            high = hex2int(*(src + 1));
            if (high != 0xFF) {
                low = hex2int(*(src + 2));
                if (low != 0xFF) {
                    high = (high << 4) | low;

                    /* map control-characters out */
                    if (high < 32 || high == 127) high = '_';

                    *dst = high;
                    src += 2;
                }
            }
        } else {
            *dst = *src;
        }

        dst++;
        src++;
    }

    *dst = '\0';

    return 0;
}


int init(char *feature_nm)
{
    if (strstr(featureList, feature_nm) != NULL)
        return RESULT_OK;

    strcat(featureList, feature_nm);
    strcat(featureList, "|");

    for (rEntry = reload_table; rEntry->id; rEntry++) {
        if (strcmp(rEntry->feature_nm, feature_nm) == 0)
        {
            if (rEntry->init_cb) rEntry->init_cb();
        }
    }

    return RESULT_OK;
}

int apply()
{
    char *feature_nm = NULL;

    reload_time = 0;


    feature_nm = strtok(featureList, "|");
    while (feature_nm != NULL)
    {
        //Count Reload Time
        for (rEntry = reload_table; rEntry->id; rEntry++) {
            if (strcmp(rEntry->feature_nm, feature_nm) == 0)
            {
                reload_time += rEntry->restart_time;
                
                if (rEntry->apply_cb) rEntry->apply_cb();
            }
        }
        
        feature_nm = strtok (NULL, "|");
    }
    return RESULT_OK;
}


int is_number(char *str)
{
    int i = 0;
    for ( i = 0; str[i] != '\0'; i++)
    {
        if (isdigit(str[i]) == 0)
            return FALSE;
    }

    return TRUE;
}

int parser_x_y(char *json_node_key, int *index_x, int *index_y)
{

    char *p = NULL;
    char split_str[128] = {0};


    sprintf(split_str, "%s", json_node_key);
    p = strtok(split_str, "_");
    while (p != NULL)
    {
        if ((is_number(p)) && (*index_x == -1))
            *index_x = atoi(p);
        else if ((is_number(p)) && (*index_x != -1) && (*index_y == -1))
            *index_y = atoi(p);
            
        p = strtok (NULL, "_");
    }

    return RESULT_OK;
}

int parser_field_nm_x_y(char *json_node_key, char *field_nm, int *index_x, int *index_y)
{
    char *p = NULL;
    char split_str[128] = {0};


    sprintf(split_str, "%s", json_node_key);
    p = strtok(split_str, "_");
    while (p != NULL)
    {
        if ((is_number(p)) && (*index_x == -1))
        {
            *index_x = atoi(p);
            strcat(field_nm, "x");
        
        }
        else if ((is_number(p)) && (*index_x != -1) && (*index_y == -1))
        {
            *index_y = atoi(p);
            strcat(field_nm, "y");
        }
        else
            strcat(field_nm, p);
        
        p = strtok (NULL, "_");
        
        if (p != NULL)
            strcat(field_nm, "_");
    }

    return RESULT_OK;
}

int add_into_g_json(char *key, char *result)
{
    int ret = 0;
    json_object *get_json_node_key = NULL;

    get_json_node_key = json_object_object_get(g_json, key);

    json_object_object_get(g_json, key);

            
    if (get_json_node_key == NULL)
        json_object_object_add(g_json, key, json_object_new_string(result));
    else
        json_object_set_string(get_json_node_key, result);

    return RESULT_OK;
}



int do_pEntry_action(struct gerneric_key_t *pEntry_action, char *key, json_object *json_node_obj, int index_x, int index_y)
{
    int ret = RESULT_OK;
    //Max buffer is MAX_BUFFER_LENGTH, Max value is MAX_BUFFER_LENGTH - 2;
    char value[MAX_BUFFER_LENGTH] = {0};
    char result[MAX_BUFFER_LENGTH] = {0};

    long type = 0;

    int method = -1;



    init(pEntry_action->feature_nm);


    method = ldap_method();

    if (method == 1)
        snprintf(value, sizeof(result)-1, "%s", json_object_get_string(json_node_obj));

    //Max value length is MAX_BUFFER_LENGTH-2
    if(strlen(value) == (MAX_BUFFER_LENGTH-1))
        return RESULT_ERROR;

    type = -1;
    /* 0:GET_CGI 1:SET_CGI 
    if (strcmp(pEntry_action->token_elememt_nm, "") == 0)
        type = -1;
    else
        type = tok_get_enum_type(pEntry_action->token_elememt_nm);*/

    //DBG_MSG("[NICKI] type = %d, pEntry->token_elememt_nm = %s, method = %d", type, pEntry->token_elememt_nm, method);	

    switch (type)
    {
        /*
        case TYPE_int:
        case TYPE_INT:
            if (method == 0)
                sprintf(result, "%d", tok_geint_value(pEntry_action->token_elememt_nm));
            else 
                tok_seint_value(atoi(value), pEntry_action->token_elememt_nm);
                
            ret |= RESULT_OK;
            break;
        case TYPE_STR:
        case TYPE_HOSTNAME:
            if (method == 0)
                sprintf(result, "%s", tok_get_str_value(pEntry_action->token_elememt_nm));
            else
                tok_set_value_by_str(value, pEntry_action->token_elememt_nm);
                    
            ret |= RESULT_OK;
            break;
        */
        default:
            
            //if (pEntry_action->init_cb) pEntry_action->init_cb();
                    
            if (method == 0)
            {
                if (pEntry_action->get_cb)
                {
                    ret |= pEntry_action->get_cb(key, result, index_x, index_y);
                    result[MAX_BUFFER_LENGTH-1] = '\0';
                    //Max value length is MAX_BUFFER_LENGTH-2
                    if(strlen(result) == (MAX_BUFFER_LENGTH-1))
                        return RESULT_ERROR;
                }
            }
            else
            {
                if (pEntry_action->set_cb)
                    ret = pEntry_action->set_cb(key, value, index_x, index_y);
            }
            //if (pEntry_action->apply_cb) pEntry_action->apply_cb();
            
    }


    if (ret & RESULT_ERROR)
        return ret;
        
    if (method == 0)
        add_into_g_json(key, result);


    return ret;
}

int carry_out_tasks(char *feature, char *json_node_key, json_object *json_node_obj)
{
    int ret = RESULT_OK;
    int method = -1;

    int index_x = -1, index_y = -1;
    int i = -1, j = -1;

    char key[128] = {0};
    char field_nm[128] = {0};
    int x_max = -1, y_max = -1;

    memset(featureList, 0x0, sizeof(featureList));

    /* 0:GET_CGI 1:SET_CGI */
    method = ldap_method(); 

    //DBG_MSG("[NICKI] method = %d", method);


    /* We just support 'GET_CGI' with object type */
    if ((feature != NULL) && (method != 0))
        return RESULT_ERROR;

        
    for (pEntry = table; pEntry->id; pEntry++) {
        
        /* Group */
        if (feature != NULL)
        {
            if (strcmp(pEntry->feature_nm, feature) != 0)
                continue;
            
            if ((pEntry->x_max == 0) && (pEntry->y_max == 0) && (pEntry->get_x_max_cb == NULL) && (pEntry->get_y_max_cb == NULL))
            {
                ret |= do_pEntry_action(pEntry, pEntry->field_nm, json_node_obj, -1, -1);
                if (ret & RESULT_ERROR)
                    return ret;
            }
            else
            {
                if (pEntry->get_x_max_cb)
                {
                    x_max = pEntry->get_x_max_cb();
                    x_max -= 1;
                }
                else
                    x_max = pEntry->x_max;
                
                if (pEntry->get_y_max_cb)
                {
                    y_max = pEntry->get_y_max_cb();
                    y_max -= 1;
                }
                else
                    y_max = pEntry->y_max;
                
                
                DBG_MSG("x_max = %d, y_max = %d", x_max, y_max);
                
                for(i = -1; i < x_max; i++)
                {
                    memset(key, 0x0, sizeof(key));
                    
                    for(j = -1; j < y_max; j++)
                    {
                        index_x = (i + 1);
                        index_y = (j + 1);
                        
                        if ((pEntry->get_y_max_cb == NULL) && (pEntry->y_max == 0))
                            index_y = -1;

                        sprintf(key, pEntry->field_format, index_x, index_y);
                        ret |= do_pEntry_action(pEntry, key, json_node_obj, index_x, index_y);
                        
                        if (ret & RESULT_ERROR)
                            return ret;

                    }
                }
                continue;
            }
        }
        
        /* Single Node */
        if (json_node_key != NULL)
        {
            if (strstr(json_node_key, pEntry->feature_nm) == NULL)
                continue;
            
            if (strcmp(json_node_key, pEntry->field_nm) != 0)
            {
                index_x = -1;
                index_y = -1;
                
                memset(field_nm, 0x0, sizeof(field_nm));
                parser_field_nm_x_y(json_node_key, field_nm, &index_x, &index_y);
            
                //DBG_MSG("[NICKI] pEntry->field_nm = %s, json_node_key = %s, field_nm = %s, x = %d, y = %d", pEntry->field_nm, json_node_key, field_nm, index_x, index_y);
                if (strcmp(field_nm, pEntry->field_nm) != 0)
                    continue;
            }
            
            ret |= do_pEntry_action(pEntry, json_node_key, json_node_obj, index_x, index_y);
            
            if (ret & RESULT_ERROR)
                return ret;
            
            break;
        }
    }
#if 0
    if (method == 1)
        apply();
#endif

    return ret;
}

int execute_object(){
    int ret = RESULT_OK;

    json_object *clone_g_json = NULL;
    enum json_type type;

    json_object_deep_copy(g_json, &clone_g_json, NULL);

    json_object_object_foreach(clone_g_json, key, jval) {
        type = json_object_get_type(jval);
        switch(type) {
        case json_type_null:
        case json_type_boolean:
        case json_type_double:
        case json_type_int:
        case json_type_string:
            ret |= carry_out_tasks(NULL, key, jval);
            
            //DBG_MSG("Type: json_type_string, key: %s, value: %s, ret = %d\n", key, json_object_get_string(jval), ret);
            break;
        case json_type_object:
            json_object_object_del(g_json, key);
            
            ret |= carry_out_tasks(key, NULL, NULL);
            
            break;
        case json_type_array:
        /*
            print_json_value(key, jval);
            json_parse_array(jobj, key);
        */
            break;

        }
        
        if (ret & RESULT_ERROR)
            return RESULT_ERROR;
    }
    json_object_put(clone_g_json);


    return ret;
}


int getUserSessionID(char *userSessionID, int length)
{
    char *pCookie = NULL;


    pCookie = getenv("HTTP_COOKIE");
    if (pCookie == NULL)
        return 0;

    buffer_urldecode_internal(pCookie, 0);

    pCookie = strstr(pCookie, "uid=");
    if (pCookie == NULL)
        return 0;
    pCookie +=4;

    pCookie[USER_SESSION_ID_LENGTH] = '\0';
    strncpy(userSessionID, pCookie, length);




    return 1;
}

int add_user(char *uid){
    char *remote_addr = NULL;
        
    char cmd_result[1024]  = {0};
    char cmd[1024]  = {0};

    time_t current_uptime;


    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));
    sprintf(cmd, "awk '{print $1}' /proc/uptime");
    cmd_popen(cmd, cmd_result);

    current_uptime = atol(cmd_result);
    DBG_MSG("current_uptime = %ld", current_uptime);

    remote_addr = getenv("REMOTE_ADDR");

    //Update the session info
    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));
    sprintf(cmd, "sed -i '/%s/d' /tmp/user.list", uid);
    cmd_popen(cmd, cmd_result);

    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));
    sprintf(cmd, "echo '%s %s %ld' > /tmp/user.list", uid, remote_addr, current_uptime);
    cmd_popen(cmd, cmd_result);	

    return RESULT_OK;
}

int do_check_uid(){

    //return RESULT_OK;

    char *remote_addr = NULL;
    char cookie_uid[USER_SESSION_ID_LENGTH+1] = {0};
    int obj_count = 0, is_login_stat = 0; 
    char cmd_result[1024]  = {0};
    char cmd[1024]  = {0};

    char uid[USER_SESSION_ID_LENGTH+1]  = {0}, ipaddr[28]  = {0};

    json_object *obj_login = json_object_object_get(g_json, "Login");

    time_t current_uptime;
    time_t session_uptime;


    json_object_object_foreach(g_json, key, jval) {
        if ((json_object_get_type(jval) == json_type_string) && (strcmp(key, "Login") ==0))
            is_login_stat = 1;
            
        obj_count++;
    }
        
    if ((obj_login != NULL) && (is_login_stat == 1) && (obj_count == 1))
        return RESULT_OK;

    getUserSessionID(cookie_uid, USER_SESSION_ID_LENGTH+1);

    if (strcmp(cookie_uid, "") == 0)
    {
        return RESULT_AUTH_FAIL;
    }

    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));
    sprintf(cmd, "awk '{print $1}' /proc/uptime");
    cmd_popen(cmd, cmd_result);

    current_uptime = atol(cmd_result);
    remote_addr = getenv("REMOTE_ADDR");



    memset(cmd, 0x0, sizeof(cmd));
    memset(cmd_result, 0x0, sizeof(cmd_result));

    sprintf(cmd, "grep -rw '/tmp/user.list' -e '%s'", cookie_uid);
    cmd_popen(cmd, cmd_result);

    DBG_MSG(" === file = %s", cmd_result);

    if (strcmp(cmd_result, "") == 0)
    {
        return RESULT_AUTH_FAIL;
    }

    sscanf(cmd_result, "%s %s %ld", uid, ipaddr, &session_uptime);


    //DBG_MSG("cookie uid = %s, file uid = %s", cookie_uid, uid);
    //DBG_MSG("current_uptime = %ld, session_uptime = %ld", current_uptime, session_uptime);
    //DBG_MSG("remo_addr = %s, file ipaddr = %s", remote_addr, ipaddr);

    if (session_uptime == 0)
        return RESULT_AUTH_FAIL;

    if (strcmp(cookie_uid, uid) != 0)
        return RESULT_AUTH_FAIL;

    if (strcmp(remote_addr, ipaddr) != 0)
        return RESULT_AUTH_FAIL;

    if (current_uptime > (session_uptime + 300))
    {
        //Update the session info
        memset(cmd, 0x0, sizeof(cmd));
        memset(cmd_result, 0x0, sizeof(cmd_result));
        sprintf(cmd, "sed -i '/%s/d' /tmp/user.list", cookie_uid);
        cmd_popen(cmd, cmd_result);
        
        return RESULT_AUTH_FAIL;
    }
    add_user(cookie_uid);
    /*	
    DBG_MSG("===================================");


    DBG_MSG("tok uid = %s", tok_get_str_value(TOK_HTTP_SESSION_ID_N, 0));
    DBG_MSG("tok session_uptime = %s", tok_get_str_value(TOK_HTTP_SESSION_UPTIME_N, 0));
    DBG_MSG("tok remo_addr = %s", tok_get_str_value(TOK_HTTP_SESSION_IP_N, 0));

    DBG_MSG("###################################");

    if (strcmp(tok_get_str_value(TOK_HTTP_SESSION_ID_N, 0), "") == 0)
        return RESULT_AUTH_FAIL;

    if (strcmp(cookie_uid, tok_get_str_value(TOK_HTTP_SESSION_ID_N, 0)) != 0)
        return RESULT_AUTH_FAIL;

    if (strcmp(remote_addr, tok_get_str_value(TOK_HTTP_SESSION_IP_N, 0)) != 0)
        return RESULT_AUTH_FAIL;

    if (current_uptime > (session_uptime + CFG_MGT_WEB_AUTH_TIMEOUT))
        return RESULT_AUTH_FAIL;

    addSessionToken(cookie_uid);
    */	
    return RESULT_OK;
    }

    void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {
        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void reload_query(void){
    char lock[]="0";
    sysinteract("",lock, 1 , "uci get system.lock 2>&1");
    
    if (!strcmp(lock , "1"))
    {
        system("uci set system.reload=1");
        return;
    }
    system("uci set system.lock=1");
    system("killall /etc/cgibox.sh");
    system("/etc/cgibox.sh &");
    return;
}

int do_action(){
    int ret = RESULT_OK;

    json_object *jobj_result = NULL;
    json_object *jnode_result = json_object_new_string("OK");

    char buf[128]={0};

    jobj_result = json_object_new_object();

    /* Check the Auth */
    ret = do_check_uid();

    /* Handle the GET/SET  */
    if (ret == RESULT_OK)
        ret = execute_object();

    /* Handle the result */
    if (ret & RESULT_ERROR)
    {
        system("rm -rf /tmp/.uci/wireless"); // delete uci set wireless
        json_object_set_string(jnode_result, "Error");
    }
    else if (ret & RESULT_TOKEN_CHANGE)
    {
        apply();
	sprintf(buf, "Restart, %d", reload_time);
	json_object_set_string(jnode_result, buf);
        //reload_query();
    } 
    else if (ret & RESULT_TOKEN_REBOOT)
    {
        json_object_set_string(jnode_result, "Reboot, 240");
    }
    else if (ret & RESULT_AUTH_SUCCESS)
    {
        genRandomID(app_uid, USER_SESSION_ID_LENGTH);
        
        DBG_MSG("app_uid = %s", app_uid);
        
        add_user(app_uid);
        
        json_object_set_string(jnode_result, "OK");
        json_object *jobj_uid = json_object_new_string("uid");
        json_object_set_string(jobj_uid, app_uid);
        json_object_object_add(jobj_result, "uid", jobj_uid);
        memset(app_uid, 0x0, sizeof(app_uid));
            
        ret = RESULT_OK;
    }
    else if (ret & RESULT_AUTH_FAIL)
    {
        json_object_set_string(jnode_result, "Error-AUTH");
        ret = RESULT_ERROR;
    }
    else if (ret & RESULT_OK)
    {
        json_object_set_string(jnode_result, "OK");	
    }



    if ((ldap_method() == 0) && (ret & RESULT_OK))
    {
        json_object_object_add(g_json, "Result", jnode_result);
        printf(json_object_to_json_string(g_json));
    }
    else
    {
        json_object_object_add(jobj_result, "Result", jnode_result);
        printf(json_object_to_json_string(jobj_result));
    }

    json_object_put(jobj_result);
    return ret;
}


void doc_destory()
{

    if(NULL != g_json)
    {
        json_object_put(g_json);
        g_json = NULL;
    }

    if((NULL != pcontent))
    {
        free(pcontent);
        pcontent = NULL;
    }

    g_buf = NULL;
}

int doc_init(void)
{
    long int len;
    char* pData = NULL;
    long int ilen = atoi(getenv("CONTENT_LENGTH"));
    int ret = 0;

    doc_destory();

    if (0 >= ilen) {
        DBG_MSG("CONTENT_LENGTH is smaller than the zero\n");
        return 105;
    }

    ilen = ilen + 1;

    pcontent = malloc(ilen);
    g_buf = pcontent;
    len = ilen;
   
    pData   = g_buf;
    memset(pData, 0, len+1);

    DBG_MSG("len=%ld, ilen=%ld\n", len, ilen);

    while ( (len > 0) && (fgets(pData, len+1, stdin) != NULL) )
    {
        len -= strlen(pData);
        pData += strlen(pData);
    }
    DBG_MSG("g_buf=(%s)\n", g_buf);

     
     if (ldap_method() > -1){
        g_json = json_tokener_parse(g_buf);
        if (g_json == NULL)
            ret = 105;
        else
        {
            DBG_MSG("g_json.to_string()=%s\n", json_object_to_json_string(g_json));

            ret = 0;
        }
     }
     

    return ret;
}


int ldap_method()
{
    char *pURL = getenv("REQUEST_URI");

    if (strstr(pURL, "GET_CGI") != NULL)
        return 0;

    if (strstr(pURL, "SET_CGI") != NULL)
        return 1;

    return -1;
}

int main(int argc, char** argv)
{
    while (FCGI_Accept() >= 0)
    {
        if ((ldap_method() < 0) || (ldap_method() > 2))
        {
            printf("Status: 400\r\n");
            printf("Content-type: text/xml; charset=utf-8\r\n");
            printf("Content-Length: 0\r\n");
            printf("\r\n");
        }
        else 
        {
            doc_init();
            
            printf("Content-Type: text/xml\n");
            printf("Transfer-Encoding: chunked\n");
            printf("\r\n");
            
            do_action();
        
            doc_destory();
        }
        FCGI_Finish();
    }

    return 0;
}

