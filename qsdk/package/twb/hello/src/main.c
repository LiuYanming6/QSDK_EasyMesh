/**
 *
 * Copyright Copyright 2018-2019, twinA Corporation
 * All Rights Reserved.
 * 
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    system("echo \"Hello World!\" > /dev/console");

    system("/sbin/uboot_upgrade");
    system("mkdir -p /etc/keep");
//    system("state_preinit");
    system("insmod /lib/gpio-keys.ko");
    system("sysledd &");
    sleep(1);
    system("syshotplugd &");
    system("cli_factory &");
    sleep(1);
    // N: cli_factory will be killed by procmanager
    // F: syshotplugd will be killed by procmanager
    system("procmanager &");
	return 0;
}
