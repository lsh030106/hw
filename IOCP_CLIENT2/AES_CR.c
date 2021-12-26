#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "src/AES_CTR_keygen.h"

unsigned char * symkey(unsigned char *key, unsigned char *iv, int flag, unsigned char *txt, 
        unsigned char *ct);

unsigned char * symkey(unsigned char *key, unsigned char *iv, int flag, unsigned char *txt, 
        unsigned char *ct) {
    int ctlen = 0; 
    int ptlen = 0;
    int num_byte = 0;
    unsigned long err = 0;
    int error = 0;

    const EVP_CIPHER *c = EVP_aes_128_ofb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == NULL) {
        //error;
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    EVP_EncryptInit_ex(ctx, c, NULL, key, iv);

    if (flag == 1) {
        printf(" txt 길이%d\n", (int)strlen((char *)txt));
        
        ct = (unsigned char *)malloc((int)strlen((char *)txt));
        if (ct == NULL) {
            // error;
            fprintf(stderr, "error: %s\n", strerror(errno));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }
        
        error = EVP_EncryptInit_ex(ctx, c, NULL, key, NULL);
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        error = EVP_EncryptUpdate(ctx, &ct[ctlen], &num_byte, &txt[ctlen], 9);
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        ctlen += num_byte;
        printf("%d\n", num_byte);
        printf("ct? : %d\n", (int)strlen((char *)ct));
        EVP_CIPHER_CTX_free(ctx);
        return ct;
    }
    else {
        error = EVP_DecryptInit_ex(ctx, c, NULL, key, NULL);
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }
        printf(" txt 길이%d\n", (int)strlen((char *)txt));
        ct = (unsigned char *)malloc(sizeof(char) * (int)strlen((char *)txt));
        if (ct == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }


        error = EVP_DecryptUpdate(ctx, &ct[ptlen], &num_byte, &txt[ptlen], 9);
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        ptlen += num_byte;

        ct[ptlen] = '\0';
        EVP_CIPHER_CTX_free(ctx);
        return ct;
    }
}

int main(int argc, char **argv) {
    unsigned char *key;
    unsigned char *iv;

    gen_aes_ctr_key(&key, &iv);
    unsigned char *msg = (unsigned char *)argv[1];
    unsigned char *ct;
    unsigned char *pt;
    

    ct = symkey(key, iv, 1, msg, ct);
    printf("%s\n", ct);

    pt = symkey(key, iv, 0, ct, pt);
    printf("%s\n", pt);
    
    free(ct);
    free(pt);
    return 0;
}
