#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

unsigned char * symkey(const char *filename, int flag, unsigned char *txt, unsigned char *ct);

unsigned char * symkey(const char *filename, int flag, unsigned char *txt, unsigned char *ct) {
    int fd, length;
    int ctlen = 0; 
    int ptlen = 0;
    int num_byte = 0;

    const EVP_CIPHER *c = EVP_aes_128_ecb();
    EVP_CIPHER_CTX *ctx;
    ctx = EVP_CIPHER_CTX_new();

    if (ctx == 0) {
        //error;
        printf("\n");
    }

    EVP_EncryptInit_ex(ctx, c, NULL, NULL, NULL);

    unsigned char key[EVP_CIPHER_CTX_key_length(ctx)];

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        // error;
        printf("\n");
    }

    length = read(fd, key, EVP_CIPHER_CTX_key_length(ctx));

    if (flag == 1) {
        ct = (unsigned char *)malloc(strlen((char *)txt) + EVP_CIPHER_CTX_block_size(ctx));
        //unsigned char ct[strlen((char *)text + EVP_CIPHER_CTX_block_size(ctx))];
        EVP_EncryptInit_ex(ctx, c, NULL, key, NULL);
        EVP_EncryptUpdate(ctx, &ct[ctlen], &num_byte, &txt[ctlen], (int)strlen((unsigned char *)txt));

        ctlen += num_byte;
        EVP_EncryptFinal_ex(ctx, &ct[ctlen], &num_byte);
        
        EVP_CIPHER_CTX_free(ctx);
        return ct;
    }
    else {
        EVP_DecryptInit_ex(ctx, c, NULL, key, NULL);
        ct = (unsigned char *)malloc(strlen((char *)txt) + EVP_CIPHER_CTX_block_size(ctx));
        //unsigned char pt[strlen(text) + EVP_CIPHER_CTX_block_size(ctx)];
        
        EVP_DecryptUpdate(ctx, &ct[ptlen], &num_byte, &txt[ptlen], (int)strlen((char *)txt));
        
        ct[ptlen] = '\0';
        
        EVP_DecryptFinal_ex(ctx, &ct[ptlen], &num_byte);
        
        ptlen += num_byte;

        EVP_CIPHER_CTX_free(ctx);
        return ct;
    }
}

int main(void) {
    unsigned char msg[] = "test aes asdf";
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
