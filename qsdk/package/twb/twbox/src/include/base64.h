#ifndef __BASE64_H_
#define __BASE64_H_

int Base64Decode(char* b64message, unsigned char** buffer, size_t* length);
int Base64Encode(const unsigned char* buffer, size_t length, char** b64text);

#endif
