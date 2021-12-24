#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include "AES_CTR_keygen.h"

int aes_ctr_encrypt(unsigned char *key, unsigned char *iv, unsigned char *pt, unsigned char **ct);
int aes_ctr_decrypt(unsigned char *key, unsigned char *iv, unsigned char *ct, unsigned char **pt);

int main(void) {
    unsigned char *key;
    unsigned char *iv;

    gen_aes_ctr_key(&key, &iv);


    char plaint[] = "test ctr test test ctr test";
    //char *plain = plaint;
    char *cipher;
    char *pt;

    aes_ctr_encrypt(key, iv, (unsigned char *)plaint, (unsigned char **)&cipher);
    aes_ctr_decrypt(key, iv, (unsigned char *)cipher, (unsigned char **)&pt);
    return 0;
}

int aes_ctr_encrypt(unsigned char *key, unsigned char *iv, unsigned char *pt, unsigned char **ct) {
    int error;
    int ct_len = 0; 
    int num_bytes = 0;
    unsigned long err;
    printf("%d\n", (int)strlen((char *)pt));
    printf("hear\n");
    int pt_len = (int)strlen((char *)pt);

    const EVP_CIPHER *c = EVP_aes_128_ctr();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == NULL) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        return 0;
    }

    error = EVP_EncryptInit_ex(ctx, c, NULL, key, iv);
    if (error == 0) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    
    *ct = (unsigned char *)malloc(pt_len + EVP_CIPHER_CTX_block_size(ctx));

    error = EVP_EncryptUpdate(ctx, ct[ct_len], &num_bytes, &pt[ct_len], 
            pt_len);
    if (error == 0) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    printf("hear\n");

    printf("%s\n", *ct);
    printf("%d\n", num_bytes);
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}

int aes_ctr_decrypt(unsigned char *key, unsigned char *iv, unsigned char *ct, unsigned char **pt) {
    int error;
    int pt_len = 0;
    int num_bytes = 0;
    unsigned long err;

    const EVP_CIPHER *c = EVP_aes_128_ctr();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == NULL) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        return 0;
    }

    error = EVP_DecryptInit_ex(ctx, c, NULL, key, iv);
    if (error == 0) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    
    *pt = (unsigned char *)malloc((int)strlen((char *)ct) + EVP_CIPHER_CTX_block_size(ctx) + 1);

    error = EVP_DecryptUpdate(ctx, pt[pt_len], &num_bytes, &ct[pt_len],
            (int)strlen((char *)ct));
    if (error == 0) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    *pt[num_bytes] = '\0';
    
    EVP_CIPHER_CTX_free(ctx);
    printf("cipher len : %d\n", (int)strlen((char *)ct));
    printf("numbyte : %d\n", num_bytes);
    printf("pt  : %s\n", *pt);
    return 1;    
}

