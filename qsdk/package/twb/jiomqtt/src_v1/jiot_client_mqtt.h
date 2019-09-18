/**
 * @file jiot_client_mqtt.h
 * @brief Interfaces for MQTT Embedded client.
 *
 */

#if defined(__cplusplus)
 extern "C" {
#endif

#ifndef JIOT_CLIENT_MQTT_H_ 
#define JIOT_CLIENT_MQTT_H_ /** @hideinitializer */

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport) /** @hideinitializer */
  #define DLLExport __declspec(dllexport) /** @hideinitializer */
#else
  #define DLLImport extern /** @hideinitializer */
  #define DLLExport  __attribute__ ((visibility ("default"))) /** @hideinitializer */
#endif

/**
 * Return code: No error. Indicates successful completion of an MQTT client
 * operation.
 */
#define JIOT_MQTT_OPERATION_SUCCESS 0

/**
 * Return code: A generic error code indicating the failure of an MQTT client
 * operation.
 */
#define JIOT_MQTT_OPERATION_FAILURE -1

/**
 * Return code: The client is disconnected.
 */
#define JIOT_MQTT_OPERATION_DISCONNECTED -3

/**
 * Return code: The maximum number of messages allowed to be simultaneously
 * in-flight has been reached.
 */
#define JIOT_MQTT_OPERATION_MAX_MESSAGES_INFLIGHT -4

/**
 * Return code: An invalid UTF-8 string has been detected.
 */
#define JIOT_MQTT_OPERATION_BAD_UTF8_STRING -5

/**
 * Return code: A NULL parameter has been supplied when this is invalid.
 */
#define JIOT_MQTT_OPERATION_NULL_PARAMETER -6

/**
 * Return code: The topic has been truncated (the topic string includes
 * embedded NULL characters). String functions will not access the full topic.
 * Use the topic length value to access the full topic.
 */
#define JIOT_MQTT_OPERATION_TOPICNAME_TRUNCATED -7

/**
 * Return code: A qos parameter is not 0, 1 or 2
 */
#define JIOT_MQTT_OPERATION_BAD_QOS -9

/**
 * Return code: All 65535 MQTT msgids are being used
 */
#define JIOT_MQTT_OPERATION_NO_MORE_MSGIDS -10

/**
 * Return code: the request is being discarded when not complete
 */
#define JIOT_MQTT_REQUEST_OPERATION_INCOMPLETE -11

/**
 * Return code: no more messages can be buffered
 */
#define JIOT_MQTT_OPERATION_MAX_BUFFERED_MESSAGES -12

/**
 * Return code: Attempting SSL connection using non-SSL version of library
 */
#define JIOT_MQTT_OPERATION_SSL_NOT_SUPPORTED -13

/**
 * Return code: protocol prefix in serverURI should be tcp:// or ssl://
 */
#define JIOT_MQTT_OPERATION_BAD_PROTOCOL -14

/**
 * Default MQTT version used for connection with broker. Default is 3.1.1.
 */
#define JIOT_MQTTVERSION_DEFAULT 0

/**
 * MQTT version 3.1 shall be used for connection with broker. 
 */
#define JIOT_MQTTVERSION_3_1 3

/**
 * MQTT version 3.1.1 shall be used for connection with broker.
 */
#define JIOT_MQTTVERSION_3_1_1 4

/**
 * No file system persistence of messages published when MQTT client is not connected.
 * Messages shall be stored in memory buffer. If auto reconnect is enabled,
 * on reconnect, MQTT client shall attempt to publish messages stored in the memory buffer.
 */
#define JIOT_MQTT_PERSISTENCE_NONE 0

/**
 * Enable file system based persistence of messages published when MQTT client is not connected.
 * If auto reconnect is enabled, on reconnect, MQTT client shall attempt
 * to publish messages stored in the file system.
 */
#define JIOT_MQTT_PERSISTENCE_DEFAULT 1

/**
 * Handle to MQTT client. Client applications shall get the handle using
 * jiot_client_MQTT_create() API.
 */
typedef void* jiot_client_MQTT_Hndl_t;

/**
 * Token returned by the MQTT client to applications to track various MQTT
 * operations.
 */
typedef int jiot_client_MQTT_token_t;

/**
 * QOS levels that client can set during an MQTT operation.
 * JIOT_QOS_NONE    - Messages shall be sent to broker with fire and forget approach. No retires.
 * JIOT_QOS_DEFAULT - This shall be the default QOS if no QOS is specified. Messages shall be delivered
 *                    atleast once to broker. If auto reconnect is turned ON, MQTT client shall
 *                    attempt to resend any buffered messages published during offline.
 * JIOT_QOS_TWO     - Messages shall be delivered to broker exactly once. If auto reconnect is turned ON, MQTT client shall
 *                    attempt to resend any buffered messages published during offline.
 */
typedef enum jiot_client_MQTT_qos_t {
    JIOT_QOS_NONE = 0,
    JIOT_QOS_DEFAULT = 1,
    JIOT_QOS_TWO = 2
} jiot_client_MQTT_qos_t;

/**
 * Structure to represent MQTT message.
 */
typedef struct {
    /** The length of the MQTT message payload in bytes. */
    int payloadlen;
    /** A pointer to the payload of the MQTT message. */
    void* payload;
    /**
     * The quality of service (QoS) assigned to the message.
     * There are three levels of QoS:
     * <DL>
     * <DT><B>QoS0</B></DT>
     * <DD>Fire and forget - the message may not be delivered</DD>
     * <DT><B>QoS1</B></DT>
     * <DD>At least once - the message will be delivered, but may be
     * delivered more than once in some circumstances.</DD>
     * <DT><B>QoS2</B></DT>
     * <DD>Once and one only - the message will be delivered exactly once.</DD>
     * </DL>
     */
    jiot_client_MQTT_qos_t qos;
    /**
     * The retained flag serves two purposes depending on whether the message
     * it is associated with is being published or received.
     *
     * <b>retained = true</b><br>
     * For messages being published, a true setting indicates that the MQTT
     * server should retain a copy of the message. The message will then be
     * transmitted to new subscribers to a topic that matches the message topic.
     * For subscribers registering a new subscription, the flag being true
     * indicates that the received message is not a new one, but one that has
     * been retained by the MQTT server.
     *
     * <b>retained = false</b> <br>
     * For publishers, this ndicates that this message should not be retained
     * by the MQTT server. For subscribers, a false setting indicates this is
     * a normal message, received as a result of it being published to the
     * server.
     */
    int retained;
    /**
     * The dup flag indicates whether or not this message is a duplicate.
     * It is only meaningful when receiving QoS1 messages. When true, the
     * client application should take appropriate action to deal with the
     * duplicate message.
     */
    int dup;
    /** The message identifier is normally reserved for internal use by the
     * MQTT client and server.
     */
    int msgid;
} jiot_client_MQTT_message_t;

/**
 * Callback for MQTT message delivery complete. Client applications should implement this to get delivery complete status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param token   Token returned to the application when the message is accepted for publishing.
 */
typedef void jiot_client_MQTT_message_deliveryComplete(void *context, jiot_client_MQTT_token_t token);

/**
 * Callback for MQTT message delivery failure. Client applications should implement this to get delivery failure cause.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause   Cause of the failure.
 * @param token   Token returned to the application when the message is accepted for publishing.
 */
typedef void jiot_client_MQTT_message_deliveryFailed(void *context, const char *cause, jiot_client_MQTT_token_t token);

/**
 * Callback for MQTT client connection failed with broker. Client applications should implement this to get connection failed status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause   Cause of the connection failure.
 * @param code    Error code.
 */
typedef void jiot_client_MQTT_connectionFailed(void *context, const char *cause, int code);

/**
 * Callback for MQTT client connection complete with broker. Client applications should implement this to get connection complete status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param token   Token value to indicate connection state, it will be set to 0 for connection complete.
 */
typedef void jiot_client_MQTT_connectionComplete(void *context, jiot_client_MQTT_token_t token);

/**
 * Callback for MQTT client auto reconnect complete with broker. Client applications should implement this to know if MQTT client auto reconnected.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 */
typedef void jiot_client_MQTT_autoReconnectComplete(void *context);

/**
 * Callback for MQTT client disconnect complete with broker. Client applications should implement this to get disconnection complete.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 */
typedef void jiot_client_MQTT_disconnectionComplete(void *context);

/**
 * Callback for MQTT client disconnect failed with broker. Client applications should implement this to get disconnection failure.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause   Cause of the failure.
 * @param code    Error code.
 */
typedef void jiot_client_MQTT_disconnectionFailed(void *context, const char *cause, int code);

/**
 * Callback for subscription complete for a topic. Client applications should implement this to get subscription complete status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param token Token returned by the MQTT Client to application when subscription request was accepted.
 * @param qos   Qos granted by the server.
 */
typedef void jiot_client_MQTT_subscribeComplete(void *context, jiot_client_MQTT_token_t token, jiot_client_MQTT_qos_t qos);

/**
 * Callback for subscription complete for more than one topic. Client applications should implement this to get subscription complete status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param token   Token returned by the MQTT Client to application when subscription request was accepted.
 * @param qos     Array of qos returned by the server.
 */
typedef void jiot_client_MQTT_subscribeManyComplete(void *context, jiot_client_MQTT_token_t token, int *qos);

/**
 * Callback for subscription failure for a topic. Client applications should implement this to get subscription failure status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause   Cause of the failure.
 * @param code    Error code.
 * @param token   Token returned by the MQTT Client to application when subscription request was accepted.
 */
typedef void jiot_client_MQTT_subscribeFailed(void *context, const char *cause, int code, jiot_client_MQTT_token_t token);

/**
 * Callback for message arrived. Client applications should implement this to get messages for the topics they have subscribed.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context  Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param topic    Name of the topic.
 * @param topicLen Length of the topic name.
 * @param message  Message received. The memory allocated for the message
 *                 is freed once the callback returns. Hence, client applications
 *                 should make a copy of the message if needed. 
 */
typedef void jiot_client_MQTT_messageArrived(void *context, char *topic, int topicLen, jiot_client_MQTT_message_t *message);

/**
 * Callback for unsubscribe complete. Client applications should implement this to get unsubscribe complete status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param token   Token returned by the MQTT Client to application when unsubscription request was accepted. 
 */
typedef void jiot_client_MQTT_unsubscribeComplete(void *context, jiot_client_MQTT_token_t token);

/**
 * Callback for unsubscribe failure. Client applications should implement this to get unsubscribe failure status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause Cause of the failure.
 * @param code  Error code.
 * @param token Token returned by the MQTT Client to application when unsubscription request was accepted.
 */
typedef void jiot_client_MQTT_unsubscribeFailed(void *context, const char *cause, int code, jiot_client_MQTT_token_t token);

/**
 * Callback for connection loss with broker. Client applications should implement this to get connection lost status.
 * Client applications can set this callback using jiot_client_MQTT_setcallbacks() API.
 * @param context Application specific context passed to jiot_client_MQTT_setcallbacks() API.
 * @param cause Cause of the connection loss.
 */
typedef void jiot_client_MQTT_connectionLost(void *context, const char *cause);

/**
 * Structure to hold CreateOptions. Client Apps to pass this in
 * jiot_client_MQTT_create() API.
 */
typedef struct {
        /**
         * Unique client Id.
         */
	char *clientId;

        /**
         * Default Qos to be used with publish. Client Apps can override by
         * specifying Qos value while publishing.
         */
	jiot_client_MQTT_qos_t qos;

        /**
         * Message to be retained by server.
         */
	int retained;

        /**
         * Username used to connect to broker.
         */
	char *username;

        /**
         * Password used to connect to broker.
         */
	char *password;

        /**
         * Broker host URL.
         */
	char *host;

        /**
         * Port.
         */
	char *port;

        /**
         * Keep alive value.
         */
	int keepAlive;

        /**
         * Use SSL to connect to broker.
         */
	int useSSL;

        /**
         * Certificate password, valid only when SSL is used.
         */
	char *certPassword;

        /**
         * Option to turn off logging in MQTT.
         */
        int enableMqttLogs;

        /**
         * Log file path for MQTT logging.
         */
        char *mqttLogPath;
} jiot_client_MQTT_createOpts_t;

#define CreateOptions_initializer {"0", JIOT_QOS_NONE, 0, NULL, NULL, NULL, NULL, 60, 0, NULL, 1, NULL} /** @hideinitializer */

/**
 * Structure to hold last will (LWT) options. This can be used for debugging purposes when a client connection is lost.
 * Broker shall publish a message to a topic and clients can subscribe to that topic. Broker shall publish this message
 * when client connection is lost unexpectedly. 
 */
typedef struct {
    /**
     * Topic to publish LWT message.
     */
    const char *topicName;

    /**
     * LWT message payload.
     */
    const char *message;

    /**
     * Whether server should retain this message.
     */
    int retained;

    /**
     * Qos of the message while publishing.
     */
    jiot_client_MQTT_qos_t qos;
} jiot_client_MQTT_willOpts_t;

#define WillOptions_initializer {NULL, NULL, 1, JIOT_QOS_NONE} /** @hideinitializer */

/**
 * Structure to hold SSL options.
 */
typedef struct {

    /**
     * Trust store directory path where the server's public certificate as .pem file is placed.
     */
    const char *trustStore;

    /**
     * Directory path where client's certificate as .pem file is placed.
     */
    const char *keyStore;

    /**
     * Private key of the client certificate.
     */
    const char *privateKey;

    /**
     * Client private key password.
     */
    const char *privateKeyPassword;

    /**
     * Enable cipher suites.
     */
    const char *enabledCipherSuites;

    /**
     * Enable server certificate validation.
     */
    int enableServerCertAuth;
} jiot_client_MQTT_sslOpts_t;

#define SSLOptions_initializer {"/usr/lib/ssl/certs/libicf-mqtt-root.pem", NULL, NULL, NULL, NULL, 1} /** @hideinitializer */

/**
 * Connection options structure. Client application shall specify various configuration parameters here.
 */
typedef struct {

    /**
     * Keep alive interval for current connection.
     */
    int keepAliveInterval;

    /**
     * Clean any previous session before the new connection.
     */
    int cleanSession;

    /**
     * Max in flight messages simultaneously.
     */
    int maxInflight;

    /**
     * Connection time out in seconds.
     */
    int connectTimeout;

    /**
     * Retry interval in case of auto reconnect.
     */
    int retryInterval;

    /**
     * MQTT version.
     */
    int mqttVersion;

    /**
     * Enable auto reconnect.
     */
    int autoReconnect;

    /**
     * Min retry interval.
     */
    int minRetryInterval;

    /**
     * Max retry interval.
     */
    int maxRetryInterval;

    /**
     * LWT options.
     */
    jiot_client_MQTT_willOpts_t *willOptions;

    /**
     * SSL options.
     */
    jiot_client_MQTT_sslOpts_t *sslOptions;
} jiot_client_MQTT_connectOpts_t;

#define ConnectOptions_initializer {60, 1, 10, 30, 0, 0, 0, 1, 60, NULL, NULL} /** @hideinitializer */

/**
 * Structure to hold persistence options.
 */
typedef struct {

    /**
     * Enable persistence of messages published when offline.
     */
    int enablePersistence;

    /**
     * Type of persistence, file system or in-memory. JIOT_MQTT_PERSISTENCE_NONE is for in-memory persistence,
     * JIOT_MQTT_PERSISTENCE_DEFAULT is for file system based persistence.
     */
    int type;

    /**
     * Application specific context. In case of JIOT_MQTT_PERSISTENCE_DEFAULT this shall be directory path where
     * offline messages shall be stored.
     */
    void *context;

    /**
     * Max no of messages buffered.
     */
    int maxBufferedMessages;
} jiot_client_MQTT_persistOpts_t;

/**
 * Callback options structure. Client Apps should set the needed callbacks and
 * pass the same in jiot_client_MQTT_setcallbacks() API. Set NULL for any callback
 * not needed. 
 */
typedef struct {
     jiot_client_MQTT_message_deliveryComplete *deliveryComplete;
     jiot_client_MQTT_message_deliveryFailed *deliveryFailed;
     jiot_client_MQTT_connectionComplete *connectionComplete;
     jiot_client_MQTT_connectionFailed *connectionFailed;
     jiot_client_MQTT_autoReconnectComplete *autoReconnectComplete;
     jiot_client_MQTT_disconnectionComplete *disconnectComplete;
     jiot_client_MQTT_disconnectionFailed *disconnectFailed;
     jiot_client_MQTT_subscribeComplete *subscribeComplete;
     jiot_client_MQTT_subscribeManyComplete *subscribeManyComplete;
     jiot_client_MQTT_subscribeFailed *subscribeFailed;
     jiot_client_MQTT_messageArrived *messageArrived;
     jiot_client_MQTT_unsubscribeComplete *unsubscribeComplete;
     jiot_client_MQTT_unsubscribeFailed *unsubscribeFailed;
     jiot_client_MQTT_connectionLost *connectionLost;
 } jiot_client_MQTT_callbackOpts_t;

/**
 * This function sets the callback functions for a specific client.
 * If your client application doesn't use a particular callback, set the
 * relevant parameter to NULL. Any necessary message acknowledgements and
 * status communications are handled in the background without any intervention
 * from the client application.
 *
 * <b>Note:</b> The MQTT client must be disconnected when this function is
 * called.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param context A pointer to any application-specific context. The
 * the <i>context</i> pointer is passed to each of the callback functions to
 * provide access to the context information in the callback.
 * @param callbacks Callback structure.
 *
 */
DLLExport void jiot_client_MQTT_setcallbacks(jiot_client_MQTT_Hndl_t handle, void *context, jiot_client_MQTT_callbackOpts_t callbacks);

/**
 * This function attempts to connect a previously-created client (see
 * jiot_client_MQTT_create()) to an MQTT broker. If you
 * want to enable asynchronous message and status notifications, you must call
 * jiot_client_MQTT_setCallbacks() prior to jiot_client_MQTT_connect().
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client connect request was accepted.
 * If the client was unable to connect to the server, an error code is
 * returned via the jiot_MQTT_connectionFailed() callback, if set.
 * Error codes greater than 0 are returned by the MQTT protocol:<br><br>
 * <b>1</b>: Connection refused: Unacceptable protocol version<br>
 * <b>2</b>: Connection refused: Identifier rejected<br>
 * <b>3</b>: Connection refused: Server unavailable<br>
 * <b>4</b>: Connection refused: Bad user name or password<br>
 * <b>5</b>: Connection refused: Not authorized<br>
 * <b>6-255</b>: Reserved for future use<br>
 */

DLLExport int jiot_client_MQTT_connect(jiot_client_MQTT_Hndl_t handle);

/**
 * This function attempts to connect a previously-created client (see
 * jiot_client_MQTT_create()) to an MQTT broker. If you
 * want to enable asynchronous message and status notifications, you must call
 * jiot_client_MQTT_setCallbacks() prior to jiot_client_MQTT_connectWithOptions().
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param options, Connection options structure.
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client connect request was accepted.
 * If the client was unable to connect to the server, an error code is
 * returned via the jiot_MQTT_connectionFailed() callback, if set.
 * Error codes greater than 0 are returned by the MQTT protocol:<br><br>
 * <b>1</b>: Connection refused: Unacceptable protocol version<br>
 * <b>2</b>: Connection refused: Identifier rejected<br>
 * <b>3</b>: Connection refused: Server unavailable<br>
 * <b>4</b>: Connection refused: Bad user name or password<br>
 * <b>5</b>: Connection refused: Not authorized<br>
 * <b>6-255</b>: Reserved for future use<br>
 */
DLLExport int jiot_client_MQTT_connectWithOptions(jiot_client_MQTT_Hndl_t handle, jiot_client_MQTT_connectOpts_t *options);

/**
 * This function attempts to connect a previously-created client (see
 * jiot_client_MQTT_create()) to an MQTT broker. If you
 * want enable asynchronous message and status notifications, you must call
 * jiot_client_MQTT_setCallbacks() prior to jiot_client_MQTT_connectWithSSLOptions().
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param sslOptions, SSL options structure.
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client connect request was accepted.
 * If the client was unable to connect to the server, an error code is
 * returned via the jiot_MQTT_connectionFailed() callback, if set.
 * Error codes greater than 0 are returned by the MQTT protocol:<br><br>
 * <b>1</b>: Connection refused: Unacceptable protocol version<br>
 * <b>2</b>: Connection refused: Identifier rejected<br>
 * <b>3</b>: Connection refused: Server unavailable<br>
 * <b>4</b>: Connection refused: Bad user name or password<br>
 * <b>5</b>: Connection refused: Not authorized<br>
 * <b>6-255</b>: Reserved for future use<br>

 */

DLLExport int jiot_client_MQTT_connectWithSSLOptions(jiot_client_MQTT_Hndl_t handle, jiot_client_MQTT_sslOpts_t sslOpts);

/**
 * This function attempts to disconnect the client from the MQTT
 * server. Its recommended to unsubscribe from any topics subscribed
 * before calling this API.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client successfully disconnects from
 * the server. An error code is returned if the client was unable to disconnect
 * from the server
 */

DLLExport int jiot_client_MQTT_disconnect(jiot_client_MQTT_Hndl_t handle);

/**
 * This function allows the client application to test whether or not a
 * client is currently connected to the MQTT server.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTTConnectionClient_create().
 * @return Boolean true if the client is connected, otherwise false.
 */
DLLExport int jiot_client_MQTT_isConnected(jiot_client_MQTT_Hndl_t handle);

/**
 * This function attempts to publish a message to a given topic.
 * An ::jiot_client_MQTT_token_t is issued when this function returns successfully.
 * If the client application needs to test for successful delivery of messages,
 * a callback should be set (see ::MQTTMessage_deliveryComplete()).
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic The topic associated with this message.
 * @param payload A pointer to the byte array payload of the message.
 * @param payloadlen The length of the payload in bytes.
 * @return ::jiot_client_MQTT_token_t, This token shall be returned in the callback 
 *  methods once message is delivered.
 *
 */

DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_publish(jiot_client_MQTT_Hndl_t handle, const char *topic, void *payload, int payloadlen);

/**
 * This function attempts to publish a message to a given topic.
 * An ::jiot_client_MQTT_token_t is issued when this function returns successfully.
 * If the client application needs to test for successful delivery of messages,
 * a callback should be set (see ::MQTTMessage_deliveryComplete()).
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic The topic associated with this message.
 * @param payload A pointer to the byte array payload of the message.
 * @param payloadlen The length of the payload in bytes.
 * @param qos Qos value.
 * @return ::jiot_client_MQTT_token_t, This token shall be returned in the callback
 * methods once message is delivered.
 *
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_publishWithQos(jiot_client_MQTT_Hndl_t handle, const char *topic, void *payload, int payloadlen, jiot_client_MQTT_qos_t qos);

/**
 * This function attempts to publish a message to a given topic.
 * An ::jiot_client_MQTT_token_t is issued when this function returns successfully.
 * If the client application needs to test for successful delivery of messages,
 * a callback should be set (see ::MQTTMessage_deliveryComplete()).
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic The topic associated with this message.
 * @param payload A pointer to the byte array payload of the message.
 * @param payloadlen The length of the payload in bytes.
 * @param qos Qos value.
 * @param retained The retained flag for the message.
 * @return ::jiot_client_MQTT_token_t, This token shall be returned in the callback
 * methods once message is delivered.
 *
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_publishWithQosAndRetained(jiot_client_MQTT_Hndl_t handle, const char *topic, void *payload, int payloadlen, jiot_client_MQTT_qos_t qos, int retained);

/**
 * Function to subscribe to a topic.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic to subscribe for.
 * @return ::jiot_client_MQTT_token_t This token shall be returned in the callback methods. 
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_subscribe(jiot_client_MQTT_Hndl_t handle, const char *topic);

/**
 * Function to subscribe to a topic and a given Qos value.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic to subscribe for.
 * @param qos value.
 * @return::jiot_client_MQTT_token_t This token shall be returned in the callback methods.  
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_subscribeWithQos(jiot_client_MQTT_Hndl_t handle, const char *topic, jiot_client_MQTT_qos_t qos);

/**
 * Function to subscribe to many topics.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param count of topics.
 * @param topic array.
 * @return::jiot_client_MQTT_token_t, This token shall be returned in the callback methods. 
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_subscribeMany(jiot_client_MQTT_Hndl_t handle, int count, char * const *topic);

/**
 * Function to subscribe to many with a Qos value.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param count of topics.
 * @param topic array.
 * @param qos value.
 * @return::jiot_client_MQTT_token_t, This token shall be returned in the callback methods. 
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_subscribeManyWithQos(jiot_client_MQTT_Hndl_t handle, int count, char * const *topic, jiot_client_MQTT_qos_t *qos);

/**
 * Function to unsubscribe.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param topic to unsubscribe.
 * @return::jiot_client_MQTT_token_t, This token shall be returned in the callback methods.
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_unsubscribe(jiot_client_MQTT_Hndl_t handle, const char *topic);

/**
 * Function to unsubscribe from many topics.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param length count of topics.
 * @param topics array.
 * @return::jiot_client_MQTT_token_t, This token shall be returned in the callback
 * methods.
 */
DLLExport jiot_client_MQTT_token_t jiot_client_MQTT_unsubscribeMany(jiot_client_MQTT_Hndl_t handle, int length, char * const *topics );

/**
 * This function sets a pointer to an array of tokens for
 * messages that are currently in-flight (pending completion).
 * <b>Important note:</b> The memory used to hold the array of tokens is
 * malloc()'d in this function. The client application is responsible for
 * freeing this memory when it is no longer required.
 * @param handle A valid client handle from a successful call to
 * jiot_client_MQTT_create().
 * @param tokens The address of a pointer to an ::jiot_client_MQTT_token_t.
 * When the function returns successfully, the pointer is set to point to an
 * array of tokens representing messages pending completion. The last member of
 * the array is set to -1 to indicate there are no more tokens. If no tokens
 * are pending, the pointer is set to NULL.
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the function returns successfully.
 * An error code is returned if there was a problem obtaining the list of
 * pending tokens.
 */
DLLExport int jiot_client_MQTT_getPendingTokens(jiot_client_MQTT_Hndl_t handle, jiot_client_MQTT_token_t **tokens);

/**
 * Function to create the MQTT Client Handle.
 * @param handle to jiot_client_MQTT_Hndl_t.
 * @param options CreateOptions used for creating the client handle.
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client is successfully created, otherwise
 * an error code is returned.
 */
DLLExport int jiot_client_MQTT_create(jiot_client_MQTT_Hndl_t *handle, jiot_client_MQTT_createOpts_t options);

/**
 * Function to create the MQTT Client Handle with persistence options.
 * @param handle to jiot_client_MQTT_Hndl_t.
 * @param options CreateOptions.
 * @param persistOpts Options to persist offline messages.
 * @return ::JIOT_MQTT_OPERATION_SUCCESS if the client is successfully created, otherwise
 * an error code is returned.
 */
DLLExport int jiot_client_MQTT_createWithPersistence(jiot_client_MQTT_Hndl_t *handle, jiot_client_MQTT_createOpts_t options, jiot_client_MQTT_persistOpts_t persistOpts);

/**
 * Function to destroy the MQTT Client Handle.
 * @param handle to jiot_client_MQTT_Hndl_t.
 */
DLLExport void jiot_client_MQTT_destroy(jiot_client_MQTT_Hndl_t *handle);

#endif /* JIOT_CLIENT_MQTT_H_ */
#ifdef __cplusplus
     }
#endif
