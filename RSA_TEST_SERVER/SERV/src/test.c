#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

int public_enc(unsigned char *msg, unsigned char **Ct, const char *filename);
int private_dec(unsigned char *Ct, unsigned char **Pt, const char *filename);

int main(void) {
    unsigned char Ct[256];
    unsigned char Ct2[256];
    unsigned char Pt[256];
    unsigned char msg[] = "test rsa test";
    int ctlen;
    int ctlen2;
    int ptlen;

    RSA *rsa = RSA_new();
    BIO *keybio = NULL;

    keybio = BIO_new(BIO_s_file());
    BIO_read_filename(keybio, "Publickey.pem");

    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    ctlen = RSA_public_encrypt(sizeof(msg), msg, Ct, rsa, RSA_PKCS1_OAEP_PADDING);
//    BIO_dump_fp(stdout, Ct, ctlen);

    ctlen2 = public_enc(msg, Ct2, "Publickey.pem");
    BIO_dump_fp(stdout, Ct2, ctlen2);
    
    
    ptlen = private_dec(Ct2, Pt, "privatekey.pem");
    BIO_dump_fp(stdout, Pt, ptlen);

    return 0;
}

int public_enc(unsigned char *msg, unsigned char **Ct, const char *filename) {
    
    int ctlen;
    RSA *rsa = RSA_new();
    BIO *keybio = NULL;

    keybio = BIO_new(BIO_s_file());
    BIO_read_filename(keybio, filename);
    
    rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    ctlen = RSA_public_encrypt(sizeof(msg), msg, Ct, rsa, RSA_PKCS1_OAEP_PADDING);

    return ctlen;
}

int private_dec(unsigned char *Ct, unsigned char **Pt, const char *filename) {

    int ptlen;
    RSA *rsa = RSA_new();
    BIO *keybio = NULL;

    keybio = BIO_new(BIO_s_file());
    BIO_read_filename(keybio, filename);

    rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    ptlen = RSA_private_decrypt(sizeof(Ct), Ct, Pt, rsa, RSA_PKCS1_OAEP_PADDING);

    return ptlen;
}
