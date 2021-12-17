#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

BIO *publickey_bio = NULL;
BIO *privatekey_bio = NULL;
BIO *stdout_bio = NULL;

void closefiles(void);

int openfiles(void) {

    stdout_bio = BIO_new(BIO_s_file());

    if (stdout_bio) {
        BIO_set_fp(stdout_bio, stdout, BIO_NOCLOSE | BIO_FP_TEXT);
    }

    publickey_bio = BIO_new(BIO_s_file());
    privatekey_bio = BIO_new(BIO_s_file());

    if (publickey_bio == NULL || privatekey_bio == NULL) {
        closefiles();
        return -1;
    }

    if (BIO_write_filename(publickey_bio, "./Publickey.pem") <= 0) {
        closefiles();
        return -1;
    }

    if (BIO_write_filename(privatekey_bio, "./privatekey.pem") <= 0) {
        closefiles();
        return -1;
    }
    return 0;
}
        
int writefiles(RSA *rsa) {

    if (! PEM_write_bio_RSA_PUBKEY(stdout_bio, rsa)) {
        return -1;
    }

    if (! PEM_write_bio_RSA_PUBKEY(publickey_bio, rsa)) {
        return -1;
    }

    if (! PEM_write_bio_RSAPrivateKey(stdout_bio, rsa, NULL, NULL, 0, NULL, NULL)) {
        return -1;
    }

    if (! PEM_write_bio_RSAPrivateKey(privatekey_bio, rsa, NULL, NULL, 0, NULL, NULL)) {
        return -1;
    }
}

void closefiles(void) {
    if (publickey_bio) {
        BIO_free_all(publickey_bio);
    }

    if (privatekey_bio) {
        BIO_free_all(privatekey_bio);
    }

    if (stdout_bio) {
        BIO_free_all(stdout_bio);
    }

}

RSA *gen_key(int key_len) {
    RAND_status();
    return RSA_generate_key(key_len, RSA_F4, NULL, NULL);
}

int main(int argc, char ** argv) {
    int i = 0;
    RSA *rsa = NULL;

    int key_len = 2048;

    rsa = gen_key(key_len);

    if (rsa) {
        if (openfiles() == 0) {
            writefiles(rsa);
            closefiles();
        }

        RSA_free(rsa);
    }
    return 0;
}
