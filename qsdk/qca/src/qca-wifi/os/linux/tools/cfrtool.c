/*
 * Copyright (c) 2013,2017-2018 Qualcomm Innovation Center, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Innovation Center, Inc.
 *
 * 2013 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 *
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 */

#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <linux/types.h>
#include <stdio.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#ifndef _BYTE_ORDER
#include <endian.h>
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define _BYTE_ORDER _LITTLE_ENDIAN
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
#define _BYTE_ORDER _BIG_ENDIAN
#endif
#endif  /* _BYTE_ORDER */

/* Enable compilation of code referencing SO_RCVBUFFORCE even on systems where
 * this isn't available. We should be able to determine availability at runtime.
 */
#ifndef SO_RCVBUFFORCE
#define SO_RCVBUFFORCE                      (33)
#endif

/* Netlink timeout specification (second and microsecond components) */
#define QCA_CFRTOOL_NL_TIMEOUT_SEC         (3)
#define QCA_CFRTOOL_NL_TIMEOUT_USEC        (0)

#define streq(a,b)      (strcasecmp(a,b) == 0)

#define MAX_PAYLOAD            1024  /* maximum payload size*/
#ifndef NETLINK_CFR_FAMILY
#define NETLINK_CFR_FAMILY     19
#endif
#define SAMPRECVBUF_SZ         (4096)
#define MAX_FILE_SIZE          (8 * 1024 * 1024)

struct cfr_app_payload_header {
    u_int8_t    phynetdevidx;
    u_int32_t   seqno;
    u_int16_t   fragno;
} __attribute__ ((__packed__));

/*
 * Function    : cfr_capture_data
 * Description : Capture cfr data from the hardware FFT engine.
 * Input       : As of now left the input scope open.
 * Output      : Expected File that contains the cfr data captured.
 */
static int cfr_capture_data(bool daemonize)
{
    int ret = 0, fs_ret = 0;
    struct sockaddr_nl src_addr, dest_addr;
    socklen_t fromlen;
    struct nlmsghdr *nlh = NULL;
    struct cfr_app_payload_header *cfr_app_hdr = NULL;
    int sock_fd = -1, read_bytes = 0;
    u_int8_t *cfr_app_data = NULL;          /* considering app data as byte stream from app standpoint */
    u_int32_t nlmsg_data_len, nlmsg_hdr_len;
    u_int32_t cfr_app_hdr_len, cfr_app_data_len, wlen;
    bool enobuf = 0;
    u_int8_t *samprecvbuf = NULL;
    FILE *fp = NULL, *fp0 = NULL, *fp1 = NULL, *fp2 = NULL;
    
    u_int16_t num_rbuff_errors = 0;

    /* SO_RCVBUF/SO_RCVBUFFORCE expect receive buffer sizes as integer
     * values.
     */
    int rbuff_sz_req = 0;            /* Receive buffer size to be requested */
    int rbuff_sz_curr = 0;           /* Current receive buffer size */
    socklen_t rbuff_sz_curr_len = 0; /* Length of current receive buffer size
                                        datatype */
    struct timeval tv_timeout;
    fd_set readfds;


    sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_CFR_FAMILY);
    if (sock_fd < 0) {
        perror("socket\n");
        ret = sock_fd;
        goto out;
    }

    /* On some platforms and under some circumstances, our netlink message
     * receive rate may not be able to keep up with the driver's send rate. This
     * can result in receive buffer errors.
     * To mitigate this, we increase the socket receive buffer size.
     *
     * An alternative considered is to have two threads, one purely for socket
     * receive operations, the other for processing the received information.
     * However, test results partially emulating this scenario showed that even
     * with this, we can run into the receive buffer errors (due to the high
     * rate at which the netlink messages arrive).
     */

    /* Get current receive buffer size */
    rbuff_sz_curr_len = sizeof(rbuff_sz_curr);
    if ((ret = getsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF,
                   (void *)&rbuff_sz_curr,
                   &rbuff_sz_curr_len)) < 0) {
            perror("getsockopt\n");
            goto out;
    }

   printf("Socket recv buffer curr len:%d\n",rbuff_sz_curr);

    /* Determine the receive buffer size to be requested - taken to be 2M as default? */
    rbuff_sz_req = SAMPRECVBUF_SZ * sizeof(u_int8_t) * 1024;

   printf("Socket recv buffer req len:%d\n",rbuff_sz_req);

    if (rbuff_sz_req > rbuff_sz_curr)
    {
        /* We first try SO_RCVBUFFORCE. This is available since Linux 2.6.14,
         * and if we have CAP_NET_ADMIN privileges.
         *
         * In case SO_RCVBUFFORCE is not available or we are not entitled to use
         * it, then an error will be returned and we can fall back to SO_RCVBUF.
         * If we use SO_RCVBUF, the kernel will cap our requested value as per
         * rmem_max. We will have to survive with the possibility of a few
         * netlink messages being lost under some circumstances.
         */
        ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUFFORCE,
                            (void *)&rbuff_sz_req, sizeof(rbuff_sz_req));

        if (ret < 0)
        {
            if ((ret = setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF,
                             (void *)&rbuff_sz_req, sizeof(rbuff_sz_req))) < 0) {
                    perror("setsockopt\n");
                    goto out;
            }
        }
    }
    /* Else if rbuff_sz_req < rbuff_sz_curr, we go with the default configured
     * into the kernel. We will have to survive with the possibility of a few
     * netlink messages being lost under some circumstances.
     */

    fp0 = fopen("/tmp/cfr_wifi0", "wb+");
    if (!fp0) {
        perror("fopen\n");
        printf("Could not open cfr_wifi0 file to write\n");
        ret = -1;
        goto out;
    }

    fp1 = fopen("/tmp/cfr_wifi1", "wb+");
    if (!fp1) {
        perror("fopen\n");
        printf("Could not open cfr_wifi1 to write\n");
        ret = -1;
        goto out;
    }

    fp2 = fopen("/tmp/cfr_wifi2", "wb+");
    if (!fp2) {
        perror("fopen\n");
        printf("Could not open cfr_wifi2 to write\n");
        ret = -1;
        goto out;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = PF_NETLINK;
    src_addr.nl_pid = getpid();        /* self pid */
    src_addr.nl_groups = 1;

    read_bytes=bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
        if (read_bytes < 0) {
            perror("bind(netlink)");
        printf("BIND errno=%d\n", read_bytes);
        ret = read_bytes;
        goto out;
    }

    samprecvbuf = (u_int8_t *)malloc(SAMPRECVBUF_SZ * sizeof(u_int8_t));
    if (samprecvbuf == NULL) {
        printf("Could not allocate buffer to receive CFR data\n");
        ret = -1;
        goto out;
    }
    memset(samprecvbuf, 0, SAMPRECVBUF_SZ * sizeof(u_int8_t));

    printf("Waiting for message from kernel\n");

    if (daemonize) {
        if (daemon(0,1) < 0)
            perror("error daemonizing\n");
    }

    while (1) {

        FD_ZERO(&readfds);
        FD_SET(sock_fd, &readfds);

        tv_timeout.tv_sec = QCA_CFRTOOL_NL_TIMEOUT_SEC;
        tv_timeout.tv_usec = QCA_CFRTOOL_NL_TIMEOUT_USEC;

        ret = select(sock_fd + 1, &readfds, NULL, NULL, &tv_timeout);

        if (ret < 0) {
            perror("select\n");
            goto out;
        } else if (0 == ret) {
            printf("Warning - timed out waiting for messages.\n");
            continue;
        } else if (!FD_ISSET(sock_fd, &readfds)) {
            /* This shouldn't happen if the kernel is behaving correctly. */
            fprintf(stderr, "Unexpected condition waiting for messages - no "
                    "socket fd indicated by select()\n");
            ret = -1;
            goto out;
        }

        fromlen = sizeof(src_addr);
        read_bytes = recvfrom(sock_fd, samprecvbuf,
                              SAMPRECVBUF_SZ * sizeof(u_int8_t), MSG_WAITALL,
                              (struct sockaddr *) &src_addr, &fromlen);
        if (read_bytes < 0) {
            if (ENOBUFS == errno)
            {
                num_rbuff_errors++;
                enobuf = 1;
            } else {
                perror("recvfrom(netlink)\n");
                printf("Error reading netlink\n");
                ret = read_bytes;
                goto out;
            }
        } else {

            nlh = (struct nlmsghdr *) samprecvbuf;
            nlmsg_hdr_len = sizeof(struct nlmsghdr);
            nlmsg_data_len = NLMSG_PAYLOAD(nlh, 0);

            if (read_bytes != NLMSG_SPACE(nlmsg_data_len)){
                printf("\n\n\n");        
                printf("FATAL!! read_bytes=%d nlmsg_len=%d nlmsg_data_len=%d\n",read_bytes,nlh->nlmsg_len,nlmsg_data_len);
                printf("\n\n\n");
                ret = -1;
                goto out;
            }        

            cfr_app_hdr = (struct cfr_app_payload_header *) NLMSG_DATA(nlh);
            cfr_app_hdr_len = sizeof(struct cfr_app_payload_header);
            cfr_app_data =  (u_int8_t *) cfr_app_hdr + cfr_app_hdr_len;
            cfr_app_data_len = nlmsg_data_len - cfr_app_hdr_len;

            switch (cfr_app_hdr->phynetdevidx) {
                case 0:
                    fp = fp0;
                break;
                case 1:
                    fp = fp1;
                break;
                case 2:
                    fp = fp2;
                break;
                default:
                    printf("Invalid phy netdevindex:%d\n",cfr_app_hdr->phynetdevidx);
                    ret = -1;
                    goto out;
            }

            if (ftell(fp) + cfr_app_data_len > MAX_FILE_SIZE) {
                fs_ret = fseek(fp, 0, SEEK_SET);
                if (fs_ret < 0) {
                    perror("fseek\n");
                    printf("fseek failed with %d\n",fs_ret);
                    ret = -1;
                    goto out;
                }
            }

	    wlen = fwrite(cfr_app_data, 1, cfr_app_data_len, fp);
	    if (wlen != cfr_app_data_len) {
		printf("Warning: %u bytes of %u written",wlen,cfr_app_data_len);
		ret = -1;
		goto out;
	    }

            fflush(fp);
            enobuf = 0;
        }

        if (enobuf)
        {
            printf("Warning: %hu receive buffer errors. Some samples were lost due"
               "to receive-rate constraints\n", num_rbuff_errors);
        }
    }


out:
    if (sock_fd >= 0) {
        close(sock_fd);
    }

    if (fp0 != NULL) {
        fclose(fp0);
    }

    if (fp1 != NULL) {
        fclose(fp1);
    }

    if (fp2 != NULL) {
        fclose(fp2);
    }

    if (samprecvbuf != NULL) {
        free(samprecvbuf);
    }

    return ret;
}

static void
usage(void)
{
	const char *msg = "\
Usage: cfrtool [cmd]\n\
           <cmd> = capture <-d> (to daemonize)\n\
           <cmd> = -h : print this usage message\n";

	fprintf(stderr, "%s", msg);
}

int
main(int argc, char *argv[])
{
        if (argc > 1 && streq(argv[1],"-h")) {
            usage();
        } else if (argc == 2 && streq(argv[1],"capture")) {
            return cfr_capture_data(0);
        } else if (argc == 3 && streq(argv[1],"capture") && streq(argv[2],"-d")) {
            return cfr_capture_data(1);
        } else {
            fprintf(stderr, "Invalid command option used for cfrtool\n");
            usage();
        }

	return 0;
}

