#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

/*
 generate aes-128-ecb key ./secretkey.key
 function returns 1 on success, 0 for error;
 */

int gen_aes_key(void);
