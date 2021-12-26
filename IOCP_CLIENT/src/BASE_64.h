#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int base64Encode(const char *msg, char **buffer);
int calcDecodeLength(const char *b64input);
int base64Decode(char *b64msg, char **buffer);

