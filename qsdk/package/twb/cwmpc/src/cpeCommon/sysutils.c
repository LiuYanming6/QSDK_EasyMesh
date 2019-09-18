/*----------------------------------------------------------------------*
 * Gatespace
 * Copyright 2008 Gatespace Networks, Inc., All Rights Reserved.
 * Gatespace Networks, Inc. confidential material.
 *----------------------------------------------------------------------*
 * File Name  :
 * Description:
 *----------------------------------------------------------------------*
 * $Revision: 1.4 $
 *
 * $Id: sysutils.c,v 1.4 2012/05/10 17:38:01 dmounday Exp $
 *----------------------------------------------------------------------*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <ctype.h>
#include <syslog.h>
#include "../includes/CPEWrapper.h"


/*
 * *
 * findProc by using ps -ax
 */
unsigned findProc(const char *pattern) {
	FILE* 	fp;
	char	buf[256];
	int i;
	unsigned pid = -1;
	system("ps -ax > /tmp/process.tmp");
	if ( (fp = fopen("/tmp/process.tmp", "r")) ){
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			if (strstr(buf, pattern)!= NULL) { /* found pattern */
				if (strstr(buf, "Z") != NULL) { /* not dead */
					i = 0;
					while (buf[i]==' ')
						++i; /* skip blanks */
					pid = atoi(&buf[i]);
					break;
				}
			}

		}
		fclose(fp);
		unlink("/tmp/process.tmp");
	}
	return pid;
}

/*--------------------*
 * returns
 *   <pid> for str
 *   -1    if process not found
 */
#if 0
int findProc(const char* str) {
	DIR* d;
	struct dirent *de;
	char path[64];
	char cmd[256];
	int n;

	if ((d = opendir("/proc")) != NULL) {
		while ((de = readdir(d))) {
			if (! isdigit(*de->d_name)) {
				continue;
			}
			sprintf(path, "/proc/%s/exe", de->d_name);
			n = readlink(path, cmd, 256);
			cmd[n] = '\0';
			/*#ifdef DEBUG
			 cpeLog(LOG_DEBUG, "findProc() pid=%s cmd=\"%s\" str=\"%s\" match=%d",
			 de->d_name, cmd, str, strcmp(cmd,str) == 0);
			 #endif */
			if (strcmp(cmd, str) == 0) {
				int pid = atoi(de->d_name);
				closedir(d);
#ifdef DEBUG
				cpeLog(LOG_DEBUG, "findProc(%s) pid=%d", str, pid);
#endif
				return pid;
			}
		}
		closedir(d);
	}
#ifdef DEBUG
	cpeLog(LOG_DEBUG, "findProc(%s) not found", str);
#endif
	return -1;
}
#endif

void signalDaemon(const char *name, int sig)
{
        int pid;

        pid = findProc(name);
        if (pid > 0) {
                kill(pid, sig);
                cpeLog(LOG_INFO,  "signaled \"%s\" with %d", name, sig);
        } else {
                cpeLog(LOG_DEBUG, "signalDaemon did not find: \"%s\" (ignore)", name);
        }

}

/*--------------------*/
void stopDaemon(const char* name) {
	int pid;

	pid = findProc(name);
	if (pid > 0) {
		kill(pid, SIGINT);
		kill(pid, SIGTERM);
		cpeLog(LOG_INFO, "stopped daemon: \"%s\"", name);
	} else {
		cpeLog(LOG_DEBUG, "stopDaemon did not find: \"%s\" (ignore)",
				name);
	}
}

/*--------------------*/

void killDaemon(const char* name) {
	int pid;

	pid = findProc(name);
	if (pid > 0) {
		kill(pid, SIGKILL);
		cpeLog(LOG_INFO,  "killed daemon: \"%s\"", name);
	} else {
		cpeLog(LOG_DEBUG, "killDaemon did not find: \"%s\" (ignore)",
				name);
	}
}
int do_cmd(int logon, const char *cmd, char *fmt, ...)
{
  va_list ap;
  char msg[1024];
  char *p;
  int  stat;

  strncpy(msg, cmd, sizeof msg);
  p = msg + strlen(cmd);
  *p++ = ' ';

  va_start(ap, fmt);
  vsnprintf(p, sizeof msg - (p-msg), fmt, ap);
  va_end(ap);
  stat = system(msg);
  if (logon)
      cpeLog(LOG_INFO, "system(\"%s\") (%d)", msg, stat);
  return stat;
}
/*
 * Scan the file for a line starting with the prefix string if it
 * is present and then return a pointer to the space delimited field
 * identified by field. If a prefix is specified the field is 1..n.
 * If no prefix the field numbering starts at 0.
 */
char *cpeReadFileField( const char *fname, const char *prefix, int field){
	FILE *f;
	static char  line[256];
	char	*p = NULL;
	int		fld = 0;
	char	*lasts;

	if ((f=fopen(fname, "r"))) {
		while ( (fgets( line, sizeof(line), f)!= NULL) ) {
			p = line;
			while ( *p == ' ') p++;		/* strip leading spaces */
			if ( prefix!=NULL ){
				/* qualify this line */
				if ( strncmp( p, prefix, strlen(prefix) ) != 0)
					continue; /* skip this line */
				else {
					/* restart scan immediately following prefix string */
					p += strlen(prefix);
					fld = 1;
				}
			}
			p = strtok_r( p, " :", &lasts);
			while ( p ){
				if ( fld==field ){
					break;
				}
				++fld;
				p = strtok_r(NULL, " ", &lasts);
			}
			if ( fld==field)
				break;
		}
		fclose(f);
	}
	return p;
}

