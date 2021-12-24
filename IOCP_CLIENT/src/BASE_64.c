#include <openssl/bio.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int base64Encode(const char *msg, char **buffer) {
    BIO *bio, *b64;
    FILE *stream;

    int encodeSize = 4 * ceil((double)strlen(msg) / 3);
    *buffer = (char *)malloc(encodeSize + 1);

    stream = fmemopen(*buffer, encodeSize + 1, "w");
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, msg, strlen(msg));
    BIO_flush(bio);
    BIO_free_all(bio);
    fclose(stream);

    return 0;
}

int calcDecodeLength(const char * b64input) {
    int len = strlen(b64input);
    int padding = 0;

    if (b64input[len - 1] == '=' && b64input[len - 2] == '=') {
        padding = 2;
    }
    else if (b64input[len - 1] == '=') {
        padding = 1;
    }

    return (int)len * 0.75 - padding;
}

int base64Decode(char *b64msg, char **buffer) {
    BIO *bio, *b64;
    int decodeLen = calcDecodeLength(b64msg);
    int len = 0;
    *buffer = (char *)malloc(decodeLen + 1);
    FILE *stream = fmemopen(b64msg, strlen(b64msg), "r");

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_fp(stream, BIO_NOCLOSE);
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    len = BIO_read(bio, *buffer, strlen(b64msg));
    (*buffer)[len] = '\0';

    BIO_free_all(bio);
    fclose(stream);

    return 0;
}
