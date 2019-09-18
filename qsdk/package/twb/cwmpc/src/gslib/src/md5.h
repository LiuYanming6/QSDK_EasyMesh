#ifndef MD5_H
#define MD5_H

#define MD5_DIGEST_LEN     16

typedef struct MD5Context {
  u_int32_t buf[4];
  u_int32_t bits[2];
  u_char in[64];
} MD5Context;

// the following were changed to static functions within the md5.c source
// for the CWMPc usage.
//void MD5Init(MD5Context *ctx);
//void MD5Update(MD5Context *ctx, u_char const *buf, u_int len);
//void MD5Final(u_char digest[16], MD5Context *ctx);
void tr69_md5it(unsigned char *out, const unsigned char *in);

#endif
