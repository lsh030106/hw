#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

void closefiles(void);
int openfiles(char *);
int writefiles(RSA *);
void closefiles(void);
RSA *gen_key(int);
int make_key(char *keyname);
char *RSA_pubkey_read(const char *filename, char *buffer);
char *RSA_pubkey_write(const char *filename, char *buffer, int len);
