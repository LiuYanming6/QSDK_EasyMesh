//
// Created by jio on 7/26/18.
//

#ifdef __cplusplus
extern "C"{
#endif

#ifndef WIFIMESH_DATACOLLECTOR_WIFIMESH_UTILS_H
#define WIFIMESH_DATACOLLECTOR_WIFIMESH_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#define CMD_BUF_SIZE 2048
#define IFSTATS_NAME_LENGTH 64
#define MAC_ADDR_LENGTH 18

//Structure for LAN and Wifi data fields.
typedef struct _ifstats_t {
    char device[IFSTATS_NAME_LENGTH];
    uint64_t rxBytes;
    uint64_t rxPackets;
    uint64_t rxErrors;
    uint64_t rxDrop;
    uint64_t rxFifo;
    uint64_t rxFrame;
    uint64_t rxCompressed;
    uint64_t rxMulticast;
    uint64_t txBytes;
    uint64_t txPackets;
    uint64_t txErrors;
    uint64_t txDrop;
    uint64_t txFifo;
    uint64_t txCollisions;
    uint64_t txCarrier;
    uint64_t txCompressed;

    struct _ifstats_t *next;
} ifstats_t;

/*TWB EAP*/
typedef struct _phy_mode_cap {
    char mode[32];
    int max_rate[3];
} phy_mode_cap;

static phy_mode_cap max_data_rate[] = {
#if 0
	{ "IEEE80211_MODE_AUTO", },
        { "IEEE80211_MODE_11A", },
        { "IEEE80211_MODE_11B", },
        { "IEEE80211_MODE_11G", },
        { "IEEE80211_MODE_FH", },
        { "IEEE80211_MODE_TURBO_A", },
        { "IEEE80211_MODE_TURBO_G", },
#endif
	{ "IEEE80211_MODE_11NA_HT20",	72000,	144000,	216000},
        { "IEEE80211_MODE_11NG_HT20",   72000,  144000, 216000},
        { "IEEE80211_MODE_11NA_HT40PLUS",   150000,     300000,    450000},
        { "IEEE80211_MODE_11NA_HT40MINUS",  150000,     300000,    450000},
        { "IEEE80211_MODE_11NG_HT40PLUS",   150000,     300000,    450000},
        { "IEEE80211_MODE_11NG_HT40MINUS",  150000,     300000,    450000},
        { "IEEE80211_MODE_11NG_HT40",	150000,     300000,    450000},
        { "IEEE80211_MODE_11NA_HT40",   150000,     300000,    450000},
	{ "IEEE80211_MODE_11AC_VHT20",	86000,	173000,	288000},
        { "IEEE80211_MODE_11AC_VHT40PLUS",	200000,	400000,	600000},
        { "IEEE80211_MODE_11AC_VHT40MINUS",	200000, 400000, 600000},
        { "IEEE80211_MODE_11AC_VHT40",		200000, 400000, 600000},
        { "IEEE80211_MODE_11AC_VHT80",  433000,	866000,	1300000},
        { "IEEE80211_MODE_11AC_VHT160", 866000,	1733000,	2340000}
        //{ "IEEE80211_MODE_11AC_VHT80_80",  },
};
/**/

//Function to parse the /proc/net/dev file and build the IFStats_t linked list.
ifstats_t *get_ifstats(char *statsFile, char *eth);

void release_ifstats(ifstats_t *stats);

//Function to fetch delta(difference) between current and previous statistics.
ifstats_t *get_ifstats_delta(char *eth);

void util_popen(const char *cmd, ...);

int util_system(const char *c, ...);

int util_popen_with_output(char **output, int32_t *size, const char *command_str, ...);

long util_popen_with_output_retry(const char *cmd_str);

long util_popen_with_output_long(const char *c, ...);

//Function to get wifimesh device Serial Number.
static inline int get_device_serial(char **output, int32_t *size) {
     return util_popen_with_output(output, size, "state_cfg get sn");
}

//Function to get device model reporting the data set.
static inline int get_device_model(char **output, int32_t *size) {
	// ard code
     //char model[20] = "JCM0112";
     //*size = strlen(model);
     //*output = (char *) malloc (sizeof(char) * (*size));
     //strcpy (*output, model);
     //return 0;
    //return util_popen_with_output(output, size, "sed -n p /tmp/cpe3/dps.productclass");
    return util_popen_with_output(output, size, "state_cfg get model_name");
}

//Function to get SW version used in the device.
static inline int get_device_firmware_version(char **output, int32_t *size) {
    return util_popen_with_output(output, size, "cat /etc/openwrt_version");
}

//Function to get MAC Address of the device.
static inline int get_device_mac_address(char **output, int32_t *size) {
    //return util_popen_with_output(output, size, "state_cfg get ethaddr");
    return util_popen_with_output(output, size, "ifconfig br-lan | grep HWaddr | awk '{print $5}'");
}

//Function to get mesh mode of the device.
static inline int get_device_mesh_mode(char **output, int32_t *size) {
    //return util_popen_with_output(output, size, "uci get repacd.repacd.DeviceRole");
    return util_popen_with_output(output, size, "uci get repacd.repacd.is_onboard"); //TWB EAP: EasyMesh 
}

//Function to backhaul type of the device.
static inline int get_device_bkhl_type(char **output, int32_t *size) {
    //return util_popen_with_output_long("uci get repacd.repacd.IsEthBackhaul");
    return util_popen_with_output(output, size, "uci get repacd.repacd.Role"); //TWB EAP: EasyMesh
}


//Function to get CPU Usage.
static inline long get_cpu_usage() {
    char *cmd_str = "grep 'cpu ' /proc/stat | awk 'function ceil(x){return int(x)+(x>int(x))}{usage=($2+$4)*100/($2+$4+$5)} END {print ceil(usage)}'";
    return util_popen_with_output_long(cmd_str);
}

//Function to get RAM(memory) usage
static inline long get_memory_usage() {
    char *cmd_str = "free -m | awk 'function ceil(x){return int(x)+(x>int(x))} NR==2{print ceil($3*100/$2)}'";
    return util_popen_with_output_long(cmd_str);
}

/*TWB EAP*/
static inline int get_re_bh_rssi(char *ifname) {
    char cmd_str[128]={0};
    int rssi = 0;
    //sprintf(cmd_str, "wlanconfig %s list | awk '!(NR==1){print $6}'", ifname);
    sprintf(cmd_str, "wlanconfig %s list | awk '(NR==2){print $6}'", ifname);
    rssi = ( -95 + util_popen_with_output_long(cmd_str));
    return rssi;
}

static inline int get_5g_re_bh_mcs(char *ifname) {
    //char cmd_str[128]={0};
    char *cmd_str = "cat /tmp/50_backhaul_mcs | awk '{m=$1;mcs=0;for(i=1;i<=NF;i++)if($i>m){m=$i;mcs=i-1};print mcs}'";
    util_system("sh /sbin/50_backhaul_mcs");
    //sprintf(cmd_str, "iwpriv %s %s | awk -F \":\" '{printf $2}'", ifname, strcmp(ifname, "ath01")? "get11NRates" : " get_vhtmcs");
    return util_popen_with_output_long(cmd_str);
}

static inline int get_24g_re_bh_mcs(char *ifname) {
    //char cmd_str[128]={0};
    char *cmd_str = "cat /tmp/24_backhaul_mcs | awk '{m=$1;mcs=0;for(i=1;i<=NF;i++)if($i>m){m=$i;mcs=i-1};print mcs}'";
    util_system("sh /sbin/24_backhaul_mcs");
    return util_popen_with_output_long(cmd_str);
}

static inline int get_80211_channel(char *ifname) {
    char cmd_str[128]={0};
    sprintf(cmd_str, "iwlist %s chan | grep '(Channel' | awk -F ' ' '{print $5}' | tr -d ')'", ifname);
    return util_popen_with_output_long(cmd_str);
}

//static inline int get_channel_utilization(int channel) {
static inline int get_channel_utilization(char *intf) {
    char cmd_str[128]={0};
    //sprintf(cmd_str, "(echo bandmon s; sleep 1) | hyt | grep \"Channel %d\" | awk -F ':' '{print $3}' | awk '{print $1}'", channel);
    sprintf(cmd_str, "iwpriv %s get_chutil | awk -F ':' '{print $2}'", intf);
    return util_popen_with_output_long(cmd_str);
}

static inline int get_easymesh_bh_interface(char **output, int32_t *size, char *ifname) {

    return util_popen_with_output(output, size, "iwconfig %s | grep \"Mode:\" | awk -F' ' '{print $1}' | awk -F':' '{print $2}'", ifname);
}




void backup_stats_file(); //Function to create a backup of stats file. 
void rename_stats_file(); //Function to rename current stats file to previous stats file.


//Function to get Ethernet interface stats.
static inline ifstats_t *get_stats_LAN() {
    return get_ifstats_delta("eth0:");
}

//Function to get Fronthaul Wifi 2.4 stats.
static inline ifstats_t *get_stats_fronthaul_WIFI24() {
    return get_ifstats_delta("ath0:");
}

//Function to get Backhaul Wifi 2.4 stats.
static inline ifstats_t *get_stats_backhaul_WIFI24() {
    return get_ifstats_delta("ath01:");
}

//Function to get Fronthaul Wifi 5.0 stats.
static inline ifstats_t *get_stats_fronthaul_WIFI50() {
    return get_ifstats_delta("ath1:");
}

//Function to get Backhaul Wifi 5.0 stats.
static inline ifstats_t *get_stats_backhaul_WIFI50() {
    return get_ifstats_delta("ath11:");
}

//Function to get Backhaul Wifi 5.0 stats.
static inline ifstats_t *get_wifi_stats(char *intf) {
    char interface[16] = {0};
    strcpy(interface, intf);
    strncat(interface, ":", 1);
    return get_ifstats_delta(interface);
}


//Function to get uptime(Number of seconds the device has been up).
long get_uptime();

//Function to get number of clients connected to wired LAN.
long get_lan_clients_count_LAN();

//Function to get number of clients with IPv6 address.
long get_lan_clients_count_IPV6();

//Function to get number of clients connected to wireless LAN 2.4 GHz radio
long get_wlan_clients_count_WIFI24(char *intf);

//Function to get number of clients connected to wireless LAN 5.0 GHz radio
long get_wlan_clients_count_WIFI50(char *intf);

//Function to get channel used on 2.4 GHz radio
long get_channel_WIFI24();

//Function to get channel used on 5.0 GHz radio
long get_channel_WIFI50();


typedef struct _client_info_t {
    char device[MAC_ADDR_LENGTH];
    long rssi_value;
    long txrate_value;
    long maxtxrate_value;	/*TWB EAP*/
    struct _client_info_t *next;
} client_info_t;

client_info_t *get_client_info(char *interface);

/*TWB EAP*/
int get_mesh_parent_mac(char *get_str, int x);
int get_mesh_lan_client(char *get_str, int x);

/*Function to get signal strength information of clients connected on WLAN. 
Format of data {"mac": "string", "rssi": number, "mcs" : number} */
void release_info(client_info_t *stats);


#endif //WIFIMESH_DATACOLLECTOR_WIFIMESH_UTILS_H
#ifdef __cplusplus
}
#endif

