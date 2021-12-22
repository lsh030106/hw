#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

int gen_aes_key(void) {
    int fd;
    int error;
    unsigned long err;

    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    
    if (ctx == NULL) {
        //error;
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

    unsigned char key[EVP_CIPHER_CTX_key_length(ctx)];

    error = RAND_bytes(key, EVP_CIPHER_CTX_key_length(ctx));
    if (error != 1) {
        err = ERR_get_error();
        printf("%s\n", ERR_error_string(err, NULL));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    puts(key);

    fd = open("secretkey.key", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        fprintf(stderr, "file: %s, error: %s\n", "secretkey.key", strerror(errno));
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    error = write(fd, key, EVP_CIPHER_CTX_key_length(ctx));
    if (error == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        EVP_CIPHER_CTX_free(ctx);
        close(fd);
        return 0;
    }

    close(fd);
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}
