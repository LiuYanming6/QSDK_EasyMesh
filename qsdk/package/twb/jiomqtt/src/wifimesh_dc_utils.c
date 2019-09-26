//
// Created by jio on 7/26/18.
//

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <linux/unistd.h>       /* for _syscallX macros/related stuff */
#include <linux/kernel.h>       /* for struct sysinfo */
#include <sys/sysinfo.h>
#include "log.h"
#include "wifimesh_dc_utils.h"
#include "common_utils.h"
/*TWB EAP*/
#include "twb_util_hyd.h"


char *proc_net_dev = "/proc/net/dev";
char *proc_net_dev_current = "/tmp/proc_net_dev_current";
char *proc_net_dev_previous = "/tmp/proc_net_dev_previous";

// Function to parse the /proc/net/dev file and build the IFStats_t linked list.
ifstats_t *get_ifstats(char *statsFile, char *eth) {
    char *fileText = NULL;
    int32_t size = 0;
    int error = 0;

    if (!file_exist(statsFile)) {
        LERROR("file not found [%s]", statsFile);
        return NULL;
    }

    char *grep_s1 = "| ";
    char *grep_s2 = "grep ";
    char *grep_s3 = eth;
    if (eth == NULL) {
        grep_s1 = "";
        grep_s2 = "";
        grep_s3 = "";
    }
    error = util_popen_with_output(&fileText, &size,
                                   "cat %s 2>&1 | sed '1,2d' %s%s%s",
                                   statsFile, grep_s1, grep_s2, grep_s3);

    if (error != 0) {
        if (!fileText) {
            errno = error;
            return NULL;
        }
    }
    
    ifstats_t *returnStats = NULL, *stats = NULL;
    
    //if (!stats) {
    //    free(fileText);
    //    fileText = NULL;
    //    errno = error;
    //    return NULL;
    //}

	
    
    //if (eth == NULL) {
    //    strncpy(stats->device, "devall", IFSTATS_NAME_LENGTH - 1);
    //} else {
    //    strncpy(stats->device, eth, IFSTATS_NAME_LENGTH - 1);
    //}

	

    //scan output for stats as per pattern in /proc/net/dev
    char *lineptr, *tokenptr;
    char *line, *token;
    int i = 0;

    for (char *lines = fileText;; lines = NULL) {
        line = strtok_r(lines, "\n", &lineptr);
        if (line == NULL) {
            break;
        } else {
			// this is new line
			// create new stats struct
			stats = (ifstats_t *) malloc(sizeof(ifstats_t));
			//LDEBUG("Allocated address by pointer: [%p]", stats);
			memset(stats, 0, sizeof(ifstats_t));
			stats->rxBytes = (uint64_t) 0;
			stats->rxPackets = (uint64_t) 0;
			stats->rxErrors = (uint64_t) 0;
			stats->rxDrop = (uint64_t) 0;
			stats->rxFifo = (uint64_t) 0;
			stats->rxFrame = (uint64_t) 0;
			stats->rxCompressed = (uint64_t) 0;
			stats->rxMulticast = (uint64_t) 0;
			stats->txBytes = (uint64_t) 0;
			stats->txPackets = (uint64_t) 0;
			stats->txErrors = (uint64_t) 0;
			stats->txDrop = (uint64_t) 0;
			stats->txFifo = (uint64_t) 0;
			stats->txCollisions = (uint64_t) 0;
			stats->txCarrier = (uint64_t) 0;
			stats->txCompressed = (uint64_t) 0;
		}

        int j = 0;
        uint64_t *currentStat = &stats->rxBytes;
        for (char *tokens = line;; tokens = NULL, j++) {
            token = strtok_r(tokens, ": ", &tokenptr);
            if (token == NULL) {
                break;
            }

            if (j != 0) {
                //uint64_t prevStat = *currentStat;
                *currentStat = (uint64_t) strtoull(token, NULL, 10);
                currentStat++;
            } else {
				strncpy(stats->device, token, IFSTATS_NAME_LENGTH - 1);
			}
        }

        if (returnStats == NULL) {
            returnStats = stats;
        } else {
			ifstats_t *nextStats = returnStats;
			while (nextStats->next) {
				nextStats = nextStats->next;
			}
			nextStats->next = stats;
		}
    }

    // returnStats is NULL, empty stats
    if (returnStats == NULL) {
        LDEBUG("returnStats is NULL, empty stats");
        //LDEBUG("Freed address by pointer: [%p]", stats);
		free(stats);
        stats = NULL;
    }
	
    free(fileText);
    fileText = NULL;
    return returnStats;
}

//Function to release the stats objects obtained by the getIfStats function.
void release_ifstats(ifstats_t *stats) {
    ifstats_t *next = NULL;
    while (stats) {
        next = stats->next;
		//LDEBUG("Freed address by pointer: [%p]", stats);
        free(stats);
        stats = next;
    }
    return;
}

//Function to fetch delta(difference) between current and previous statistics.
ifstats_t *get_ifstats_delta(char *eth) {
    ifstats_t *scurr = get_ifstats(proc_net_dev_current, eth);
	ifstats_t *top_curr = scurr;
    if (scurr) {
		ifstats_t *curr_stats = scurr;
		ifstats_t *curr_next = NULL;
		while (curr_stats) {
			curr_next = curr_stats->next;
			LDEBUG("[CURRENT] Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] "
				   "rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
		     curr_stats->device, curr_stats->rxBytes, curr_stats->rxPackets, curr_stats->rxErrors, curr_stats->rxDrop,
		     curr_stats->txBytes, curr_stats->txPackets, curr_stats->txErrors, curr_stats->txDrop);
			curr_stats = curr_next;
		}
    }

    if (scurr && file_exist(proc_net_dev_previous)) {
        ifstats_t *sprev = get_ifstats(proc_net_dev_previous, eth);
		ifstats_t *top_prev = sprev;
        if (sprev) {
			ifstats_t *prev_stats = sprev;
		    ifstats_t *prev_next = NULL;
		    while (prev_stats) {
			   prev_next = prev_stats->next;
		       LDEBUG("[PREVIOUS] Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] "
                           "rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
                   prev_stats->device, prev_stats->rxBytes, prev_stats->rxPackets, prev_stats->rxErrors, prev_stats->rxDrop,
                   prev_stats->txBytes, prev_stats->txPackets, prev_stats->txErrors, prev_stats->txDrop);
			   prev_stats = prev_next;
			}
			
			// scurr->device match sprev->device
			while(scurr)
			{
				 // get device name in scurr
				sprev = top_prev;
				
                while(sprev)  // loop over sprev to find matching device name
				{
					if(strcmp(scurr->device, sprev->device) == 0)
					{
						// scurr = scurr - sprev
					   //LDEBUG("Match found !!");
                       if(scurr->rxBytes < sprev->rxBytes) {
                        LINFO("OVERFLOW occurred... scurr->rxBytes[%llu] sprev->rxBytes[%llu] ", scurr->rxBytes, sprev->rxBytes);
                       }
                       scurr->rxBytes = (scurr->rxBytes >= sprev->rxBytes)
                                        ? scurr->rxBytes - sprev->rxBytes
                                        : ULONG_MAX - sprev->rxBytes + scurr->rxBytes ;
                       scurr->rxPackets = (scurr->rxPackets >= sprev->rxPackets)
                                          ? scurr->rxPackets - sprev->rxPackets
                                          : ULONG_MAX - sprev->rxPackets + scurr->rxPackets;
                       scurr->rxErrors = (scurr->rxErrors >= sprev->rxErrors)
                                         ? scurr->rxErrors - sprev->rxErrors
                                         : ULONG_MAX - sprev->rxErrors + scurr->rxErrors;
                       scurr->rxDrop = (scurr->rxDrop >= sprev->rxDrop)
                                       ? scurr->rxDrop - sprev->rxDrop
                                       : ULONG_MAX - sprev->rxDrop + scurr->rxDrop;
                       scurr->rxFifo = (scurr->rxFifo >= sprev->rxFifo)
                                       ? scurr->rxFifo - sprev->rxFifo
                                       : ULONG_MAX - sprev->rxFifo + scurr->rxFifo;
                       scurr->rxFrame = (scurr->rxFrame >= sprev->rxFrame)
                                        ? scurr->rxFrame - sprev->rxFrame
                                        : ULONG_MAX - sprev->rxFrame + scurr->rxFrame;
                       scurr->rxCompressed = (scurr->rxCompressed >= sprev->rxCompressed)
                                             ? scurr->rxCompressed - sprev->rxCompressed
                                             : ULONG_MAX - sprev->rxCompressed + scurr->rxCompressed;
                       scurr->rxMulticast = (scurr->rxMulticast >= sprev->rxMulticast)
                                            ? scurr->rxMulticast - sprev->rxMulticast
                                            : ULONG_MAX - sprev->rxMulticast + scurr->rxMulticast;

                       if(scurr->txBytes < sprev->txBytes) {
                         LINFO("OVERFLOW occurred... scurr->txBytes[%llu] sprev->txBytes[%llu] ", scurr->txBytes, sprev->txBytes);
                       }
                       scurr->txBytes = (scurr->txBytes >= sprev->txBytes)
                                        ? scurr->txBytes - sprev->txBytes
                                        : ULONG_MAX - sprev->txBytes + scurr->txBytes;
                       scurr->txPackets = (scurr->txPackets >= sprev->txPackets)
                                          ? scurr->txPackets - sprev->txPackets
                                          : ULONG_MAX - sprev->txPackets + scurr->txPackets;
                       scurr->txErrors = (scurr->txErrors >= sprev->txErrors)
                                         ? scurr->txErrors - sprev->txErrors
                                         : ULONG_MAX - sprev->txErrors + scurr->txErrors;
                       scurr->txDrop = (scurr->txDrop >= sprev->txDrop)
                                       ? scurr->txDrop - sprev->txDrop
                                       : ULONG_MAX - sprev->txDrop + scurr->txDrop;
                       scurr->txFifo = (scurr->txFifo >= sprev->txFifo)
                                       ? scurr->txFifo - sprev->txFifo
                                       : ULONG_MAX - sprev->txFifo + scurr->txFifo;
                       scurr->txCollisions = (scurr->txCollisions >= sprev->txCollisions)
                                             ? scurr->txCollisions - sprev->txCollisions
                                             : ULONG_MAX - sprev->txCollisions + scurr->txCollisions;
                       scurr->txCarrier = (scurr->txCarrier >= sprev->txCarrier)
                                          ? scurr->txCarrier - sprev->txCarrier
                                          : ULONG_MAX - sprev->txCarrier + scurr->txCarrier;
                       scurr->txCompressed = (scurr->txCompressed >= sprev->txCompressed)
                                             ? scurr->txCompressed - sprev->txCompressed
                                             : ULONG_MAX - sprev->txCompressed + scurr->txCompressed;
                       LDEBUG("[DELTA] Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] "
                       "rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
                        scurr->device, scurr->rxBytes, scurr->rxPackets, scurr->rxErrors, scurr->rxDrop,
                        scurr->txBytes, scurr->txPackets, scurr->txErrors, scurr->txDrop);
					   //LDEBUG("Release sprev");
					   //release_ifstats(sprev);
					   break;
					}
					else{
						sprev = sprev->next;
					}
				}
			  
			 scurr = scurr->next;
			}
        }	
		//LDEBUG("Releasing sprev");
		//release_ifstats(sprev);
		LDEBUG("Releasing previous stats");
		release_ifstats(top_prev);
    }	
	
   scurr = top_curr;
   ifstats_t *stats = NULL;
   if(scurr)
   {  
       stats = (ifstats_t *) malloc(sizeof(ifstats_t));
       //LDEBUG("Allocated address by pointer: [%p]", stats);
       memset(stats, 0, sizeof(ifstats_t));
	   stats->rxBytes = (uint64_t) 0;
	   stats->rxPackets = (uint64_t) 0;
	   stats->rxErrors = (uint64_t) 0;
	   stats->rxDrop = (uint64_t) 0;
	   stats->rxFifo = (uint64_t) 0;
	   stats->rxFrame = (uint64_t) 0;
	   stats->rxCompressed = (uint64_t) 0;
	   stats->rxMulticast = (uint64_t) 0;
	   stats->txBytes = (uint64_t) 0;
	   stats->txPackets = (uint64_t) 0;
	   stats->txErrors = (uint64_t) 0;
	   stats->txDrop = (uint64_t) 0;
	   stats->txFifo = (uint64_t) 0;
	   stats->txCollisions = (uint64_t) 0;
	   stats->txCarrier = (uint64_t) 0;
	   stats->txCompressed = (uint64_t) 0;
       // LDEBUG("[DELTA] Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] "
       //                "rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
       //        scurr->device, scurr->rxBytes, scurr->rxPackets, scurr->rxErrors, scurr->rxDrop,
       //        scurr->txBytes, scurr->txPackets, scurr->txErrors, scurr->txDrop);
		
		//add all lists in scurr
	   strcpy(stats->device, eth);
    	while(scurr) {
			stats->rxBytes = stats->rxBytes + scurr->rxBytes;
			stats->rxPackets = stats->rxPackets + scurr->rxPackets;
			stats->rxErrors = stats->rxErrors + scurr->rxErrors;
			stats->rxDrop = stats->rxDrop + scurr->rxDrop;
			stats->rxFifo = stats->rxFifo + scurr->rxFifo;
			stats->rxFrame = stats->rxFrame + scurr->rxFrame;
			stats->rxCompressed = stats->rxCompressed + scurr->rxCompressed;
			stats->rxMulticast = stats->rxMulticast + scurr->rxMulticast;
			stats->txBytes = stats->txBytes + scurr->txBytes;
			stats->txPackets = stats->txPackets + scurr->txPackets;
			stats->txErrors = stats->txErrors + scurr->txErrors;
			stats->txDrop = stats->txDrop + scurr->txDrop;
			stats->txFifo = stats->txFifo + scurr->txFifo;
			stats->txCollisions = stats->txCollisions + scurr->txCollisions;
			stats->txCarrier = stats->txCarrier + scurr->txCarrier;
			stats->txCompressed = stats->txCompressed + scurr->txCompressed;
			
			scurr = scurr->next;
		   }
	  if(stats) {
		LDEBUG("[SUM] Device:[%s] rxBytes:[%llu] rxPackets:[%llu] rxErrors:[%llu] "
                       "rxDrop:[%llu] txBytes:[%llu] txPackets:[%llu] txErrors:[%llu] txDrop:[%llu]",
                        stats->device, stats->rxBytes, stats->rxPackets, stats->rxErrors, stats->rxDrop,
                        stats->txBytes, stats->txPackets, stats->txErrors, stats->txDrop);
    }   
   }	
  if(top_curr) {
    LDEBUG("Releasing current stats");
    release_ifstats(top_curr);
  }
  LDEBUG("Address of stats being returned: [%p]", stats);
  return stats;
}
 
int util_popen_with_output(char **output, int32_t *size, const char *command_str, ...) {
    errno = 0;
    *output = NULL;
    *size = 0;

    char *buf = NULL;
    FILE *devFile;
    size_t ret;
    char cmd[CMD_BUF_SIZE];
    memset(cmd, 0, sizeof(char) * CMD_BUF_SIZE);
    va_list ap;

    va_start(ap, command_str);
    vsprintf(cmd, command_str, ap);
    va_end(ap);

    int index = 0;
    int textSize = 64;
    char *filetext = (char *) malloc(sizeof(char) * textSize);

    if (!filetext) {
        return errno;
    }

    LDEBUG("executing command \"%s\"", cmd);
    devFile = popen(cmd, "r");

    if (!devFile) {
        return errno;
    }

    int c = 0;
    while ((c = fgetc(devFile)) != EOF) {
        filetext[index++] = c;
        if (index >= textSize) {
            textSize += 64;
            filetext = (char *) realloc(filetext, sizeof(char) * textSize);
            if (!filetext) {
                return errno;
            }
        }
    }

    fclose(devFile);

    filetext = (char *) realloc(filetext, sizeof(char) * (index + 1));
    if (!filetext) {
        return errno;
    }

    filetext[index] = '\0';
    *output = filetext;
    *size = index;

    LDEBUG("command output \"%s\"", filetext);
    return errno;
}

void util_popen(const char *c, ...) {
    char cmd[CMD_BUF_SIZE];
    memset(cmd, 0, sizeof(char) * CMD_BUF_SIZE);
    va_list ap;
    FILE *f;
    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    f = popen(cmd, "r");
    if (!f) {
        return;
    }

    fclose(f);
}

int util_system(const char *c, ...) {
    char cmd[CMD_BUF_SIZE];
    memset(cmd, 0, sizeof(char) * CMD_BUF_SIZE);
    va_list ap;

    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    return system(cmd);
}

long util_popen_with_output_long(const char *c, ...) {
    char cmd[CMD_BUF_SIZE];
    memset(cmd, 0, sizeof(char) * CMD_BUF_SIZE);
    va_list ap;
    va_start(ap, c);
    vsprintf(cmd, c, ap);
    va_end(ap);

    char *command_output = NULL;
    int32_t size = 0;
    int error = 0;
    long count = 0;
    error = util_popen_with_output(&command_output, &size, cmd);

    if (error != 0) {
        if (!command_output) {
            errno = error;
            LERROR("Unable to execute command \"%s\" error %d (%s)", cmd, errno, strerror(errno));
        }
    }

    if (command_output) {
        strip_newline_chars(command_output);
        count = (long) strtol(command_output, NULL, 10);
        free(command_output);
        command_output = NULL;
        size = 0;
        error = 0;
    } else {
        count = 0;
        LWARNING("command_output is NULL for command \"%s\" error %d (%s)", cmd, error, strerror(error));
    }

    return count;
}

/*Function to get uptime(Number of seconds the device has been up)
Returns long value. */
long get_uptime() {
    struct sysinfo s_info;
    int error = sysinfo(&s_info);
    if (error != 0) {
        LERROR("unable to get uptime error %d (%s)", errno, strerror(errno));
        return 0;
    }
    return s_info.uptime;
}

long util_popen_with_output_retry(const char *cmd_str) {
    char *command_output = NULL;
    int32_t size = 0;
    int error = 0;
    long count = 0;
    for (int retry = 0; retry < 5; retry++) {
        error = util_popen_with_output(&command_output, &size, cmd_str);
        if (error != 0) {
            if (!command_output) {
                errno = error;
                LERROR("Unable to execute command \"%s\" error %d (%s)", cmd_str, errno, strerror(errno));
            }
        }

        if (command_output) {
            count = 0;
            if (strstr(command_output, "SQL error") || strstr(command_output, "Error")) {
                LWARNING("found SQL error retrying cmd \"%s\"", cmd_str);
                free(command_output);
                command_output = NULL;
                size = 0;
                error = 0;
                sleep(1); //wait for a sec and then retry
                continue;
            }
            char *temp_str_ptr = command_output;
            while (*temp_str_ptr) if (*temp_str_ptr++ == '\n') ++count;
            free(command_output);
            command_output = NULL;
            size = 0;
            error = 0;
            break;
        } else {
            count = 0;
            LWARNING("command_output is NULL for command \"%s\" error %d (%s)", cmd_str, error, strerror(error));
        }
    }

    return count;
}

/*Function to get number of clients connected to wired LAN.
Returns long value*/
long get_lan_clients_count_LAN() {
    char *cmd_str = "sqlite3 /tmp/system.db 'SELECT * FROM lanhosts "
            "LEFT JOIN dot11STA ON dot11STA.macAddress = lanhosts.MACAddress "
            "WHERE dot11STA.macAddress IS NULL' 2>&1";
    return util_popen_with_output_retry(cmd_str);
}

//Function to get number of clients with IPv6 address.
long get_lan_clients_count_IPV6() {
    char *cmd_str = "sqlite3 /tmp/system.db 'select IPv6Address from lanhosts where IPv6Address<>\"\"' 2>&1";
    return util_popen_with_output_retry(cmd_str);
}

//Function to get number of clients connected to wireless LAN 2.4 GHz radio
long get_wlan_clients_count_WIFI24() {
    char *cmd_str = "wlanconfig ath0 list sta | wc -l";
    int count = util_popen_with_output_long(cmd_str);
    if (count <= 1) return 0;
    return count -1;
}

//Function to get number of clients connected to wireless LAN 5.0 GHz radio
long get_wlan_clients_count_WIFI50() {
    char *cmd_str = "wlanconfig ath1 list sta | wc -l";
    int count = util_popen_with_output_long(cmd_str);
    if (count <= 1) return 0;
    return count -1;
}

//Function to get channel used on 2.4 GHz radio
long get_channel_WIFI24() {
   // char *cmd_str = "wl -i wl0 channel | grep 'current mac channel' | awk '{print $4}'";
      char *cmd_str = "wl -i wl0 status | grep Channel | awk '{print $13}'";
    return util_popen_with_output_long(cmd_str);
}

//Function to get channel used on 5.0 GHz radio
long get_channel_WIFI50() {
    //char *cmd_str = "wl -i wl1 channel | grep 'current mac channel' | awk '{print $4}'";
    char *cmd_str = "wl -i wl1 status | grep Channel | awk '{print $13}'";
    return util_popen_with_output_long(cmd_str);
}

long _get_client_txrate(char *addr) {
	char *m = addr;
	char cmd_str[128] = {0};

	sprintf(cmd_str, "apstats -s -m %s | grep \"Average Tx Rate (kbps)\" | awk '{print $6}'", m);
	return util_popen_with_output_long(cmd_str);
}

/*Function to get signal strength information of clients connected on WLAN. 
Format of data {"mac": "string", "rssi": number, "mcs": number} */
client_info_t *get_client_info(char *interface) {
    char *command_output = NULL, cmd2 = NULL;
    int32_t size = 0;
    int error = 0;
    client_info_t *returnStats = NULL, *stats = NULL;
    char cmd_str[128], cmd2_str[128];

    sprintf(cmd_str, "wlanconfig %s list sta | sed \"1 d\"", interface);
        error = util_popen_with_output(&command_output, &size, cmd_str);
        if (error != 0) {
            if (!command_output) {
                errno = error;
                LERROR("Unable to execute command \"%s\" error %d (%s)", cmd_str, errno, strerror(errno));
            }
        }
  
        if (command_output) {

            char *line, *lineptr;
            for (char *lines = command_output;; lines = NULL) {
                line = strtok_r(lines, "\n", &lineptr);
                if (line == NULL) {
                    break;
                }

                stats = (client_info_t *) malloc(sizeof(client_info_t));
                if (!stats) {
                    error = errno;
                    release_info(returnStats);
                    errno = error;
                    return NULL;
                }

                memset(stats, 0, sizeof(client_info_t));
		char *token = strtok(line, " ");
		int token_num = 0;
		/*TWB EAP*/
		char phymode[32] = {0};
		int stream = 0;
		int item = 0;
                int rec;
		phy_mode_cap *temp = NULL;
		phy_mode_cap *phy = max_data_rate;
		/**/
		while(token != NULL) {
                    if (token_num == 0) {
	   	        strncpy(stats->device, token, MAC_ADDR_LENGTH - 1);
		    } else if (token_num == 5) {
                        stats->rssi_value = (-95 +  (long) strtol(token, NULL, 10));
		/*TWB EAP*/
 		    } else if (token_num == 3) { //TX Rate in wlanconfig but we use apstats instead
			stats->txrate_value = _get_client_txrate(stats->device);
                    }
#if 0
		    } else if (token_num == 21) {
			for (item = 0; item < sizeof(max_data_rate)/sizeof(max_data_rate[0]); ++item)
			{
			    if(!strcmp(phy[item].mode, token))
			    {
				temp = &phy[item];
				break;
			    }
			}
                    } else if (token_num == 23) {
			stream = (long) strtol(token, NULL, 10);
			stats->maxtxrate_value = temp->max_rate[stream-1];
                    }
#endif
                    else if (strstr(token, "IEEE80211") != NULL ) { /*QSDK 6.2.1 wlanconfig data format varies (wifi backhaul or ethernet)*/
                    
                       rec = token_num + 2;
                       for (item = 0; item < sizeof(max_data_rate)/sizeof(max_data_rate[0]); ++item)
                       {
                           if(!strcmp(phy[item].mode, token))
                           {
                               temp = &phy[item];
                               break;
                           }
                       }

                    } else if (token_num == rec) {
                        stream = (long) strtol(token, NULL, 10);
                        stats->maxtxrate_value = temp->max_rate[stream-1];
                    }
                    
                      
		/**/
		    //stats->mcs_value = 0;// TwinA needs fix
		    token_num++;
		    token = strtok(NULL, " ");
		}
		
                if (returnStats == NULL) {
                    returnStats = stats;
                } else {
                    client_info_t *current;
                    for (current = returnStats; current->next != NULL; current = current->next);
                    current->next = stats;
                }
            }

            free(command_output);
            command_output = NULL;
            size = 0;
            error = 0;
        } else {
            LWARNING("command_output is NULL for command \"%s\" error %d (%s)", cmd_str, error, strerror(error));
        }

    return returnStats;
}

void release_info(client_info_t *stats) {
    client_info_t *next = NULL;
    while (stats) {
        next = stats->next;
        free(stats);
        stats = next;
    }
    return;
}

//Function to create a backup of stats file.
void backup_stats_file() {
    LDEBUG("taking snap of [%s] to [%s]", proc_net_dev, proc_net_dev_current);
    remove(proc_net_dev_current);
    file_copy(proc_net_dev, proc_net_dev_current);
}

//Function to rename current stats file to previous stats file.
void rename_stats_file() {
    if (file_exist(proc_net_dev_current)) {
        LDEBUG("renaming [%s] to [%s]", proc_net_dev_current, proc_net_dev_previous);
        remove(proc_net_dev_previous);
        rename(proc_net_dev_current, proc_net_dev_previous);
    } else {
        LWARNING("file not found [%s]", proc_net_dev_current);
    }
}

/*TWB EAP*/
int get_mesh_parent_mac(char *get_str, int x)
{

    int ret;
    int dev = 0;
    char addr[32] = {0};
    hyd_wifison_dev son_dev[12] = {0};

    if((ret = ubox_get_qca_wifison_dev_num(&dev, 1)) != 0)
    {
        return -1;
    }
    else
    {
        ubox_get_qca_wifison_dev_topology(dev+1, son_dev, 0);
        strcpy(addr, son_dev[x].pmac);
        sprintf(get_str, "%s", addr);
        return 0;
    }
}

int get_mesh_lan_client(char *get_str, int x)
{
    int ret;
    char addr[32] = {0};
    
    ubox_get_qca_wifison_re_wired_client(addr, 0);
    sprintf(get_str, "%s", addr);
    return 0;
}
/*END*/

#ifdef __cplusplus
}
#endif
