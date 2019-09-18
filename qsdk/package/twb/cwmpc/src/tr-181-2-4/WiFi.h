#ifndef _GS__WIFI_H_
#define _GS__WIFI_H_

/**@obj WiFiAccessPointAssociatedDevice **/
typedef struct WiFiAccessPointAssociatedDevice {
    char *          mACAddress;
    unsigned char   authenticationState;
    unsigned        lastDataDownlinkRate;
    unsigned        lastDataUplinkRate;
    char *          signalStrength;
    unsigned        retransmissions;
    unsigned char   active;
} WiFiAccessPointAssociatedDevice;
/**@endobj  **/

/**@obj WiFiAccessPointSecurity **/
typedef struct WiFiAccessPointSecurity {
    unsigned char   reset;
    char *          modesSupported;
    char *          modeEnabled;
    char *          preSharedKey;
    char *          keyPassphrase;
} WiFiAccessPointSecurity;
/**@endobj  **/

/**@obj WiFiAccessPointWPS **/
typedef struct WiFiAccessPointWPS {
    unsigned char   enable;
    char *          configMethodsSupported;
    char *          configMethodsEnabled;
} WiFiAccessPointWPS;
/**@endobj  **/

/**@obj WiFiAccessPoint **/
typedef struct WiFiAccessPoint {
    unsigned char   enable;
    char *          status;
    Instance *      sSIDReference;
    unsigned char   sSIDAdvertisementEnabled;
    unsigned        associatedDeviceNumberOfEntries;
} WiFiAccessPoint;
/**@endobj  **/

/**@obj WiFiRadioStats **/
typedef struct WiFiRadioStats {
    long long   bytesSent;
    long long   bytesReceived;
    long long   packetsSent;
    long long   packetsReceived;
    unsigned    errorsSent;
    unsigned    errorsReceived;
    unsigned    discardPacketsSent;
    unsigned    discardPacketsReceived;
} WiFiRadioStats;
/**@endobj  **/

/**@obj WiFiRadio **/
typedef struct GS_WiFiRadio {
    unsigned char   enable;
    char *          status;
    char *          name;
    unsigned        maxBitRate;
    char *          supportedFrequencyBands;
    char *          operatingFrequencyBand;
    char *          supportedStandards;
    char *          operatingStandards;
    char *          possibleChannels;
    char *          channelsInUse;
    unsigned        channel;
    unsigned char   autoChannelSupported;
    unsigned char   autoChannelEnable;
    char *          autoChannelRefreshPeriod;
    char *          operatingChannelBandwidth;
    char *          extensionChannel;
    char *          guardInterval;
    char *          mCS;
    char *          transmitPowerSupported;
    int             transmitPower;
    unsigned char   iEEE80211hSupported;
    unsigned char   iEEE80211hEnabled;
    char *          regulatoryDomain;
} GS_WiFiRadio;
/**@endobj  **/

/**@obj WiFiSSIDStats **/
typedef struct WiFiSSIDStats {
    long long   bytesSent;
    long long   bytesReceived;
    long long   packetsSent;
    long long   packetsReceived;
    unsigned    errorsSent;
    unsigned    errorsReceived;
} WiFiSSIDStats;
/**@endobj  **/

/**@obj WiFiSSID **/
typedef struct GS_WiFiSSID {
    unsigned char   enable;
    char *          status;
    char *          name;
    char *          bSSID;
    char *          mACAddress;
    char *          sSID;
} GS_WiFiSSID;
/**@endobj  **/
#endif /* _GS__WIFI_H_ */
