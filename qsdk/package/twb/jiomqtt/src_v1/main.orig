#include <execinfo.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jiot_client_mqtt.h>

void publish(jiot_client_MQTT_Hndl_t client, char *topic);

jiot_client_MQTT_Hndl_t mqttClient;

int doStop = 0;
unsigned long int cnt = 1;
int delayPublish = 1;
unsigned long delayTime = 10000000L;
char *main_topic = "Test_Topic";

void messageDelivered(void *context, jiot_client_MQTT_token_t token)
{
    jiot_client_MQTT_Hndl_t client = context;
    printf("Message delivery completed for message id %d\n", token);
    printf("Message delivered count %d\n", cnt);
    
    if (!doStop) {
        cnt ++;
        if (cnt > 6) {
            printf("Disconnecting client\n");
            jiot_client_MQTT_disconnect(client);
            printf("Disconnect initiated\n");
        } else {
            int conn = jiot_client_MQTT_isConnected(client);
            printf("Connected status %d\n", conn);
            
            if(delayPublish) {
                usleep(delayTime);
            }
            publish(client, main_topic);
        }
    }
}

void messageFailed(void *context, const char *cause, jiot_client_MQTT_token_t token)
{
    printf("Message delivery failed for message id %d\n with exception %s", token, cause);
}

void connectionFailed(void *context, const char *cause, int code){
    printf("Connection failed with error %d\n", code);
}

void connectionComplete(void *context, jiot_client_MQTT_token_t token){
    int pendingTokenCount = 0;
    printf("Connection completed\n");
    jiot_client_MQTT_Hndl_t client = context;
    jiot_client_MQTT_token_t *pendingTokens = NULL;

    // Check if there are any unprocessed tokens from previous connect session.
    // If there any unprocessed publish messages, those tokens shall be returned.
    // Client applications can re-trigger the unprocessed publish messages.
    // In the example app, we are simply printing the unprocessed tokens for
    // illustration purpose. Example app doesn't re-trigger any unprocessed
    // publish messages.
    int ret = jiot_client_MQTT_getPendingTokens(client, &pendingTokens);
    if(pendingTokens != NULL) {
        while(pendingTokens[pendingTokenCount] != -1) {
            printf("Pending token from previous session %d\n", pendingTokens[pendingTokenCount]);
            pendingTokenCount ++;
        }
        free(pendingTokens);
    } else {
        printf("No pending tokens from previous session\n");
    }
    if(!doStop) {
       publish(client, main_topic);
    }
}

void disconnectCompleted(void *context) {
    printf("Disconnect completed\n");
    doStop = 1;
}

void disconnectFailed(void *context, const char *message, int code) {
    printf("Disconnect failed with error code %d\n", code);
    doStop = 1;
}

void publish(jiot_client_MQTT_Hndl_t client, char *topic) {
    int retVal = JIOT_MQTT_OPERATION_FAILURE;
    char *message = "Hello World!!";
    int length = strlen(message);

    // Example application show cases the use of various Publish APIs().
    // Clients can call any variant of the Publish API() as per their need.

#if defined(PUBLISH_DEFAULT) // Publish with default QOS (Default value 1).
    printf("Default publishing...\n");
    retVal = jiot_client_MQTT_publish(client, topic, message, length);

#elif defined(PUBLISH_QOS_ZERO) // Publish with QOS 0.
    printf("Publishing with QOS 0...\n");
    retVal = jiot_client_MQTT_publishWithQos(client, topic, message, length, JIOT_QOS_NONE);

#elif defined(PUBLISH_QOS_TWO) // Publish with QOS 2.
    printf("Publishing with QOS 2...\n");
    retVal = jiot_client_MQTT_publishWithQos(client, topic, message, length, JIOT_QOS_TWO);

#else // Publish with application provided Qos and retained values.
    printf("Publishing with Qos and retained...\n");
    retVal = jiot_client_MQTT_publishWithQosAndRetained(client, topic, message, length, JIOT_QOS_DEFAULT, 0);
#endif
    
    if (retVal >= 0) {
        printf("Publish initiated with token id %d\n", retVal );
    } else {
        int published = 0;
        for (int i = 0; i < 5; i ++) {
            // Introducing sleep to simulate retry back off.
            // Clients can call any variant of the Publish API() here.
            usleep(500000L);
            retVal = jiot_client_MQTT_publish(client, topic, message, length);
            if (retVal >= 0) {
                printf("Publish initiated with token id %d\n", retVal );
                published = 1;
                break;
            }
        }

        if (!published) {
            printf("Error in publishing %d\n", retVal);
        }
    }
}

void stopHandler(int sig) {
    signal(SIGINT, NULL);
    signal(SIGTERM, NULL);
    doStop = 1;
}

void start() {

    jiot_client_MQTT_createOpts_t options = CreateOptions_initializer;
    int retVal = JIOT_MQTT_OPERATION_FAILURE;
    jiot_client_MQTT_callbackOpts_t callbacks;

    // Example app show cases creation of MQTT client with following-
    // 1. Creation with mandatory create options.
    // 2. Creation with both mandatory and optional create options.
    // 3. Creation with persistence options.
    // 4. Creation with SSL enabling.
    // Mandatory create options (Client application shall provide these) are-
    //      - clientId
    //      - username
    //      - password
    //      - host
    //      - port
    // Optional create options (Default values shall be used if client application doesn't provide) are-
    //      - qos (Default value 1)
    //      - retained (Default value 0)
    //      - keepAlive (Default value 60 secs)
    //      - useSSL (Default value 0)
    // Example app also show cases the connection using-
    //      - Connect to MQTT broker with default connection options.
    //      - Connect to MQTT broker with client application provided options.

#if defined(PERSISTENCE_CREATE)
    jiot_client_MQTT_persistOpts_t persistOption;
#endif

#if defined(CONNECT_OPTIONS)
    jiot_client_MQTT_connectOpts_t connOpts = ConnectOptions_initializer;
    jiot_client_MQTT_sslOpts_t sslOptions = SSLOptions_initializer;
    jiot_client_MQTT_willOpts_t willOptions = WillOptions_initializer;
#endif

#if defined (SSL_ONLY_OPTIONS)
    jiot_client_MQTT_sslOpts_t sslOptions = SSLOptions_initializer;
#endif

#if defined(CUSTOM_CREATE_OPTIONS) //Filling both mandatory and optional values.
    options.clientId = "XXXX";
    options.username = "deviceevent";
    options.password = "deviceevent@123";
    options.host = "ws://jpsit.jpdc.jiophone.net";
    options.port = "9797";
    options.qos = JIOT_QOS_TWO;
    options.retained = 1;
    options.keepAlive = 120;

#if defined(ENABLE_SSL_HANDSHAKE) // If needed turn on SSL.
    options.useSSL = 1;
#else
    options.useSSL = 0;
#endif //ENABLE_SSL_HANDSHAKE

    options.certPassword = NULL; // Completed filling mandatory and optional values.

#else // Filling only mandatory values. 
    options.clientId = "AAAA";
    options.username = "deviceevent";
    options.password = "deviceevent@123";
    options.host = "ws://jpsit.jpdc.jiophone.net";
    options.port = "9797";

#if defined(ENABLE_SSL_HANDSHAKE) // If needed turn ON SSL.
    options.useSSL = 1;
#endif //ENABLE_SSL_HANDSHAKE

#endif // End of filling Create Options.
    
#if defined(PERSISTENCE_CREATE) // Fill persistence options if needed.
    printf("Creating client with persistence...\n");
    persistOption.enablePersistence = 1;
    persistOption.type = JIOT_MQTT_PERSISTENCE_DEFAULT;
    persistOption.maxBufferedMessages = 100;
    char *dir = "/mnt/sdcard/mqtt_test";
    persistOption.context = dir;

    // Create MQTT client with persistence options.
    retVal = jiot_client_MQTT_createWithPersistence(&mqttClient, options, persistOption);

#else // Create MQTT client without persistence options.
    printf("Creating client without persistence\n");
    retVal = jiot_client_MQTT_create(&mqttClient, options);
#endif

    if (retVal == JIOT_MQTT_OPERATION_SUCCESS) {
        printf("Client created\n");
    }

    // MQTT client is created, set the callbacks now.
    memset(&callbacks, 0x00, sizeof(jiot_client_MQTT_callbackOpts_t));
    callbacks.connectionComplete = connectionComplete;
    callbacks.connectionFailed = connectionFailed;
    callbacks.deliveryComplete = messageDelivered;
    callbacks.deliveryFailed = messageFailed;
    callbacks.disconnectComplete = disconnectCompleted;
    callbacks.disconnectFailed = disconnectFailed;
    printf("Setting callbacks\n");
    jiot_client_MQTT_setcallbacks(mqttClient, mqttClient, callbacks);

    //Connect now!!
    printf("Connecting...\n");

#if defined(CONNECT_OPTIONS) // Set Application provided connection options.
    willOptions.topicName = "JIOT-LWT-Topic1";
    willOptions.message = "Client lost connection";

#if defined(CUSTOM_SSL) // Set the SSL options if needed.
    sslOptions.trustStore = "/mnt/sdcard/mqtt_test_truststore.pem";
    sslOptions.keyStore = "/mnt/sdcard/mqtt_test.pem";
    sslOptions.privateKey = "/mnt/sdcard/mqtt_test_key";
    sslOptions.privateKeyPassword = "mqtt-test";
    sslOptions.enableServerCertAuth = 1;
#endif // End of setting SSL options.

    connOpts.keepAliveInterval = 10;
    connOpts.cleanSession = 1;
    connOpts.maxInflight = 50;
    connOpts.connectTimeout = 20;
    connOpts.retryInterval = 2;
    connOpts.mqttVersion = JIOT_MQTTVERSION_DEFAULT;
    connOpts.autoReconnect = 1;
    connOpts.minRetryInterval = 2;
    connOpts.maxRetryInterval = 50;

#if defined(ENABLE_SSL_HANDSHAKE) // Set the SSL options inside the connection options.
    connOpts.sslOptions = &sslOptions;
#endif

#if defined(ENABLE_WILL) // Set LWT options inside the connection options.
    connOpts.willOptions = &willOptions;
#endif

#endif
  
#if defined(CONNECT_OPTIONS) // Connect the MQTT client with Application provided connect options.
    printf("Connecting with options\n");
    retVal = jiot_client_MQTT_connectWithOptions(mqttClient, &connOpts);
#elif defined(SSL_ONLY_OPTIONS)
    printf("Connecting with SSL only options\n");
    sslOptions.trustStore = "mqtt.pem";
    sslOptions.enableServerCertAuth = 1;
    retVal = jiot_client_MQTT_connectWithSSLOptions(mqttClient, sslOptions);
#else // Connect the MQTT client with default connect options.
    printf("Connecting without options\n");
    retVal = jiot_client_MQTT_connect(mqttClient);
#endif 
    
    if (retVal == JIOT_MQTT_OPERATION_SUCCESS) {
        printf("Connect intiated\n");
    } else {
        printf("Error in connecting... %d\n", retVal);
    }
}


int main() {
   // printf("\n Main: MQTT sample app \n");
    signal(SIGTERM, stopHandler);
    signal(SIGINT, stopHandler);
    start();
    //while(!doStop);
    usleep(120000000L);
    printf("Destroying the client\n");
    jiot_client_MQTT_destroy(&mqttClient);
    printf("Destroyed the client");
}
