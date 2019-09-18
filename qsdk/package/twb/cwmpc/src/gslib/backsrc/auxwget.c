/*
 * auxwget.c
 *
 *  Created on: Aug 7, 2009
 *      Author: dmounday
 */


/*----------------------------------------------------------------------*
 * wget_GetFile
 *----------------------------------------------------------------------*/
typedef struct {
    EventHandler cb;
    void *handle;
    char *filename;
} tWgetFile;

/*----------------------------------------------------------------------*
 * returns
 *    0 Ok
 *   -1 File could not be open for writing
 *   -2 Error when reading from fd
 *   -3 Error when writing to file
 */
static int copy_to_file(tProtoCtx *pc, const char *filename) {
    int ofd;
    char buf[BUF_SIZE];
    int rlen, wlen;
    mode_t mode;
    int flags, bflags;

    /* 664 */
    mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;
    if ((ofd = creat(filename, mode)) < 0) {
        return -1;
    }

    /* TBD: Temporary fix part 1, turn on synchroneous I/O, this call will block. */
    {
        flags = (long) fcntl(pc->fd, F_GETFL);
        bflags = flags & ~O_NONBLOCK; /* clear non-block flag, i.e. block */
        fcntl(pc->fd, F_SETFL, bflags);
    }

    while ((rlen = proto_Readn(pc, buf, BUF_SIZE)) != 0) {
        if (rlen < 0) {
            if (errno == EAGAIN) {
#ifdef DEBUG
                fprintf(stderr, "wget, data not ready, let's busy wait\n");
#endif
continue;
            }
            /* error when reading from fd */
            return -2;
        } else if (rlen > 0) {
            /* Bytes read */
            wlen = write(ofd, buf, rlen);
            if (wlen != rlen) {
                /* error on writing */
                return -3;
            }
        }
    }

    /* TBD: Temp fix part 2. remove blocking flags */
    {
        fcntl(pc->fd, F_SETFL, flags);
    }
    /* EOF */
    close(ofd);
    return 0;
}

/*----------------------------------------------------------------------
 * argument of callback is of type (tWget *)
 */
static void save_to_file(void *p) {
    tWgetFile *wf;
    tWget *w;
    tWget cbw;
    int res;

#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file()...\n");
#endif
w = (tWget *) p;
wf = (tWgetFile *) w->handle;

memset(&cbw, 0, sizeof(tWget));

if (w->status != iWgetStatus_Ok) {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file, status NOT ok (%d)\n", w->status);
#endif
cbw.status = iWgetStatus_InternalError;
} else if ((res = copy_to_file(w->pc, wf->filename)) < 0) {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file, copy_to_file failed (%d), status NOT ok (%d)\n",
    res, w->status);
#endif
cbw.status = iWgetStatus_InternalError;
} else {
#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file, status OK (%d)\n", w->status);
#endif
cbw.status = iWgetStatus_Ok;
}

/* call callback */
cbw.msg = w->msg;
cbw.pc = w->pc;
cbw.hdrs = w->hdrs;
cbw.handle = wf->handle;
#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file() calling callback = %p...\n", (void *) wf->cb);
#endif
(*wf->cb)(&cbw);

GS_FREE(wf->filename);
GS_FREE(wf);
#ifdef DEBUG
    fprintf(stderr, "DEBUG: wget.save_to_file() done.\n");
#endif
}

/*----------------------------------------------------------------------*
 * returns
 *    0  Ok
 *   -1 to -5 same error codes as wget_Get
 * set status in callback to
 *    0 Ok
 *   -1 Could not open file for writing
 */
int wget_GetFile(const char *url, const char *filename, EventHandler callback, void *handle) {
    int res = -1;
    tWgetFile *wf;

    if ( (wf = (tWgetFile *) GS_MALLOC(sizeof(tWgetFile)))){
		memset(wf, 0, sizeof(tWgetFile));
		wf->cb = callback;
		wf->handle = handle;
		wf->filename = GS_STRDUP(filename);
		if ( wf->filename )
			res = wget_Get(url, save_to_file, wf);
		if (res < 0) {
			GS_FREE(wf->filename);
			GS_FREE(wf);
			return res;
		}
    }
    return 0;
}

/*----------------------------------------------------------------------*
 * returns
 *    0  Ok
 *   -1 to -5 same error codes as wget_Get
 * set status in callback to
 *    0 Ok
 *   -1 Could not open file for writing
 */
int wget_PostXmlFile(const char *url, char *data, int len, const char *filename, EventHandler callback, void *handle) {
    int res = -1;
    tWgetFile *wf;

    if ( (wf = (tWgetFile *) GS_MALLOC(sizeof(tWgetFile))) ){
		memset(wf, 0, sizeof(tWgetFile));
		wf->cb = callback;
		wf->handle = handle;
		wf->filename = GS_STRDUP(filename);
		if ( wf->filename)
			res = wget_PostRaw(url, "text/xml", data, len, save_to_file, wf);
		if (res < 0) {
			GS_FREE(wf->filename);
			GS_FREE(wf);
			return res;
		}
    }
    return 0;
}

/*----------------------------------------------------------------------*
 * returns
 *    0  Ok
 *   -1 to -5 same error codes as wget_Get
 * set status in callback to
 *    0 Ok
 *   -1 Could not open file for writing
 */
int wget_PostXml(const char *url, char *data, int len, EventHandler callback, void *handle) {
    return do_wget(url, callback, handle, data, len, "text/xml");
}

/*----------------------------------------------------------------------*/
int do_wget(const char *url, EventHandler callback, void *handle, char *postdata, int datalen, char *content_type) {
    int port;
    tWgetInternal *data;

#ifdef DEBUG
    cpeLog(LOG_DEBUG, "do_wget(\"%s\", ...)", url);
#endif
	if (www_ParseUrl(url, wg->proto, wg->host, &port, wg->uri) < 0) {
	    return -5;
	}

	if (port == 0) {
	    if (strcmp(proto, "http") == 0) {
	        port = 80;
	    #ifdef USE_SSL
	    } else if (strcmp(proto, "https") == 0) {
	        port = 443;
	    #endif
	    } else {
	        cpeLog(LOG_ERROR, "unsupported protocol in url \"%s\"", proto);
	        port = 80; /* guess http and port 80 */
	    }
	}
	if ( (data = (tWgetInternal *) GS_MALLOC(sizeof(tWgetInternal)))){
	    memset(data, 0, sizeof(tWgetInternal));
	    data->keepConnection = eCloseConnection;
	    data->pc = NULL;
	    data->cb = callback;
	    data->handle = handle;
	    data->port = port;
	    data->content_type = content_type;
	    data->postdata = postdata;
	    data->request = postdata? ePostData: eGetData;
	    data->datalen = datalen;

	    if (dns_lookup(data->host, SOCK_STREAM, AF_UNSPEC, &(data->host_addr))) {
			cpeLog(LOG_DEBUG, "Host IP address: %s", writeInIPAddr(&data->host_addr));
	        do_resolve(data);
	    } else {
	        setCallback(&(data->host_addr), do_resolve, data);
	    }
	    return 0;
	}

	return -10;
}

/*----------------------------------------------------------------------*
 * return
 *   0 Ok
 *  -1 Could not create socket
 *  -2 Could not connect to host
 *  -3 Could not resolve host or host syntax error
 *  -4 WAN interface is not active
 *  -5 URL syntax error
 *  -10 unknown error
 */
int wget_Get(const char *url, EventHandler callback, void *handle) {
    return do_wget(url, callback, handle, NULL, 0, NULL);
}

/*----------------------------------------------------------------------*
 * return
 *   0  Ok
 *  -1  Could not create socket
 *  -2  Could not connect to host
 *  -3  Could not resolve host or host syntax error
 *  -4  WAN interface is not active
 *  -5  URL syntax error
 *  -10 unknown error
 */
int wget_Post(const char *url, char *arg_keys[], char *arg_values[],
		EventHandler callback, void *handle) {
	char *argbuf;
	int arglen;
	int i;
	char buf[BUF_SIZE];
	int blth;

	argbuf = (char *) GS_MALLOC(BUF_SIZE * 4);
	if (argbuf == NULL) {
		return -10;
	}

	blth = BUF_SIZE*4;
	/* build arguments */
#if 0
	fprintf(stderr, "DEBUG: wget_Post()\n");
	cvprint(arg_keys);
	cvprint(arg_values);
#endif
	strcpy(argbuf, "");
	for (i = 0; arg_keys[i] != NULL; i++) {
		if (i > 0) {
			strcat(argbuf, "&");
		}
		www_UrlEncode(arg_values[i], buf);
		blth = strlen(argbuf);
		snprintf(&argbuf[blth], (BUF_SIZE*4)-blth, "%s=%s", arg_keys[i], buf);
	}
	arglen = strlen(argbuf);

	return do_wget(url, callback, handle, argbuf, arglen,
			"application/x-www-form-urlencoded");
}

/*----------------------------------------------------------------------*
 * return
 *   0  Ok
 *  -1  Could not create socket
 *  -2  Could not connect to host
 *  -3  Could not resolve host or host syntax error
 *  -4  WAN interface is not active
 *  -5  URL syntax error
 *  -10 unknown error
 */
int wget_PostRaw(const char *url, char *content_type, char *data, int len, EventHandler callback, void *handle) {
    return do_wget(url, callback, handle, data, len, content_type);
}
