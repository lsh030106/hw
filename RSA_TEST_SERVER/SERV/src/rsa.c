#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>

#define eprintf(args...) fprintf(stderr, ##args)

int RSA_genkey(RSA **rsakey, int bits) {

    BIGNUM *bne = NULL;

    bne = BN_new();
    if (BN_set_word(bne, RSA_F4) != 1) {
        return 0;
    }

    *rsakey = RSA_new();
    if (RSA_generate_key_ex(*rsakey, bits, bne, NULL) != 1) {
        BN_free(bne);
        return 0;
    }

    return 1;
}

int main(void) {
    BIO *out = NULL;
    RSA *rsakey = NULL;
    EVP_PKEY *pkey = NULL;
    unsigned char msg[] = "test, RSA test";
    unsigned char ct[256], pt[256];
    
    int ctLen, ptLen;

    /*if (!RSA_genkey(&rsakey, 2048)) {
        eprintf("RSA_genkey errror\n");

        return 1;
    }*/
    
    

    out = BIO_new_fp(stdout, BIO_CLOSE);

    pkey = EVP_PKEY_new();
    EVP_PKEY_set1_RSA(pkey, rsakey);

    printf("===============privatekey\n================\n");
    EVP_PKEY_print_private(out, pkey, 0, NULL);
    printf("===============publickey\n=================\n");
    EVP_PKEY_print_public(out, pkey, 0, NULL);

    printf("original text\n");
    BIO_dump_fp(stdout, msg, sizeof(msg));
    
    printf("public_ecrypt\n");
    ctLen = RSA_public_encrypt(sizeof(msg), msg, ct, rsakey, RSA_PKCS1_OAEP_PADDING);
    BIO_dump_fp(stdout, ct, ctLen);
    
    printf("private_decrypt\n");
    ptLen = RSA_private_decrypt(ctLen, ct, pt, rsakey, RSA_PKCS1_OAEP_PADDING);
    BIO_dump_fp(stdout, pt, ptLen);

    RSA_free(rsakey);
    EVP_PKEY_free(pkey);
    BIO_free(out);
}
    


