/*
 * auxproto.c
 *
 *  Created on: Aug 7, 2009
 *      Author: dmounday
 */


/*----------------------------------------------------------------------*/
void proto_ParsePost(tProtoCtx *pc, tHttpHdrs *hdrs)
{
	char *data;
	size_t n;
	int len;

#ifdef DEBUG
	fprintf(stderr, "DEBUG: proto_ParsePost() to read %d bytes\n", hdrs->content_length);
#endif
	len = hdrs->content_length;
	data = (char *) GS_MALLOC(len + 1); /* make room for terminating '\0' */
	if ( data ){
		n = proto_Readn(pc, data, len);
		if (n>0)
			data[n] = '\0';
		else
			data[0]	= '\n';
	#ifdef DEBUG
		fprintf(stderr, "DEBUG: proto_ParsePost() read %d bytes \"%s\"\n", n, data);
	#endif
	}
	GS_FREE(hdrs->arg);
	hdrs->arg = data;

	proto_Skip(pc);
}
