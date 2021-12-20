#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

int main(void) {
    int fd;
    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();
    
    if (ctx == 0) {
        //error;
        printf("\n");
    }
    
    EVP_EncryptInit_ex(ctx, c, NULL, NULL, NULL);

    unsigned char key[EVP_CIPHER_CTX_key_length(ctx)];

    RAND_bytes(key, EVP_CIPHER_CTX_key_length(ctx));

    puts(key);

    fd = open("secretkey.key", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        //error;
        printf("\n");
    }

    if (write(fd, key, EVP_CIPHER_CTX_key_length(ctx)) == -1) {
        // error;
        printf("\n");
    }

    close(fd);

    return 0;
}
