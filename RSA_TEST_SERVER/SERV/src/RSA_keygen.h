#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

BIO *publickey_bio = NULL;
BIO *privatekey_bio = NULL;
BIO *stdout_bio = NULL;

void closefiles(void);
int openfiles(char *);
int writefiles(RSA *);
void closefiles(void);
RSA *gen_key(int);
int make_key(char *keyname);
