#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define KEYLEN 256

int public_enc(unsigned char *msg, unsigned char *Ct, const char *filename);
int private_dec(unsigned char *Ct, unsigned char *Pt, const char *filename);
