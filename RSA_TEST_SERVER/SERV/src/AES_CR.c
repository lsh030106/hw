#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

unsigned char * symkey(const char *filename, int flag, unsigned char *txt, unsigned char *ct);

unsigned char * symkey(const char *filename, int flag, unsigned char *txt, unsigned char *ct) {
    int fd, length;
    int ctlen = 0; 
    int ptlen = 0;
    int num_byte = 0;
    unsigned long err = 0;
    int error = 0;

    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == NULL) {
        //error;
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    EVP_EncryptInit_ex(ctx, c, NULL, NULL, NULL);

    unsigned char key[EVP_CIPHER_CTX_key_length(ctx)];

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        // error;
        fprintf(stderr, "file: %s, error: %s\n", filename, strerror(errno));
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    length = read(fd, key, EVP_CIPHER_CTX_key_length(ctx));
    if (length == -1) {
        // error;
        fprintf(stderr, "file: %s, error: %s\n", filename, strerror(errno));
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }

    if (flag == 1) {
        ct = (unsigned char *)malloc(strlen((char *)txt) + EVP_CIPHER_CTX_block_size(ctx));
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

        error = EVP_EncryptUpdate(ctx, &ct[ctlen], &num_byte, &txt[ctlen], (int)strlen((unsigned char *)txt));
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        ctlen += num_byte;
        
        EVP_EncryptFinal_ex(ctx, &ct[ctlen], &num_byte);
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

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
        
        ct = (unsigned char *)malloc(strlen((char *)txt) + EVP_CIPHER_CTX_block_size(ctx) + 1);
        if (ct == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        error = EVP_DecryptUpdate(ctx, &ct[ptlen], &num_byte, &txt[ptlen], (int)strlen((char *)txt));
        if (error == 0) {
            err = ERR_get_error();
            printf("%s\n", ERR_error_string(err, NULL));
            EVP_CIPHER_CTX_free(ctx);
            return NULL;
        }

        ptlen += num_byte;

        error = EVP_DecryptFinal_ex(ctx, &ct[ptlen], &num_byte);
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

int main(void) {
    unsigned char msg[] = "test AES_128_ECB Symmetic cipher this winner";
    unsigned char *ct;
    unsigned char *pt;
    ct = symkey("secretkey.key", 1, msg, ct);
    printf("%s\n", ct);

    pt = symkey("secretkey.key", 0, ct, pt);
    printf("%s\n", pt);
    
    free(ct);
    free(pt);
    return 0;
}
