#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int aes_ctr_encrypt(unsigned char *key, unsigned char *iv, unsigned char *pt, unsigned char **ct);
int aes_ctr_decrypt(unsigned char *key, unsigned char *iv, int ct_len, unsigned char *ct, unsigned char **pt);
