#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void aes128Decrypt(unsigned char *ct, unsigned char *key);

void aes128Encrypt(unsigned char *pt) {
    int fd, lenght;
    int ctlen, num_byte = 0;

    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == 0) {
        //error;
        printf("\n");
    }

    EVP_EncryptInit_ex(ctx, c, NULL, NULL, NULL);
   
    unsigned char key[EVP_CIPHER_CTX_key_length(ctx)]; 

    fd = open("secretkey.key", O_RDONLY);
    if (fd == -1) {
        // error;
        printf("\n");
    }

    lenght = read(fd, key, EVP_CIPHER_CTX_key_length(ctx));
   
    unsigned char ct[strlen((char *)pt + EVP_CIPHER_CTX_block_size(ctx))];
    
    EVP_EncryptInit_ex(ctx, c, NULL, key, NULL);

    EVP_EncryptUpdate(ctx, &ct[ctlen], &num_byte, &pt[ctlen], (int)strlen((char *)pt));

    ctlen += num_byte;
    EVP_EncryptFinal_ex(ctx, &ct[ctlen], &num_byte);
    EVP_CIPHER_CTX_free(ctx);
    
    printf("%s\n", ct);

    aes128Decrypt(ct, key);
}

void aes128Decrypt(unsigned char *ct, unsigned char *key) {
    
    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == 0) {
        //error;
        printf("\n");
    }
    
    EVP_DecryptInit_ex(ctx, c, NULL, key, NULL);

    unsigned char pt[strlen(ct) + EVP_CIPHER_CTX_block_size(ctx)];
    memset(pt, 0, strlen(pt));

    int ptlen, num_byte = 0;

    EVP_DecryptUpdate(ctx, &pt[ptlen], &num_byte, &ct[ptlen], strlen(ct));
    
    ptlen += num_byte;
    EVP_DecryptFinal_ex(ctx, &pt[ptlen], &num_byte);
    
    printf("%s\n", pt);
}

int main(void) {
    char pt[] = "test aes test";
    aes128Encrypt(pt);
    

    return 0;
}

