#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>

int padding = RSA_PKCS1_OAEP_PADDING;

RSA * createRSA(unsigned char *key, int public);
int public_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted);
int public_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted);
int private_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted);
int private_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted);

RSA * createRSA(unsigned char *key, int public) {
    
    RSA *rsa = NULL;
    BIO *keybio;
    keybio = BIO_new_mem_buf(key, -1);  // 읽기전용 메모리 BIO
    if (keybio == NULL) {
        printf("Failed to create key BIO");
        return 0;
    }

    // PEM 형식 키 파일 RSA 구조체 형식으로 변환

    if (public) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    }
    else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }

    if (rsa == NULL) {
        printf("Failed to create RSA");
    }

    return rsa;
}

// 공개키 암호화

int public_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted) {

    RSA *rsa = createRSA(key, 1);
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    return result; // return the size of the encrypted data
}

// 공개키 복호화

int public_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted) {
    
    RSA *rsa = createRSA(key, 1);
    int result = RSA_public_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

// 개인키 암호화

int private_encrypt(unsigned char *data, int data_len, unsigned char *key, unsigned char *encrypted) {

    RSA *rsa = createRSA(key, 0);
    int result = RSA_private_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

// 개인키 복호화

int private_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted) {

    RSA *rsa = createRSA(key, 0);
    int result = RSA_private_decrypt(data_len, enc_data, decrypted, rsa, padding);
    return result;
}

int main(void) {
    char plain[2048/8] = "test RAS test";
    unsigned char encrypted[4098] = {};
    unsigned char decrypred[4098] = {};

    unsigned char *privatekey = "MIGrAgEAAiEAyrNJwTpKgpnUXgx6RCX5l038MFq2GSkybPAbJvyIpR8CAwEAAQIhALpG51nGuuCog4xmhRoJmAL63pOd92FSLx7grkBrftchAhEA+Lx+292iFVDx1UbHPBA+bwIRANCeovgL+PbzDXL02ATbXFECEDvNrj6Uqhyv6oUZ0Fs7mJcCEQC9ooY3fgKY0EU4avJ2L8SRAhA/KeSuIJaT49Y3qEJe2sI+";

    unsigned char *publickey = "-----BEGIN PUBLIC KEY-----\n\
        MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA2T74pnszVf9rYyE5XzhE\n\
        Li0UMy86YCJIxG3HdXXuKU8veVyU38o+ebgx/moeMK2ONDSM1I7jiELXQUiIGbye\n\
        cfmv4OiQVARi6Tq8xlHohEwwsTYxi4bBB4Bc1n0Cjg29aHNlM9mTBTLXkQwNQiEr\n\
        Nc9sR1nYM/NZimj7akdAGyRHY8aWqQDlITUmMjxx6CjTgsTqEt9PNAB7S0RPzqc6\n\
        3cyrdw6BkAquGXY2OIy5QwiLzUH+zaG7eWviwF/zbe+ad7gkf8TZ5f28w7o8C4Es\n\
        KECfdtnTbqzXxtPPXXXWk7U3Nqaj8GAeCwyxB5nXE5/H2HQWQgJ+UNedyxQS/YMY\n\
        VQIDAQAB\n\
        -----END PUBLIC KEY-----\n";

    int encrypted_length = public_encrypt(plain, strlen(plain), publickey, encrypted);

    printf("encrypted : %s\n", encrypted);

    return 0;
}




