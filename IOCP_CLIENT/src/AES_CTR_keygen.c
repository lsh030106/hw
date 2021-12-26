#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

int gen_aes_ctr_key(unsigned char **key, unsigned char **iv);

/*int main(void) {
    unsigned char *key = NULL;
    unsigned char *iv = NULL;

    gen_aes_ctr_key(&key, &iv);

    printf("===========================================\n");
    printf("key length : %ld\n", strlen(key));
    printf("%s\n", key);

    printf("iv length : %ld\n", strlen(iv));
    printf("%s\n", iv);
    
    free(key);
    free(iv);
    return 0;
}*/


int gen_aes_ctr_key(unsigned char **key, unsigned char **iv) {
    int error;
    unsigned long err;

    const EVP_CIPHER *c = EVP_aes_128_ctr();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == NULL) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        return 0;

    }

    error = EVP_EncryptInit_ex(ctx, c, NULL, NULL, NULL);
    if (error == 0) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    *key = (unsigned char *)malloc(EVP_CIPHER_CTX_key_length(ctx));
    *iv = (unsigned char *)malloc(EVP_CIPHER_CTX_iv_length(ctx));

    if (key == NULL || iv == NULL) {
        fprintf(stderr, "%s\n", strerror(errno));
        
        if (key != NULL) {
            free(key);
        }

        return 0;
    }

    error = RAND_bytes(*key, EVP_CIPHER_CTX_key_length(ctx));
    if (error != 1) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        free(key);
        free(iv);
        return 0;
    }

    error = RAND_bytes(*iv, EVP_CIPHER_CTX_iv_length(ctx));
    if (error != 1) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        free(key);
        free(iv);
        return 0;
    }
    printf("KEY_length of bytes : %d\n", EVP_CIPHER_CTX_key_length(ctx)); 
    printf("%s\n", *key);

    printf("IV_length of bytes : %d\n", EVP_CIPHER_CTX_iv_length(ctx));
    printf("%s\n", *iv);

    EVP_CIPHER_CTX_free(ctx);
    return 1;
}
