#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <time.h>
#include <sys/wait.h>
#include <getopt.h>
#include <ini.h>
#include "wifimesh_dc.h"
#include "log.h"
#include "wifimesh_dc_utils.h"
#include "common_utils.h"
#include "jiot_client_mqtt.h"


#define CLIENT_ID_SIZE 64
#define MODEL_ID_SIZE 64
#define MAIN_TOPIC_SIZE 256
#define STATS_JSON_DEBUG_PRINT_SIZE 128

char *dc_get_stats();

void dc_publish_stats();

int publishMessage(const char *message);

char *stats_json = NULL;
char client_id[CLIENT_ID_SIZE];
char model_id[MODEL_ID_SIZE];    
char main_topic[MAIN_TOPIC_SIZE];
jiot_client_MQTT_Hndl_t mqttClient;
int doStop = 0;

/*TWB EAP: Get device role*/
char device_role[8];

/*Callback for MQTT client connection complete with broker.
 Client applications should implement this to get connection complete status. */
void connectionComplete(void *context, jiot_client_MQTT_token_t token) {
    LDEBUG("Connection established");
//    jiot_client_MQTT_Hndl_t client = context;
    LDEBUG("State change -> CONNECT_DONE");
    cstate = CONNECT_DONE;
//    if (!doStop) {
//        char *message = stats_json;
//        publishMessage(client, message);
//    } else {
//        LWARNING("Disconnecting becz of doStop %d", doStop);
//        jiot_client_MQTT_disconnect(client);
//    }
}

//Function to publish a message to MQTT broker.
int publishMessage(const char *message) {
    int retVal = JIOT_MQTT_OPERATION_FAILURE;
    int length = (int) strlen(message);
    LDEBUG("Publishing with Qos[%d]", (jiot_client_MQTT_qos_t) config.server_qos);
    retVal = jiot_client_MQTT_publishWithQos(mqttClient, main_topic, message, length,
                                             (jiot_client_MQTT_qos_t) config.server_qos);
    if (retVal >= 0) {
        LDEBUG("Publish initiated with token id %d", retVal);
    } else {
        int published = 0;
        for (int i = 1; i <= 5; i++) {
            usleep((__useconds_t) (i * 1000000));
            retVal = jiot_client_MQTT_publishWithQos(mqttClient, main_topic, message, length,
                                                     (jiot_client_MQTT_qos_t) config.server_qos);
            if (retVal >= 0) {
                LWARNING("Publish initiated with token id %d on retry %d", retVal, i);
                published = 1;
                break;
            }
        }

        if (!published) {
//            LERROR("Disconnecting becz of error in publishing %d", retVal);
//            jiot_client_MQTT_disconnect(mqttClient);
            return retVal;
        }
    }

    return 0;
}

/*Callback for MQTT client connection failed with broker. 
Client applications should implement this to get connection failed status. */
void connectionFailed(void *context, const char *cause, int code) {
    LERROR("Connection failed with error %d", code);
    LDEBUG("State change -> CONNECT_ERROR");
    cstate = CONNECT_ERROR;
//    doStop = 1;
}

/*Callback for MQTT message delivery complete. 
Client applications should implement this to get delivery complete status. */
void messageDelivered(void *context, jiot_client_MQTT_token_t token) {
    jiot_client_MQTT_Hndl_t client = context;
    LINFO("Message delivery completed for message id %d", token);
    LDEBUG("State change -> PUBLISH_DONE");
    cstate = PUBLISH_DONE;
//    LDEBUG("Disconnecting client");
//    jiot_client_MQTT_disconnect(client);
}

/*Callback for MQTT message delivery failure. 
Client applications should implement this to get delivery failure cause. */
void messageFailed(void *context, const char *cause, jiot_client_MQTT_token_t token) {
    jiot_client_MQTT_Hndl_t client = context;
    LERROR("Message delivery failed for message id %d with exception %s", token, cause);
    LDEBUG("State change -> PUBLISH_ERROR");
    cstate = PUBLISH_ERROR;
//    LDEBUG("Disconnecting client");
//    jiot_client_MQTT_disconnect(client);
}

/*Callback for MQTT client disconnect complete with broker. 
Client applications should implement this to get disconnection complete. */ 
void disconnectCompleted(void *context) {
    LDEBUG("Disconnect completed");
    LDEBUG("State change -> DISCONNECT_DONE");
    cstate = DISCONNECT_DONE;
    //doStop = 1;
}

/*Callback for MQTT client disconnect failed with broker. 
Client applications should implement this to get disconnection failure. */ 
void disconnectFailed(void *context, const char *message, int code) {
    LERROR("Disconnect failed with error code %d", code);
    LDEBUG("State change -> DISCONNECT_ERROR");
    cstate = DISCONNECT_ERROR;
    //doStop = 1;
}

//Function to handle program-stop conditions.
void stopHandler(int sig) {
    signal(SIGINT, NULL);
    signal(SIGTERM, NULL);
    doStop = 1;
}

//Function to create the MQTT Client, set callbacks and connect a created client to an MQTT Broker.
int do_connect() {
    jiot_client_MQTT_createOpts_t options = CreateOptions_initializer;
    int retVal = JIOT_MQTT_OPERATION_FAILURE;
    jiot_client_MQTT_callbackOpts_t callbacks;

    options.clientId = client_id;
    options.username = config.server_username;
    options.password = config.server_password;
    options.host = config.server_address;
    options.port = config.server_port;

    if (str_startswith(options.host, "ssl://")
        || str_startswith(options.host, "wss://")
        || str_startswith(options.host, "tls://")) {
        options.useSSL = 1;
    }

    LDEBUG("Connecting with host:[%s] port:[%s] useSSL:[%d] clientId:[%s]",
           options.host, options.port, options.useSSL, options.clientId);
    retVal = jiot_client_MQTT_create(&mqttClient, options);

    if (retVal == JIOT_MQTT_OPERATION_SUCCESS) {
        LDEBUG("Client created");
    } else {
        LERROR("Error in client creation [%d]", retVal);
        return -1;
    }

    // callbacks
    memset(&callbacks, 0x00, sizeof(jiot_client_MQTT_callbackOpts_t));
    callbacks.connectionComplete = connectionComplete;
    callbacks.connectionFailed = connectionFailed;
    callbacks.deliveryComplete = messageDelivered;
    callbacks.deliveryFailed = messageFailed;
    callbacks.disconnectComplete = disconnectCompleted;
    callbacks.disconnectFailed = disconnectFailed;
    jiot_client_MQTT_setcallbacks(mqttClient, mqttClient, callbacks);

	//create struct with 
	//jiot_client_MQTT_connectOpts_t 
    //{60, 1, 10, 30, 0, 0, 1, 1, 60, NULL, NULL}
    jiot_client_MQTT_connectOpts_t connOpts = ConnectOptions_initializer;

	connOpts.keepAliveInterval = 60;
	connOpts.cleanSession = 1;
	connOpts.maxInflight = 10;
	connOpts.connectTimeout = 30;
	connOpts.retryInterval = 0;
	connOpts.mqttVersion = 0;
	connOpts.autoReconnect = 1;
	connOpts.minRetryInterval = 1;
	connOpts.maxRetryInterval = 60;

        jiot_client_MQTT_sslOpts_t dcSSLOptions = {config.ssl_trustStore, NULL, NULL, NULL, NULL, 1};
        connOpts.sslOptions = &dcSSLOptions;

	retVal = jiot_client_MQTT_connectWithOptions(mqttClient, &connOpts);
    if (retVal == JIOT_MQTT_OPERATION_SUCCESS) {
        LDEBUG("Connect initiated");
        return 0;
    } else {
        // TODO should destroy be called ?
        LERROR("Error in client connection [%d]", retVal);
        LDEBUG("Destroying the client");
        jiot_client_MQTT_destroy(&mqttClient);
	mqttClient = NULL;
        LDEBUG("Destroyed the client");
        return -1;
    }
}

//Function for handling configuration parameters for the application.
static int config_handler(void *user, const char *section, const char *name, const char *value) {
    configuration *pconfig = (configuration *) user;

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("configuration", "logLevel")) {
        pconfig->conf_logLevel = atoi(value);
    } else if (MATCH("configuration", "startupWaitSec")) {
        pconfig->conf_startupWaitSec = atoi(value);
    } else if (MATCH("configuration", "topicRoot")) {
        free(pconfig->conf_topicRoot);
        pconfig->conf_topicRoot = NULL;
        pconfig->conf_topicRoot = strdup(value);
    } else if (MATCH("configuration", "intervalSec")) {
        pconfig->conf_intervalSec = atoi(value);
    } else if (MATCH("configuration", "offlineStore")) {
        free(pconfig->conf_offlineStore);
        pconfig->conf_offlineStore = NULL;
        pconfig->conf_offlineStore = strdup(value);
    } else if (MATCH("configuration", "destroyPollSec")) {
        pconfig->conf_destroyPollSec = atoi(value);
    } else if (MATCH("server", "address")) {
        free(pconfig->server_address);
        pconfig->server_address = NULL;
        pconfig->server_address = strdup(value);
    } else if (MATCH("server", "port")) {
        free(pconfig->server_port);
        pconfig->server_port = NULL;
        pconfig->server_port = strdup(value);
    } else if (MATCH("server", "username")) {
        free(pconfig->server_username);
        pconfig->server_username = NULL;
        pconfig->server_username = strdup(value);
    } else if (MATCH("server", "password")) {
        free(pconfig->server_password);
        pconfig->server_password = NULL;
        pconfig->server_password = strdup(value);
    } else if (MATCH("server", "qos")) {
        pconfig->server_qos = atoi(value);
    } else if (MATCH("ssl", "trustStore")) {
        free(pconfig->ssl_trustStore);
        pconfig->ssl_trustStore = NULL;
        pconfig->ssl_trustStore = strdup(value);
    } else if (MATCH("ssl", "keyStore")) {
        free(pconfig->ssl_keyStore);
        pconfig->ssl_keyStore = NULL;
        pconfig->ssl_keyStore = strdup(value);
    } else if (MATCH("ssl", "privateKey")) {
        free(pconfig->ssl_privateKey);
        pconfig->ssl_privateKey = NULL;
        pconfig->ssl_privateKey = strdup(value);
    } else if (MATCH("ssl", "privateKeyPassword")) {
        free(pconfig->ssl_privateKeyPassword);
        pconfig->ssl_privateKeyPassword = NULL;
        pconfig->ssl_privateKeyPassword = strdup(value);
    } else if (MATCH("connection", "keepAliveInterval")) {
        pconfig->connection_keepAliveInterval = atoi(value);
    } else if (MATCH("connection", "cleanSession")) {
        pconfig->connection_cleanSession = atoi(value);
    } else if (MATCH("connection", "maxInflight")) {
        pconfig->connection_maxInflight = atoi(value);
    } else if (MATCH("connection", "connectTimeout")) {
        pconfig->connection_connectTimeout = atoi(value);
    } else if (MATCH("connection", "retryInterval")) {
        pconfig->connection_retryInterval = atoi(value);
    } else if (MATCH("connection", "autoReconnect")) {
        pconfig->connection_autoReconnect = atoi(value);
    } else if (MATCH("connection", "minRetryInterval")) {
        pconfig->connection_minRetryInterval = atoi(value);
    } else if (MATCH("connection", "maxRetryInterval")) {
        pconfig->connection_maxRetryInterval = atoi(value);
    } else {
        return 0;
    }

    return 1;
}

//Function to terminate application
void terminate_application(void) {
    free(config.conf_topicRoot);
    config.conf_topicRoot = NULL;
    free(config.conf_offlineStore);
    config.conf_offlineStore = NULL;

    free(config.server_address);
    config.server_address = NULL;
    free(config.server_port);
    config.server_port = NULL;
    free(config.server_username);
    config.server_username = NULL;
    free(config.server_password);
    config.server_password = NULL;

    free(config.ssl_trustStore);
    config.ssl_trustStore = NULL;
    free(config.ssl_keyStore);
    config.ssl_keyStore = NULL;
    free(config.ssl_privateKey);
    config.ssl_privateKey = NULL;
    free(config.ssl_privateKeyPassword);
    config.ssl_privateKeyPassword = NULL;

    LOGCLOSE();
}

void print_app_usage() {
    fprintf(stdout, "Usage: ONT Data Collector version 2.3\n\t1. Execute without options to run in daemon mode.\n");
    fprintf(stdout, "\t2. Execute with \"-o\" option to run in STANDALONE ONCE mode.\n");
}

void dump_config_data() {
    LDEBUG("configuration logLevel=[%d] startupWaitSec=[%d] topicRoot=[%s] "
                   "intervalSec=[%d] offlineStore=[%s] destroyPollSec=[%d]",
           config.conf_logLevel, config.conf_startupWaitSec, config.conf_topicRoot,
           config.conf_intervalSec, config.conf_offlineStore, config.conf_destroyPollSec);
    LDEBUG("server address=[%s] port=[%s] username=[%s] password=[%s]",
           config.server_address, config.server_port, config.server_username, config.server_password);
    LDEBUG("ssl trustStore=[%s] keyStore=[%s] privateKey=[%s] privateKeyPassword=[%s]",
           config.ssl_trustStore, config.ssl_keyStore, config.ssl_privateKey, config.ssl_privateKeyPassword);
    LDEBUG("connection keepAliveInterval=[%d] cleanSession=[%d] maxInflight=[%d] "
                   "connectTimeout=[%d] retryInterval=[%d] autoReconnect=[%d] "
                   "minRetryInterval=[%d] maxRetryInterval=[%d]",
           config.connection_keepAliveInterval, config.connection_cleanSession, config.connection_maxInflight,
           config.connection_connectTimeout, config.connection_retryInterval, config.connection_autoReconnect,
           config.connection_minRetryInterval, config.connection_maxRetryInterval);
}

int main(int argc, char *argv[]) {
    atexit(terminate_application);
    char* custom_config = NULL;

    //default configuration
    config.conf_logLevel = 6;
    config.conf_startupWaitSec = 600;

    config.conf_topicRoot = strdup("dc/0/wifimesh/");
    config.conf_intervalSec = 1800;
    config.conf_offlineStore = strdup("/tmp/wifimesh_datacollector");
    config.conf_destroyPollSec = 5;

    config.server_address = strdup("ws://jpsit.jpdc.jiophone.net");
    config.server_port = strdup("9797");
    config.server_username = strdup("deviceevent");
    config.server_password = strdup("deviceevent@123");
    config.server_qos = 0;

    config.ssl_trustStore = strdup("/usr/lib/ssl/certs/libicf-mqtt-root.pem");
    config.ssl_keyStore = strdup("/mnt/sdcard/jiot");
    config.ssl_privateKey = strdup("/mnt/sdcard/jiot");
    config.ssl_privateKeyPassword = strdup("Test123");

    config.connection_keepAliveInterval = 10;
    config.connection_cleanSession = 0;
    config.connection_maxInflight = 50;
    config.connection_connectTimeout = 20;
    config.connection_retryInterval = 2;
    config.connection_autoReconnect = 1;
    config.connection_minRetryInterval = 2;
    config.connection_maxRetryInterval = 50;

    int getopt_option = 0;
    bool run_once = false;

    while ((getopt_option = getopt(argc, argv, "oc:")) != -1) {
        switch (getopt_option) {
            case 'o' :
                run_once = true;
                break;
	    case 'c' :
		custom_config = optarg;
		break;
            default:
                print_app_usage();
                exit(EXIT_SUCCESS);
        }
    }

    if(custom_config != NULL) {
	    ini_parse(custom_config, config_handler, &config);
    } else {
	    ini_parse("/etc/wifimesh_dc.conf", config_handler, &config);
    }


    if (run_once) {
        LOGINITCONSOLE("wifimesh_dc");
        LOG_ENABLE_UPTO(LOG_DEBUG);
        dump_config_data();

        LOG_ENABLE_UPTO(config.conf_logLevel);
        LINFO("started data collection in ONCE mode...");

        dc_publish_stats();
        LINFO("done with data collection in ONCE mode...");
        exit(EXIT_SUCCESS);
    } else {
        LOGINIT("wifimesh_dc");
        LOG_ENABLE_UPTO(config.conf_logLevel);
        usleep((unsigned int) (config.conf_startupWaitSec * 1000000));

        while (1) {
            pid_t child_pid = fork();
            if (child_pid == 0) {
                //child process
                pid_t sub_child_pid = fork();
                if (sub_child_pid == 0) {
		    if(custom_config != NULL) {
			    ini_parse(custom_config, config_handler, &config);
		    } else {
			    ini_parse("/etc/wifimesh_dc.conf", config_handler, &config);
		    }

                    LOG_ENABLE_UPTO(config.conf_logLevel);
                    LINFO("SUB CHILD PROCESS started data collection...");
                    dc_publish_stats();
                    LINFO("SUB CHILD PROCESS done with data collection...");
                    exit(EXIT_SUCCESS);
                } else if (sub_child_pid < 0) {
                    fprintf(stderr, "sub child fork failed!!\n");
                }
                LDEBUG("exiting child process...");
                exit(EXIT_SUCCESS);
            } else if (child_pid < 0) {
                LERROR("child fork failed!!");
            } else {
                wait(child_pid);
            }
            usleep((unsigned int) (config.conf_intervalSec * 1000000));
        }
    }
}

void log_final_json(char *json_data) {
    char *data = json_data;
    char buffer[STATS_JSON_DEBUG_PRINT_SIZE + 1] = {0};

    size_t len = strlen(data);
    size_t blen = sizeof(buffer) - 1;
    size_t i = 0;

    for (i = 0; i < len / blen; ++i) {
        memcpy(buffer, data + (i * blen), blen);
        LINFO("[FINAL_JSON] '%s'", buffer);
    }

    if (len % blen) {
        LINFO("[FINAL_JSON] '%s'", data + (len - len % blen));
    }
}

//Function to publish ONT_DC statistics.
void dc_publish_stats() {
    int retVal = 0;
    memset(client_id, 0, sizeof(char) * CLIENT_ID_SIZE);
    memset(model_id, 0, sizeof(char) * MODEL_ID_SIZE);

    free(stats_json);
    stats_json = NULL;
    stats_json = dc_get_stats();
    if ((stats_json) && (strlen(client_id) > 0)) {
        //LINFO("[FINAL_JSON] %s", stats_json);
        log_final_json(stats_json);
        memset(main_topic, 0, sizeof(char) * MAIN_TOPIC_SIZE);

        // dc/0/wifimesh/{serial number}/uc/fwd/event/{model number of wifimesh device}
        if ((strlen(model_id) > 0)) {
            sprintf(main_topic, "%s%s/uc/fwd/event/%s", config.conf_topicRoot, client_id, model_id);
        } else {
            // when model_id is not available, 0 is used
            sprintf(main_topic, "%s%s/uc/fwd/event/%s", config.conf_topicRoot, client_id, "0");
        }

        LINFO("publishing on topic [%s]", main_topic);
        cstate = CONNECT;
        while (!doStop && cstate != DONE) {
            switch (cstate) {
                case CONNECT:
                    retVal = do_connect();
                    if (retVal == 0) {
                        LDEBUG("State change CONNECT -> CONNECT_INPROGRESS");
                        cstate = CONNECT_INPROGRESS;
                    } else {
                        LDEBUG("State change CONNECT -> DONE");
                        cstate = DONE;
                    }
                    break;
                case CONNECT_INPROGRESS:
                    usleep(500000L); //sleep for 0.5sec
                    break;
                case CONNECT_DONE:
                    LDEBUG("State change CONNECT_DONE -> PUBLISH");
                    cstate = PUBLISH;
                    break;
                case CONNECT_ERROR:
                    LDEBUG("State change CONNECT_ERROR -> DESTROY");
                    cstate = DESTROY;
                    break;
                case PUBLISH:
                    retVal = publishMessage(stats_json);
                    if (retVal == 0) {
                        LDEBUG("State change PUBLISH -> PUBLISH_INPROGRESS");
                        cstate = PUBLISH_INPROGRESS;
                    } else {
                        LERROR("Disconnecting becz of error in publishing %d", retVal);
                        LDEBUG("State change PUBLISH -> DISCONNECT");
                        cstate = DISCONNECT;
                    }
                    break;
                case PUBLISH_INPROGRESS:
                    usleep(500000L); //sleep for 0.5sec
                    break;
                case PUBLISH_DONE:
                    LDEBUG("State change PUBLISH_DONE -> DISCONNECT");
                    cstate = DISCONNECT;
                    break;
                case PUBLISH_ERROR:
                    LDEBUG("State change PUBLISH_ERROR -> DISCONNECT");
                    cstate = DISCONNECT;
                    break;
                case DISCONNECT:
                    jiot_client_MQTT_disconnect(mqttClient);
                    LDEBUG("State change DISCONNECT -> DISCONNECT_INPROGRESS");
                    cstate = DISCONNECT_INPROGRESS;
                    break;
                case DISCONNECT_INPROGRESS:
                    usleep(500000L); //sleep for 0.5sec
                    break;
                case DISCONNECT_DONE:
                    LDEBUG("State change DISCONNECT_DONE -> DESTROY");
                    cstate = DESTROY;
                    break;
                case DISCONNECT_ERROR:
                    LDEBUG("State change DISCONNECT_ERROR -> DESTROY");
                    cstate = DESTROY;
                    break;
                case DESTROY:
                    LDEBUG("Destroying the client");
                    jiot_client_MQTT_destroy(&mqttClient);
                    LDEBUG("Destroyed the client");
                    LDEBUG("State change DESTROY -> DONE");
                    cstate = DONE;
                    break;
                case DONE:
                    break;
            }
        }

//        if (do_connect() == 0) {
//            while (!doStop) {
//                //usleep(5000000L); //sleep for 5sec
//                usleep((unsigned int) (config.conf_destroyPollSec * 1000000));
//            }
//
//            LDEBUG("Destroying the client");
//            jiot_client_MQTT_destroy(&mqttClient);
//            LDEBUG("Destroyed the client");
//        }
//        // TODO: incase of failure should destroy be called ?

    } else {
        if (!stats_json) {
            LERROR("unable to get stats_json: [NULL]");
        } else if (strlen(client_id) <= 0) {
            LERROR("unable to get client_id: [BLANK]");
        } else {
            LERROR("unable to get stats_json");
        }
    }

    free(stats_json);
    stats_json = NULL;
}

char *dc_get_stats() {
    char *command_output = NULL;
    int32_t size = 0;
    int error = 0;

    char *device_serial_format = "\"DI1\" : \"%s\"";
    char *device_serial_buffer = NULL;
    error = get_device_serial(&command_output, &size);
    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("get_device_serial failed errno: %d (%s)", errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
        if (strlen(command_output) > 0) {
            LDEBUG("device_serial:[%s]", command_output);
            strcpy(client_id, command_output);

            error = asprintf(&device_serial_buffer, device_serial_format, command_output);
            if (error < 0) {
                LERROR("FAILED to format device serial err:[%d]", error);
                free(device_serial_buffer);
                device_serial_buffer = NULL;
            } else {
                LINFO("[JSON] serial: [%s]", device_serial_buffer);
            }
        } else {
            LERROR("FAILED to fetch device serial [blank]");
        }
    } else {
        LERROR("FAILED to fetch device serial  [null]");
    }

    free(command_output);
    command_output = NULL;

    // if serial cannot be fetched use 0 as serial
    if (!device_serial_buffer) {
        strcpy(client_id, "0");
        error = asprintf(&device_serial_buffer, device_serial_format, "0");
        if (error < 0) {
            LERROR("FAILED to format device serial err:[%d]", error);
            free(device_serial_buffer);
            device_serial_buffer = NULL;
            // possible that device is running out of RAM or HEAP space
            return NULL;
        } else {
            LINFO("[JSON] serial: [%s]", device_serial_buffer);
        }
    }

    char *device_model_format = "\"DI2\" : \"%s\"";
    char *device_model_buffer = NULL;
    error = get_device_model(&command_output, &size);
    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("get_device_model failed errno: %d (%s)", errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
        if (strlen(command_output) > 0) {
            LDEBUG("device_model:[%s]", command_output);
            strcpy(model_id, command_output);

            error = asprintf(&device_model_buffer, device_model_format, command_output);
            if (error < 0) {
                LERROR("FAILED to format device model err:[%d]", error);
                free(device_model_buffer);
                device_model_buffer = NULL;
            } else {
                LINFO("[JSON] model: [%s]", device_model_buffer);
            }
        } else {
            LERROR("FAILED to fetch device model [blank]");
        }
    } else {
        LERROR("FAILED to fetch device model  [null]");
    }

    free(command_output);
    command_output = NULL;

    // if model cannot be fetched use 0 as model
    if (!device_model_buffer) {
        strcpy(model_id, "0");
        error = asprintf(&device_model_buffer, device_model_format, "0");
        if (error < 0) {
            LERROR("FAILED to format device model err:[%d]", error);
            free(device_model_buffer);
            device_model_buffer = NULL;
            // possible that device is running out of RAM or HEAP space
            return NULL;
        } else {
            LINFO("[JSON] model: [%s]", device_model_buffer);
        }
    }

    char *device_firmware_version_format = "\"DI3\" : \"%s\"";
    char *device_firmware_version_buffer = NULL;
    error = get_device_firmware_version(&command_output, &size);
    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("get_device_firmware_version failed errno: %d (%s)", errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
        if (strlen(command_output) > 0) {
            LDEBUG("device_firmware_version:[%s]", command_output);

            error = asprintf(&device_firmware_version_buffer, device_firmware_version_format, command_output);
            if (error < 0) {
                LERROR("FAILED to format device firmware version err:[%d]", error);
                free(device_firmware_version_buffer);
                device_firmware_version_buffer = NULL;
            } else {
                LINFO("[JSON] firmware version: [%s]", device_firmware_version_buffer);
            }
        } else {
            LERROR("FAILED to fetch device firmware version [blank]");
        }
    } else {
        LERROR("FAILED to fetch device firmware version  [null]");
    }

    free(command_output);
    command_output = NULL;

    char *device_mac_address_format = "\"DI4\" : \"%s\"";
    char *device_mac_address_buffer = NULL;
    error = get_device_mac_address(&command_output, &size);
    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("get_device_mac_address failed errno: %d (%s)", errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
        if (strlen(command_output) > 0) {
            LDEBUG("device_mac_address:[%s]", command_output);

            error = asprintf(&device_mac_address_buffer, device_mac_address_format, command_output);
            if (error < 0) {
                LERROR("FAILED to format device mac_address err:[%d]", error);
                free(device_mac_address_buffer);
                device_mac_address_buffer = NULL;
            } else {
                LINFO("[JSON] mac address: [%s]", device_mac_address_buffer);
            }
        } else {
            LERROR("FAILED to fetch device mac adderss [blank]");
        }
    } else {
        LERROR("FAILED to fetch device mac address  [null]");
    }

    free(command_output);
    command_output = NULL;

    char *device_mesh_mode_format = "\"DI5\" : \"%s\"";
    char *device_mesh_mode_buffer = NULL;
    error = get_device_mesh_mode(&command_output, &size);
    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("get_device_mesh_mode failed errno: %d (%s)", errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
	/*TWB EAP: Read DeviceRole here and save it for classifying rest items */
	memset(device_role, 0, sizeof(device_role));
	strcpy(device_role, command_output);
	LINFO("Device Role: [%s]", device_role);
	/*END*/
        if (strlen(command_output) > 0) {
            LDEBUG("device_mesh_mode:[%s]", command_output);

            error = asprintf(&device_mesh_mode_buffer, device_mesh_mode_format, command_output);
            if (error < 0) {
                LERROR("FAILED to format device mesh_mode err:[%d]", error);
                free(device_mesh_mode_buffer);
                device_mesh_mode_buffer = NULL;
            } else {
                LINFO("[JSON] mesh mode: [%s]", device_mesh_mode_buffer);
            }
        } else {
            LERROR("FAILED to fetch device mesh mode [blank]");
        }
    } else {
        LERROR("FAILED to fetch device mesh mode  [null]");
    }

    free(command_output);
    command_output = NULL;

    /*TWB EAP*/
    /*D16 Parent MAC (RE exclusive item)*/
    char *pmac_type_format = "\"DI6\" : %s";
    char *pmac_type_buffer = NULL;
    if (!strcmp(device_role, "RE"))
    {
	char pmac[32];
	memset(pmac, 0, sizeof(pmac));
	error = get_mesh_parent_mac(&pmac, 0); //zero indicates itself
        if (error != 0) {
            if (!pmac) {
                errno = error;
                LERROR("get_device_mesh_parent failed errno: %d (%s)", errno, strerror(errno));
            }
        }

        if (pmac) {
            strip_newline_chars(pmac);
            if (strlen(pmac) > 0) {
                LDEBUG("device_parent_mac_address:[%s]", pmac);

                error = asprintf(&pmac_type_buffer, pmac_type_format, pmac);
                if (error < 0) {
                    LERROR("FAILED to format parent mac address  err:[%d]", error);
                    free(pmac_type_buffer);
                    pmac_type_buffer = NULL;
                } else {
                    LINFO("[JSON] parent mac address: [%s]", pmac_type_buffer);
                }
            } else {
                LERROR("FAILED to fetch device mac adderss [blank]");
                }
        } else {
            LERROR("FAILED to fetch device mac address  [null]");
          }

    }
    /*END D16*/

    char *bkhl_type_format = "\"DI7\" : %ld";
    char *bkhl_type_buffer = NULL;
    /*TWB EAP: D17 (RE exclusive item)*/
    if (!strcmp(device_role, "RE"))
    {
        long btype = get_device_bkhl_type();
        LDEBUG("bkhl type:[%ld]", btype);
        error = asprintf(&bkhl_type_buffer, bkhl_type_format, btype);
        if (error < 0) {
            LERROR("FAILED to format CPU err:[%d]", error);
            free(bkhl_type_buffer);
            bkhl_type_buffer = NULL;
        } else {
            LINFO("[JSON] bkhl_type_buffer : [%s]", bkhl_type_buffer);
        }
    }

    char *datetime_format = "\"DT\" : \"%02d-%02d-%04d %02d:%02d:%02d\"";
    char *datetime_buffer = NULL;
    time_t now_time = time(NULL);
    struct tm now_lt = *localtime(&now_time);
    unsigned long long datetime_value = (unsigned long long) now_time;
    LDEBUG("datetime epoch:[%llu]", datetime_value);
    LDEBUG("datetime:[%02d-%02d-%04d %02d:%02d:%02d]",
           now_lt.tm_mday, now_lt.tm_mon + 1, now_lt.tm_year + 1900, now_lt.tm_hour, now_lt.tm_min, now_lt.tm_sec);
    error = asprintf(&datetime_buffer, datetime_format,
                     now_lt.tm_mday, now_lt.tm_mon + 1, now_lt.tm_year + 1900, now_lt.tm_hour, now_lt.tm_min,
                     now_lt.tm_sec);
    if (error < 0) {
        LERROR("FAILED to format timestamp err:[%d]", error);
        free(datetime_buffer);
        datetime_buffer = NULL;
    } else {
        LINFO("[JSON] datetime: [%s]", datetime_buffer);
    }

    char *cpu_format = "\"SI1\" : %ld";
    char *cpu_buffer = NULL;
    long cpu = get_cpu_usage();
    LDEBUG("CPU:[%ld]", cpu);
    error = asprintf(&cpu_buffer, cpu_format, cpu);
    if (error < 0) {
        LERROR("FAILED to format CPU err:[%d]", error);
        free(cpu_buffer);
        cpu_buffer = NULL;
    } else {
        LINFO("[JSON] cpu: [%s]", cpu_buffer);
    }

    char *memory_format = "\"SI2\" : %ld";
    char *memory_buffer = NULL;
    long memory = get_memory_usage();
    LDEBUG("memory:[%ld]", memory);
    error = asprintf(&memory_buffer, memory_format, memory);
    if (error < 0) {
        LERROR("FAILED to format memory err:[%d]", error);
        free(memory_buffer);
        memory_buffer = NULL;
    } else {
        LINFO("[JSON] memory: [%s]", memory_buffer);
    }

    char *uptime_format = "\"SI3\" : %ld";
    char *uptime_buffer = NULL;
    long uptime = get_uptime();
    LDEBUG("uptime:[%ld]", uptime);
    error = asprintf(&uptime_buffer, uptime_format, uptime);
    if (error < 0) {
        LERROR("FAILED to format uptime err:[%d]", error);
        free(uptime_buffer);
        uptime_buffer = NULL;
    } else {
        LINFO("[JSON] uptime: [%s]", uptime_buffer);
    }

    /*TWB EAP: LI1 (RE exclusive item and non ethernet backhaul)*/
    char *lan_client_type_format = "\"LI1\" : %s";
    char *lan_client_type_buffer = NULL;

    if (!strcmp(device_role, "RE") && ((int)get_device_bkhl_type()==0))
    {
        char lan_mac[32];
        memset(lan_mac, 0, sizeof(lan_mac));
        error = get_mesh_lan_client(&lan_mac, 0); //zero indicates itself
        if (error != 0) {
            if (!lan_mac) {
                errno = error;
                LERROR("get_mesh_lan_client failed errno: %d (%s)", errno, strerror(errno));
            }
        }

        if (lan_mac) {
            strip_newline_chars(lan_mac);
            if (strlen(lan_mac) > 0) {
                LDEBUG("LAN client mac address:[%s]", lan_mac);

                error = asprintf(&lan_client_type_buffer, lan_client_type_format, lan_mac);
                if (error < 0) {
                    LERROR("FAILED to format LAN client MAC address  err:[%d]", error);
                    free(lan_client_type_buffer);
                    lan_client_type_buffer = NULL;
                } else {
                    LINFO("[JSON] LAN client MAC address: [%s]", lan_client_type_buffer);
                }
            } else {
                LERROR("FAILED to fetch LAN client MAC address [blank]");
                }
        } else {
            LERROR("FAILED to fetch LAN client MAC address  [null]");
          }

    }
    /*END LI1*/

    backup_stats_file();

    ifstats_t *stats;
    // Ethernet Interface
    char *stats_lan_format = "\"LI3\" : %llu, \"LI2\" : %llu, \"LI5\" : %llu, \"LI4\" : %llu, \"LI7\" : %llu, \"LI6\" : %llu, \"LI9\" : %llu, \"LI8\" : %llu";
    char *stats_lan_buffer = NULL;
    stats = get_stats_LAN();
    if (!stats) {
        LERROR("FAILED to get_stats_LAN [NULL]");
    } else {
        ifstats_t *current = stats;
        LDEBUG("Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
               current->device, current->rxBytes, current->rxPackets, current->rxErrors,
               current->rxDrop, current->txBytes, current->txPackets, current->txErrors, current->txDrop);

        error = asprintf(&stats_lan_buffer, stats_lan_format, current->rxBytes, current->txBytes, current->rxPackets,
                         current->txPackets, current->rxErrors, current->txErrors, current->rxDrop, current->txDrop);
        if (error < 0) {
            LERROR("FAILED to format stats_lan err:[%d]", error);
            free(stats_lan_buffer);
            stats_lan_buffer = NULL;
        } else {
            LINFO("[JSON] stats_lan: [%s]", stats_lan_buffer);
        }
    }
    release_ifstats(stats);
    stats = NULL;

    // 2.4GHz Fronthaul interface
    char *stats_fronthaul_wifi24_format = "\"WI5\" : %llu, \"WI4\" : %llu, \"WI7\" : %llu, \"WI6\" : %llu, \"WI9\" : %llu, \"WI8\" : %llu";
    char *stats_fronthaul_wifi24_buffer = NULL;
    stats = get_stats_fronthaul_WIFI24();
    if (!stats) {
        LWARNING("FAILED to get_stats_fronthaul_WIFI24 [NULL]");
    } else {
        ifstats_t *current = stats;
        LDEBUG("Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
               current->device, current->rxBytes, current->rxPackets, current->rxErrors,
               current->rxDrop, current->txBytes, current->txPackets, current->txErrors, current->txDrop);

        error = asprintf(&stats_fronthaul_wifi24_buffer, stats_fronthaul_wifi24_format, current->rxBytes, current->txBytes,
                         current->rxPackets, current->txPackets, current->rxErrors, current->txErrors);
        if (error < 0) {
            LERROR("FAILED to format stats_fronthaul_wifi24 err:[%d]", error);
            free(stats_fronthaul_wifi24_buffer);
            stats_fronthaul_wifi24_buffer = NULL;
        } else {
            LINFO("[JSON] stats_fronthaul_wifi24: [%s]", stats_fronthaul_wifi24_buffer);
        }
    }
    release_ifstats(stats);
    stats = NULL;

    // 2.4GHz backhaul interface
    char *stats_backhaul_wifi24_format = "\"BKI16\" : %llu, \"BKI15\" : %llu, \"BKI18\" : %llu, \"BKI17\" : %llu, \"BKI20\" : %llu, \"BKI19\" : %llu";
    char *stats_backhaul_wifi24_buffer = NULL;
    if (!strcmp(device_role, "RE"))  /*TWB EAP: RE exclusive items*/
    {
        stats = get_stats_backhaul_WIFI24();
        if (!stats) {
            LWARNING("FAILED to get_stats_backhaul_WIFI24 [NULL]");
        } else {
            ifstats_t *current = stats;
            LDEBUG("Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
               current->device, current->rxBytes, current->rxPackets, current->rxErrors,
               current->rxDrop, current->txBytes, current->txPackets, current->txErrors, current->txDrop);

            error = asprintf(&stats_backhaul_wifi24_buffer, stats_backhaul_wifi24_format, current->rxBytes, current->txBytes,
                         current->rxPackets, current->txPackets, current->rxErrors, current->txErrors);
            if (error < 0) {
                LERROR("FAILED to format stats_backhaul_wifi24 err:[%d]", error);
                free(stats_backhaul_wifi24_buffer);
                stats_backhaul_wifi24_buffer = NULL;
            } else {
                LINFO("[JSON] stats_backhaul_wifi24: [%s]", stats_backhaul_wifi24_buffer);
            }
        }
        release_ifstats(stats);
        stats = NULL;
    }

    // 5GHz Fronthaul interface
    char *stats_fronthaul_wifi50_format = "\"WI14\" : %llu, \"WI13\" : %llu, \"WI16\" : %llu, \"WI15\" : %llu, \"WI18\" : %llu, \"WI17\" : %llu";
    char *stats_fronthaul_wifi50_buffer = NULL;
    stats = get_stats_fronthaul_WIFI50();
    if (!stats) {
        LWARNING("FAILED to get_stats_fronthaul_WIFI50 [NULL]");
    } else {
        ifstats_t *current = stats;
        LDEBUG("Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
               current->device, current->rxBytes, current->rxPackets, current->rxErrors,
               current->rxDrop, current->txBytes, current->txPackets, current->txErrors, current->txDrop);

        error = asprintf(&stats_fronthaul_wifi50_buffer, stats_fronthaul_wifi50_format, current->rxBytes, current->txBytes,
                         current->rxPackets, current->txPackets, current->rxErrors, current->txErrors);
        if (error < 0) {
            LERROR("FAILED to format stats_fronthaul_wifi50 err:[%d]", error);
            free(stats_fronthaul_wifi50_buffer);
            stats_fronthaul_wifi50_buffer = NULL;
        } else {
            LINFO("[JSON] stats_fronthaul_wifi50: [%s]", stats_fronthaul_wifi50_buffer);
        }
    }
    release_ifstats(stats);
    stats = NULL;

    // 5GHz backhaul interface
    char *stats_backhaul_wifi50_format = "\"BKI6\" : %llu, \"BKI5\" : %llu, \"BKI8\" : %llu, \"BKI7\" : %llu, \"BKI10\" : %llu, \"BKI9\" : %llu";
    char *stats_backhaul_wifi50_buffer = NULL;
    if (!strcmp(device_role, "RE")) /*TWB EAP: RE exclusive items*/
    {
        stats = get_stats_backhaul_WIFI50();
        if (!stats) {
            LWARNING("FAILED to get_stats_backhaul_WIFI50 [NULL]");
        } else {
            ifstats_t *current = stats;
            LDEBUG("Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
                   current->device, current->rxBytes, current->rxPackets, current->rxErrors,
                   current->rxDrop, current->txBytes, current->txPackets, current->txErrors, current->txDrop);

            error = asprintf(&stats_backhaul_wifi50_buffer, stats_backhaul_wifi50_format, current->rxBytes, current->txBytes,
                             current->rxPackets, current->txPackets, current->rxErrors, current->txErrors);
            if (error < 0) {
                LERROR("FAILED to format stats_backhaul_wifi50 err:[%d]", error);
                free(stats_backhaul_wifi50_buffer);
                stats_backhaul_wifi50_buffer = NULL;
            } else {
                LINFO("[JSON] stats_backhaul_wifi50: [%s]", stats_backhaul_wifi50_buffer);
            }
        }
        release_ifstats(stats);
        stats = NULL;
    }

    /*TWB EAP: BKI1 (RE exclusive item)*/
    char *bh_rssi_wifi50_format = "\"BKI1\" : %ld";
    char *bh_rssi_wifi50_buffer = NULL;
    int bh50_rssi = 0;

    if (!strcmp(device_role, "RE"))
    {
	char *if_re_bh = "ath11";
        bh50_rssi = get_re_bh_rssi(if_re_bh);

        LDEBUG("RE 5G backhaul RSSI:[%d]", bh50_rssi);
        error = asprintf(&bh_rssi_wifi50_buffer, bh_rssi_wifi50_format, bh50_rssi);
        if (error < 0) {
            LERROR("FAILED to format RE 5G backhaul RSSI err:[%d]", error);
            free(bh_rssi_wifi50_buffer);
            bh_rssi_wifi50_buffer = NULL;
        } else {
            LINFO("[JSON] bh_rssi_wifi50_buffer : [%s]", bh_rssi_wifi50_buffer);
        }
    }

    /*TWB EAP: BKI11 (RE exclusive item)*/
    char *bh_rssi_wifi24_format = "\"BKI11\" : %ld";
    char *bh_rssi_wifi24_buffer = NULL;
    int bh24_rssi = 0;

    if (!strcmp(device_role, "RE"))
    {
        char *if_re_bh = "ath01";
        bh24_rssi = get_re_bh_rssi(if_re_bh);

        LDEBUG("RE 2.4G backhaul RSSI:[%d]", bh24_rssi);
        error = asprintf(&bh_rssi_wifi24_buffer, bh_rssi_wifi24_format, bh24_rssi);
        if (error < 0) {
            LERROR("FAILED to format RE 2.4G backhaul RSSI err:[%d]", error);
            free(bh_rssi_wifi24_buffer);
            bh_rssi_wifi24_buffer = NULL;
        } else {
            LINFO("[JSON] bh_rssi_wifi24_buffer : [%s]", bh_rssi_wifi24_buffer);
        }
    }


    /*TWB EAP: BKI2 (RE exclusive item)*/
    char *bh_mcs_wifi50_format = "\"BKI2\" : %ld";
    char *bh_mcs_wifi50_buffer = NULL;
    int bh50_mcs = 0;

    if (!strcmp(device_role, "RE"))
    {
        char *if_re_bh = "ath11";
        bh50_mcs = get_5g_re_bh_mcs(if_re_bh);

        LDEBUG("RE backhaul MCS:[%d]", bh50_mcs);
        error = asprintf(&bh_mcs_wifi50_buffer, bh_mcs_wifi50_format, bh50_mcs);
        if (error < 0) {
            LERROR("FAILED to format RE backhaul MCS err:[%d]", error);
            free(bh_mcs_wifi50_buffer);
            bh_mcs_wifi50_buffer = NULL;
        } else {
            LINFO("[JSON] bh_mcs_wifi50_buffer : [%s]", bh_mcs_wifi50_buffer);
        }
    }

    /*TWB EAP: BKI12 (RE exclusive item)*/
    char *bh_mcs_wifi24_format = "\"BKI12\" : %ld";
    char *bh_mcs_wifi24_buffer = NULL;
    int bh24_mcs = 0;

    if (!strcmp(device_role, "RE"))
    {
        char *if_re_bh = "ath01";
        bh24_mcs = get_24g_re_bh_mcs(if_re_bh);

        LDEBUG("RE 2.4G backhaul MCS:[%d]", bh24_mcs);
        error = asprintf(&bh_mcs_wifi24_buffer, bh_mcs_wifi24_format, bh24_mcs);
        if (error < 0) {
            LERROR("FAILED to format RE 2.4G backhaul MCS err:[%d]", error);
            free(bh_mcs_wifi24_buffer);
            bh_mcs_wifi24_buffer = NULL;
        } else {
            LINFO("[JSON] bh_mcs_wifi24_buffer : [%s]", bh_mcs_wifi24_buffer);
        }
    }


    /*TWB EAP: BKI3 (RE exclusive item)*/
    char *bh_chan_wifi50_format = "\"BKI3\" : %ld";
    char *bh_chan_wifi50_buffer = NULL;
    int bh50_chan = 0;

    if (!strcmp(device_role, "RE"))
    {
        char *if_re_bh = "ath11";
        bh50_chan = get_80211_channel(if_re_bh);

        LDEBUG("RE backhaul channel:[%d]", bh50_chan);
        error = asprintf(&bh_chan_wifi50_buffer, bh_chan_wifi50_format, bh50_chan);
        if (error < 0) {
            LERROR("FAILED to format RE backhaul channel err:[%d]", error);
            free(bh_chan_wifi50_buffer);
            bh_chan_wifi50_buffer = NULL;
        } else {
            LINFO("[JSON] bh_chan_wifi50_buffer : [%s]", bh_chan_wifi50_buffer);
        }
    }

    /*TWB EAP: BKI13 (RE exclusive item)*/
    char *bh_chan_wifi24_format = "\"BKI13\" : %ld";
    char *bh_chan_wifi24_buffer = NULL;
    int bh24_chan = 0;

    if (!strcmp(device_role, "RE"))
    {
        char *if_re_bh = "ath01";
        bh24_chan = get_80211_channel(if_re_bh);

        LDEBUG("RE 2.4G backhaul channel:[%d]", bh24_chan);
        error = asprintf(&bh_chan_wifi24_buffer, bh_chan_wifi24_format, bh24_chan);
        if (error < 0) {
            LERROR("FAILED to format RE 2.4G backhaul channel err:[%d]", error);
            free(bh_chan_wifi24_buffer);
            bh_chan_wifi24_buffer = NULL;
        } else {
            LINFO("[JSON] bh_chan_wifi24_buffer : [%s]", bh_chan_wifi24_buffer);
        }
    }

    /*TWB EAP: BKI4 (RE exclusive item)*/
    char *bh_chan_ut_wifi50_format = "\"BKI4\" : %ld";
    char *bh_chan_ut_wifi50_buffer = NULL;
    int bh50_chan_ut = 0;

    if (!strcmp(device_role, "RE"))
    {
        //char *if_re_bh = "ath11";
        bh50_chan_ut = get_channel_utilization(bh50_chan);

        LDEBUG("RE backhaul channel utilization:[%d]", bh50_chan_ut);
        error = asprintf(&bh_chan_ut_wifi50_buffer, bh_chan_ut_wifi50_format, bh50_chan_ut);
        if (error < 0) {
            LERROR("FAILED to format RE backhaul channel utilization err:[%d]", error);
            free(bh_chan_ut_wifi50_buffer);
            bh_chan_ut_wifi50_buffer = NULL;
        } else {
            LINFO("[JSON] bh_chan_ut_wifi50_buffer : [%s]", bh_chan_ut_wifi50_buffer);
        }
    }
    /*END BKI4*/

    /*TWB EAP: BKI14 (RE exclusive item)*/
    char *bh_chan_ut_wifi24_format = "\"BKI14\" : %ld";
    char *bh_chan_ut_wifi24_buffer = NULL;
    int bh24_chan_ut = 0;

    if (!strcmp(device_role, "RE"))
    {
        //char *if_re_bh = "ath11";
        bh24_chan_ut = get_channel_utilization(bh24_chan);

        LDEBUG("RE 2.4G backhaul channel utilization:[%d]", bh24_chan_ut);
        error = asprintf(&bh_chan_ut_wifi24_buffer, bh_chan_ut_wifi24_format, bh24_chan_ut);
        if (error < 0) {
            LERROR("FAILED to format RE 2.4G backhaul channel utilization err:[%d]", error);
            free(bh_chan_ut_wifi24_buffer);
            bh_chan_ut_wifi24_buffer = NULL;
        } else {
            LINFO("[JSON] bh_chan_ut_wifi24_buffer : [%s]", bh_chan_ut_wifi24_buffer);
        }
    }
    /*END BKI14*/

    char *clients_count_WIFI24_format = "\"WI1\" : %ld";
    char *clients_count_WIFI24_buffer = NULL;
    if (stats_fronthaul_wifi24_buffer) {
        long wlan_clients_count_WIFI24 = get_wlan_clients_count_WIFI24();
        LDEBUG("wlan_clients_count_WIFI24:[%ld]", wlan_clients_count_WIFI24);
        error = asprintf(&clients_count_WIFI24_buffer, clients_count_WIFI24_format, wlan_clients_count_WIFI24);
        if (error < 0) {
            LERROR("FAILED to format clients WIFI24 err:[%d]", error);
            free(clients_count_WIFI24_buffer);
            clients_count_WIFI24_buffer = NULL;
        } else {
            LINFO("[JSON] clients_WIFI24: [%s]", clients_count_WIFI24_buffer);
        }
    }

    /*TWB EAP: WI2*/
    char *fh_chan_wifi24_format = "\"WI2\" : %ld";
    char *fh_chan_wifi24_buffer = NULL;
    int fchan24 = 0;

    char *ifname24 = "ath0";
    fchan24 = get_80211_channel(ifname24);

    LDEBUG("Fronthaul channel:[%d]", fchan24);
    error = asprintf(&fh_chan_wifi24_buffer, fh_chan_wifi24_format, fchan24);
    if (error < 0) {
        LERROR("FAILED to format fronthaul channel err:[%d]", error);
        free(fh_chan_wifi24_buffer);
        fh_chan_wifi24_buffer = NULL;
    } else {
        LINFO("[JSON] fh_chan_wifi24_buffer : [%s]", fh_chan_wifi24_buffer);
      }
    /*END WI2*/

    /*TWB EAP: WI3*/
    char *fh_chan_ut_wifi24_format = "\"WI3\" : %ld";
    char *fh_chan_ut_wifi24_buffer = NULL;
    int fchan24_ut = 0;

    fchan24_ut = get_channel_utilization(fchan24);

    LDEBUG("Fronthaul channel utilization:[%d]", fchan24_ut);
    error = asprintf(&fh_chan_ut_wifi24_buffer, fh_chan_ut_wifi24_format, fchan24_ut);
    if (error < 0) {
        LERROR("FAILED to format fronthaul channel utilization err:[%d]", error);
        free(fh_chan_ut_wifi24_buffer);
        fh_chan_ut_wifi24_buffer = NULL;
    } else {
        LINFO("[JSON] fh_chan_ut_wifi24_buffer : [%s]", fh_chan_ut_wifi24_buffer);
      }
    /*END WI3*/


    /*TWB EAP: WI11*/
    char *fh_chan_wifi50_format = "\"WI11\" : %ld";
    char *fh_chan_wifi50_buffer = NULL;
    int fchan50 = 0;

    char *ifname50 = "ath1";
    fchan50 = get_80211_channel(ifname50);

    LDEBUG("Fronthaul channel:[%d]", fchan50);
    error = asprintf(&fh_chan_wifi50_buffer, fh_chan_wifi50_format, fchan50);
    if (error < 0) {
        LERROR("FAILED to format fronthaul channel err:[%d]", error);
        free(fh_chan_wifi50_buffer);
        fh_chan_wifi50_buffer = NULL;
    } else {
        LINFO("[JSON] fh_chan_wifi50_buffer : [%s]", fh_chan_wifi50_buffer);
      }
    /*END WI11*/

    /*TWB EAP: WI12*/
    char *fh_chan_ut_wifi50_format = "\"WI12\" : %ld";
    char *fh_chan_ut_wifi50_buffer = NULL;
    int fchan50_ut = 0;

    fchan50_ut = get_channel_utilization(fchan50);

    LDEBUG("Fronthaul channel utilization:[%d]", fchan50_ut);
    error = asprintf(&fh_chan_ut_wifi50_buffer, fh_chan_ut_wifi50_format, fchan50_ut);
    if (error < 0) {
        LERROR("FAILED to format fronthaul channel utilization err:[%d]", error);
        free(fh_chan_ut_wifi50_buffer);
        fh_chan_ut_wifi50_buffer = NULL;
    } else {
        LINFO("[JSON] fh_chan_ut_wifi50_buffer : [%s]", fh_chan_ut_wifi50_buffer);
      }
    /*END WI12*/




    
    //char *clients_count_WIFI50_format = "\"WI9\" : %ld"; Jio error??
    char *clients_count_WIFI50_format = "\"WI10\" : %ld";
    char *clients_count_WIFI50_buffer = NULL;
    if (stats_fronthaul_wifi50_buffer) {
        long wlan_clients_count_WIFI50 = get_wlan_clients_count_WIFI50();
        LDEBUG("wlan_clients_count_WIFI50:[%ld]", wlan_clients_count_WIFI50);
        error = asprintf(&clients_count_WIFI50_buffer, clients_count_WIFI50_format, wlan_clients_count_WIFI50);
        if (error < 0) {
            LERROR("FAILED to format clients WIFI50 err:[%d]", error);
            free(clients_count_WIFI50_buffer);
            clients_count_WIFI50_buffer = NULL;
        } else {
            LINFO("[JSON] clients_WIFI50: [%s]", clients_count_WIFI50_buffer);
        }
    }

    char *stats_rssi24_format = "{ \"mac\" : \"%s\", \"rssi\" : %d , \"txrate\" : %d, \"maxtxrate\" : %d}";
    char *stats_rssi24_buffer = NULL;
    if (stats_fronthaul_wifi24_buffer) {
        char *wlan_interface = "ath0";
        client_info_t *rssi24_stats = get_client_info(wlan_interface);
        if (!rssi24_stats) {
            LWARNING("FAILED to get_client_rssi for %s [NULL]", wlan_interface);
        } else {
            for (client_info_t *current = rssi24_stats; current != NULL; current = current->next) {

                char *stats_rssi24_buffer_local = NULL;
                stats_rssi24_buffer_local = malloc(
                        sizeof(char) * (strlen(stats_rssi24_format) + strlen(current->device) + (30 * 1) + 1));
                memset(stats_rssi24_buffer_local, 0,
                       sizeof(char) * (strlen(stats_rssi24_format) + strlen(current->device) + (30 * 1) + 1));
                sprintf(stats_rssi24_buffer_local, stats_rssi24_format, current->device, current->rssi_value, current->txrate_value, current->maxtxrate_value);
                LDEBUG("[JSON] stats_rssi24_client: [%s]", stats_rssi24_buffer_local);

                if (!stats_rssi24_buffer) {
                    char *rssi_json_header = "\"WI19\" : [ ";
                    stats_rssi24_buffer = malloc(
                            sizeof(char) * (strlen(stats_rssi24_buffer_local) + strlen(rssi_json_header) + 1));
                    memset(stats_rssi24_buffer, 0,
                           sizeof(char) * (strlen(stats_rssi24_buffer_local) + strlen(rssi_json_header) + 1));
                    strcat(stats_rssi24_buffer, rssi_json_header);
                } else {
                    char *rssi_json_seperator = ", ";
                    stats_rssi24_buffer = realloc(stats_rssi24_buffer,
                                                  sizeof(char) * (strlen(stats_rssi24_buffer_local) +
                                                                  strlen(stats_rssi24_buffer) +
                                                                  strlen(rssi_json_seperator) + 1));
                    strcat(stats_rssi24_buffer, rssi_json_seperator);
                }
                strcat(stats_rssi24_buffer, stats_rssi24_buffer_local);
                free(stats_rssi24_buffer_local);
                //LINFO("[JSON] stats_rssi24_buffer: [%s]", stats_rssi24_buffer);
            }
            char *rssi_json_tail = " ]";
            stats_rssi24_buffer = realloc(stats_rssi24_buffer,
                                          sizeof(char) * (strlen(stats_rssi24_buffer) + strlen(rssi_json_tail) + 1));
            strcat(stats_rssi24_buffer, rssi_json_tail);
            LINFO("[JSON] stats_rssi24: [%s]", stats_rssi24_buffer);

            release_info(rssi24_stats);
            rssi24_stats = NULL;
        }
    }

    char *stats_rssi50_format = "{ \"mac\" : \"%s\", \"rssi\" : %d, \"txrate\" : %d, \"maxtxrate\" : %d }";
    char *stats_rssi50_buffer = NULL;
    if (stats_fronthaul_wifi50_buffer) {
        char *wlan_interface = "ath1";
        client_info_t *rssi50_stats = get_client_info(wlan_interface);
        if (!rssi50_stats) {
            LWARNING("FAILED to get_client_info for %s [NULL]", wlan_interface);
        } else {
            for (client_info_t *current = rssi50_stats; current != NULL; current = current->next) {

                char *stats_rssi50_buffer_local = NULL;
                stats_rssi50_buffer_local = malloc(
                        sizeof(char) * (strlen(stats_rssi50_format) + strlen(current->device) + (30 * 1) + 1));
                memset(stats_rssi50_buffer_local, 0,
                       sizeof(char) * (strlen(stats_rssi50_format) + strlen(current->device) + (30 * 1) + 1));
                sprintf(stats_rssi50_buffer_local, stats_rssi50_format, current->device, current->rssi_value, current->txrate_value ,current->maxtxrate_value);
                LDEBUG("[JSON] stats_rssi50_client: [%s]", stats_rssi50_buffer_local);

                if (!stats_rssi50_buffer) {
                    char *rssi_json_header = "\"WI20\" : [ ";
                    stats_rssi50_buffer = malloc(
                            sizeof(char) * (strlen(stats_rssi50_buffer_local) + strlen(rssi_json_header) + 1));
                    memset(stats_rssi50_buffer, 0,
                           sizeof(char) * (strlen(stats_rssi50_buffer_local) + strlen(rssi_json_header) + 1));
                    strcat(stats_rssi50_buffer, rssi_json_header);
                } else {
                    char *rssi_json_seperator = ", ";
                    stats_rssi50_buffer = realloc(stats_rssi50_buffer,
                                                  sizeof(char) * (strlen(stats_rssi50_buffer_local) +
                                                                  strlen(stats_rssi50_buffer) +
                                                                  strlen(rssi_json_seperator) + 1));
                    strcat(stats_rssi50_buffer, rssi_json_seperator);
                }
                strcat(stats_rssi50_buffer, stats_rssi50_buffer_local);
                free(stats_rssi50_buffer_local);
                //LINFO("[JSON] stats_rssi50_buffer: [%s]", stats_rssi50_buffer);
            }
            char *rssi_json_tail = " ]";
            stats_rssi50_buffer = realloc(stats_rssi50_buffer,
                                          sizeof(char) * (strlen(stats_rssi50_buffer) + strlen(rssi_json_tail) + 1));
            strcat(stats_rssi50_buffer, rssi_json_tail);
            LINFO("[JSON] stats_rssi50: [%s]", stats_rssi50_buffer);

            release_info(rssi50_stats);
            rssi50_stats = NULL;
        }
    }

    char *dc_data_head = "[{ ";
    char *dc_data_tail = " }]";
    char *dc_data_separator = ", ";
    size_t dc_data_separator_length = strlen(dc_data_separator);
    char *dc_data_buffer = NULL;
    char *perodic_tag = NULL;
    asprintf(&perodic_tag, "\"Name\" : \"DE1\"");
    char *dc_elements[] = {perodic_tag, device_serial_buffer, device_model_buffer,
	                   device_firmware_version_buffer, device_mac_address_buffer, 
			   device_mesh_mode_buffer, pmac_type_buffer, bkhl_type_buffer,
                           datetime_buffer, cpu_buffer, memory_buffer, uptime_buffer,
                           clients_count_WIFI24_buffer, clients_count_WIFI50_buffer,
			   lan_client_type_buffer, stats_lan_buffer, fh_chan_wifi24_buffer,
			   fh_chan_ut_wifi24_buffer, fh_chan_wifi50_buffer, fh_chan_ut_wifi50_buffer,
			   bh_rssi_wifi50_buffer, bh_rssi_wifi24_buffer, bh_mcs_wifi50_buffer,
			   bh_mcs_wifi24_buffer, bh_chan_wifi50_buffer, bh_chan_wifi24_buffer,
			   bh_chan_ut_wifi50_buffer, bh_chan_ut_wifi24_buffer,
                           stats_backhaul_wifi24_buffer, stats_fronthaul_wifi24_buffer, 
                           stats_backhaul_wifi50_buffer, stats_fronthaul_wifi50_buffer,
                           stats_rssi24_buffer, stats_rssi50_buffer};
	    /*
                           channel_WIFI24_buffer, channel_WIFI50_buffer,
                           clients_count_LAN_buffer, clients_count_IPV6_buffer,
                           stats_wan_buffer, stats_lan_buffer,
                           stats_wifi24_buffer, stats_wifi50_buffer,
                           stats_rssi24_buffer, stats_rssi50_buffer};
	     */

    size_t buffer_len = 0;
    int dc_elements_count = (int) (sizeof(dc_elements) / sizeof(dc_elements[0]));
    LDEBUG("number of dc_elements: [%d]", dc_elements_count);
    for (int el = 0; el < dc_elements_count; el++) {
        if (dc_elements[el]) {
            int element_length = (int) (strlen(dc_elements[el]));
            if (element_length > 0) {
                buffer_len = buffer_len + element_length + dc_data_separator_length;
                //LINFO("dc_elements[%d]: [%d] [%s]", el, element_length , dc_elements[el]);
            } else {
                LDEBUG("dc_elements[%d] is BLANK: [%d] [%s]", el, element_length, dc_elements[el]);
            }
        } else {
            LDEBUG("dc_elements[%d] is NULL", el);
        }
    }

    buffer_len = buffer_len + strlen(dc_data_head) + strlen(dc_data_tail) + dc_data_separator_length + 1;
    //LINFO("buffer_len: [%ld]", buffer_len);

    dc_data_buffer = malloc(sizeof(char) * buffer_len);
    memset(dc_data_buffer, 0, sizeof(char) * buffer_len);

    strcat(dc_data_buffer, dc_data_head);
    for (int el = 0; el < dc_elements_count; el++) {
        if (dc_elements[el]) {
            int element_length = (int) (strlen(dc_elements[el]));
            if (element_length > 0) {
                //LINFO("adding dc_elements[%d]: [%s]", el , dc_elements[el]);
                if (el > 0) {
                    strcat(dc_data_buffer, dc_data_separator);
                }
                strcat(dc_data_buffer, dc_elements[el]);
                //LINFO("dc_data_buffer: [%s]", dc_data_buffer);
            } else {
                LDEBUG("dc_elements[%d] is BLANK: [%d] [%s]", el, element_length, dc_elements[el]);
            }
        } else {
            LDEBUG("dc_elements[%d] is NULL", el);
        }
    }
    strcat(dc_data_buffer, dc_data_tail);
    //LINFO("FINAL dc_data_buffer: [%s]", dc_data_buffer);

    for (int el = 0; el < dc_elements_count; el++) {
        if (dc_elements[el]) {
            free(dc_elements[el]);
            dc_elements[el] = NULL;
        } else {
            LDEBUG("dc_elements[%d] is NULL", el);
        }
    }

    rename_stats_file();
    return dc_data_buffer;
}

