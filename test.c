#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "AES_CTR_CR.h"
#include "AES_CTR_keygen.h"
#include "RSA_CR.h"
#include "RSA_keygen.h"

char test[] = "test test test test test test";

struct AesTest {
    int (* keygen) (unsigned char **, unsigned char **);
    int (* encrypt) (unsigned char *, unsigned char *, unsigned char *, unsigned char **);
    int (* decrypt) (unsigned char *, unsigned char *,\
            int, unsigned char *, unsigned char **);
};

struct RsaTest {
    int (* keygen) (char *);
    int (* encrypt) (unsigned char *, unsigned char *, const char *);
    int (* decrypt) (unsigned char *, unsigned char *, const char *);
};

void *p_aes_crypto(void *arg) {
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    clock_t start, end;
    float res;
    unsigned char *key = NULL;
    unsigned char *iv = NULL;
    unsigned char *plain = NULL;
    unsigned char *cipher = NULL;
    int cplen = 0;
    int i = 0;

    struct AesTest *aes = (struct AesTest *)arg;
    
    start = clock();
    aes->keygen(&key, &iv);

    for (i = 0; i < 10000; i++) {
        cplen = aes->encrypt(key, iv, test, &cipher);
        aes->decrypt(key, iv, cplen, cipher, &plain);
    }
    
    end = clock();
    res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("%.5f\n", res);
}

/*int aes_crypto(void) {
      clock_t start, end;
      float res;
      unsigned char *key = NULL;
      unsigned char *iv = NULL;
      unsigned char *plain;
      unsigned char *cipher;
      int cplen = 0;
      int i = 0;
  
      
      start = clock();
      gen_aes_ctr_key(&key, &iv);

      for (i; i < 10000; i++) {
          cplen = 0;
          
          cplen = aes_ctr_encrypt(key, iv, test, &cipher);
          aes_ctr_decrypt(key, iv, cplen, cipher, &plain);
      }
      
      end = clock();
      res = (float)(end - start)/CLOCKS_PER_SEC;
      printf("AES : %.3f\n", res);

      return 0;
 }*/


void *p_rsa_crypto(void *arg) {
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    clock_t start, end;
    float res;
    int i = 0;
    unsigned char plain[256];
    unsigned char cipher[256];

    struct RsaTest *rsa = (struct RsaTest *)arg;
    start = clock();
    
    rsa->keygen("test");
    
    for (i = 0; i < 10000; i++) {
        rsa->encrypt(test, cipher, "public/test.pem");
        rsa->decrypt(cipher, plain, "private/test.pem");
    }

    end - clock();
    res = (float)(end - start)/CLOCKS_PER_SEC;
    printf("%.5f\n", res);
}

/*int rsa_crypto(void) {
      clock_t start, end;
      float res;
      int i = 0;
      unsigned char plain[256];
      unsigned char cipher[256];
  
      start = clock();
  
      make_key("test");
  
      for (i; i < 10000; i++) {
          memset(plain, 0, sizeof(plain));
          memset(cipher, 0, sizeof(cipher));
          public_enc((unsigned char *)test, cipher, "public/test.pem");
          private_dec(cipher, plain, "private/test.pem");
      }
  
      end = clock();
      res = (float)(end - start)/CLOCKS_PER_SEC;                                                                     
      printf("RSA : %.3f\n", res);
      return 0;
 }

int main(void) {
    aes_crypto();
    rsa_crypto();
    return 0;
}
*/
int main(void) {
    struct AesTest *aes;
    struct RsaTest *rsa;

    aes->keygen = gen_aes_ctr_key;
    aes->encrypt = aes_ctr_encrypt;
    aes->decrypt = aes_ctr_decrypt;

    rsa->keygen = make_key;
    rsa->encrypt = public_enc;
    rsa->decrypt = private_dec;

    pthread_t pthread[2];
    int thr_id;
    int status;

    thr_id = pthread_create(&pthread[0], NULL, p_aes_crypto, (void *)aes);
    if (thr_id < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }

    thr_id = pthread_create(&pthread[1], NULL, p_rsa_crypto, (void *)rsa);
    if (thr_id < 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        return 1;
    }

    pthread_join(pthread[0], (void **)&status);
    pthread_join(pthread[1], (void **)&status);

    return 0;
}


