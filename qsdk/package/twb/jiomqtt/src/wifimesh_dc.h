//
// Created by jio on 8/14/18.
//

#ifndef WIFIMESH_DATACOLLECTOR_WIFIMESH_DC_H
#define WIFIMESH_DATACOLLECTOR_WIFIMESH_DC_H

typedef struct {
    int conf_logLevel;
    int conf_startupWaitSec;
    char *conf_topicRoot;
    int conf_intervalSec;
    char *conf_offlineStore;
    int conf_destroyPollSec;

    char *server_address;
    char *server_port;
    char *server_username;
    char *server_password;
    int server_qos;

    char *ssl_trustStore;
    char *ssl_keyStore;
    char *ssl_privateKey;
    char *ssl_privateKeyPassword;

    int connection_keepAliveInterval;
    int connection_cleanSession;
    int connection_maxInflight;
    int connection_connectTimeout;
    int connection_retryInterval;
    int connection_autoReconnect;
    int connection_minRetryInterval;
    int connection_maxRetryInterval;
} configuration;

configuration config;

typedef enum {
    CONNECT,
    CONNECT_INPROGRESS,
    CONNECT_DONE,
    CONNECT_ERROR,

    PUBLISH,
    PUBLISH_INPROGRESS,
    PUBLISH_DONE,
    PUBLISH_ERROR,

    DISCONNECT,
    DISCONNECT_INPROGRESS,
    DISCONNECT_DONE,
    DISCONNECT_ERROR,

    DESTROY,

    DONE
} connection_state;

connection_state cstate;

#endif //WIFIMESH_DATACOLLECTOR_WIFIMESH_DC_H
