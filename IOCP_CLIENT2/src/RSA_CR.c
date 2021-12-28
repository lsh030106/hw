#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

#define KEYLEN 256

int public_enc(unsigned char *msg, unsigned char *Ct, const char *filename);
int private_dec(unsigned char *Ct, unsigned char *Pt, const char *filename);

int main(void) {
    unsigned char Ct2[KEYLEN * 2];
    unsigned char Pt[KEYLEN];
    unsigned char msg[] = "test rsa test";
    int ctlen2;
    int ptlen;

    ctlen2 = public_enc(msg, Ct2, "serv_key2.pem");
    BIO_dump_fp(stdout, Ct2, ctlen2);
    printf("%s\n", Ct2); 
    printf("===============================\n"); 
    ptlen = private_dec(Ct2, Pt, "serv_key.pem");
    BIO_dump_fp(stdout, Pt, ptlen);
    printf("%s\n", Pt);
    return 0;
}

int public_enc(unsigned char *msg, unsigned char *Ct, const char *filename) {
    
    int ctlen;
    RSA *rsa = RSA_new();
    BIO *keybio = NULL;

    keybio = BIO_new(BIO_s_file());
    BIO_read_filename(keybio, filename);
    
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    ctlen = RSA_public_encrypt(strlen((char *)msg), msg, Ct, rsa, RSA_PKCS1_OAEP_PADDING);
    printf("%d\n", ctlen); 
    RSA_free(rsa);
    BIO_free(keybio);
    return ctlen;
}

int private_dec(unsigned char *Ct, unsigned char *Pt, const char *filename) {

    int ptlen;
    RSA *rsa = RSA_new();
    BIO *keybio = NULL;
    keybio = BIO_new(BIO_s_file());
    BIO_read_filename(keybio, filename);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    ptlen = RSA_private_decrypt(KEYLEN, Ct, Pt, rsa, RSA_PKCS1_OAEP_PADDING);
    
    printf("%s\n", Pt);
    RSA_free(rsa);
    BIO_free(keybio);
    return ptlen;
}
