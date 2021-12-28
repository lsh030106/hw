#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "AES_CTR_keygen.h"
#include "RSA_keygen.h"
#include "RSA_CR.h"
#include "BASE_64.h"
#include "euc2utf.h"
#include "AES_CTR_CR.h"

#ifdef WIN32
    #define OS "WIN32"
#endif
#ifdef __APPLE__
    #define OS "apple"
#endif
#ifdef __linux__
    #define OS "linux"
#elif define  __unix__
    #define OS "unix"
#endif

#define PUB "PUB"
#define SYM "SYM"
#define MSG "MSG"
#define OKL "OKL"
#define EXT "EXT"
#define DEFAULT 1024
#define PORT 10000

int loop = 1;
int authen = 0;
unsigned char *g_key;

struct thread_info {
    pthread_t thread_id;
    int thread_num;
    int servsock;
};

struct thread_buf {
    char sendbuf[DEFAULT];
    char recvbuf[DEFAULT];
    char *payload;
};

int cat_header(char *sendbuf, const char *header, char *payload);
int start_client(void);

int main(void) {

    start_client();

    return 0;
}

// MT-safe 함수들만 사용, mutex는 필요 없음
void *recv_thread(void *arg) {
    struct thread_info *tinfo = (struct thread_info *)arg;
    struct thread_buf buf;
    memset(&buf, 0, sizeof(buf));
    char *next_ptr;
    char *retval = NULL;
    char *header;
    unsigned char ct[DEFAULT] = {0,};
    int len;
    
    /*
       OKL 프로토콜로 클라이언트 정보 전송 (ex. 인코딩, 운영체제 등)
       현재 플랫폼 종류를 송신
     */
    cat_header(buf.sendbuf, OKL, (char *)OS);
    len = write(tinfo->servsock, buf.sendbuf, strlen(buf.sendbuf)); 

    while (loop) {
        memset(&buf, 0, sizeof(buf));
        
        if (! loop) {
            pthread_exit((void *)retval);
        }
        
        len = read(tinfo->servsock, buf.recvbuf, DEFAULT);
        if (len < 0) {
            fprintf(stderr, "%s", strerror(errno));
            break;
        }
        header = strtok_r(buf.recvbuf, ":", &next_ptr);
         
        /*
        서버에서 PUBKEY 수신, 
        현재 디렉토리에 public 디렉토리 생성후 .pem 파일로 저장
         */
        if (! (strcmp(header, PUB))) {
            printf("recieve server publickey.....\n");

            header = strtok_r(NULL, ":", &next_ptr);
            buf.payload = header;
            
            retval = RSA_pubkey_write("public/serv_key.pem", buf.payload,
                    len - 4);
            if (retval == NULL) {
                printf("fail store server pubkey...\n");
                retval = strerror(errno);
                pthread_exit((void *)retval);
            }
            /*
            간단한 세션과 세션키의 정의
            프로그램 사이에서 서로를 인식해  통신을 시작하고 마치기까지 기간을 세션이라고 함
            세션키는 하나의 통신 세션 동안에만 사용하는 암호화 임시 키
            
            공개키를 받고 자신의 sym key를 전송
             */        
            memset(&buf.payload, 0, sizeof(buf.payload));
            
            unsigned char *key = NULL;
            unsigned char *iv = NULL;
            gen_aes_ctr_key(&key, &iv); 
            
            g_key = key;
            //char *BASE64 = NULL;
            int cplen = public_enc(key, ct, 
                    "public/serv_key.pem");
            
            if (cplen == 0) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
            //base64Encode((char *)ct, &BASE64);
            
            cat_header(buf.sendbuf, SYM, (char *)ct);
            printf("%s\n", buf.sendbuf);
            len = write(tinfo->servsock, buf.sendbuf, 260);

        }
        
        if (! (strcmp(header, OKL))) {
            authen = 1;
        }

        // recv message 
        if (! (strcmp(header, MSG))) {
            header = strtok_r(NULL, ":", &next_ptr);
            buf.payload = header;
            printf("%s\n", buf.payload);
        }

        printf("%s\n", buf.recvbuf);

    }
    pthread_exit((void *)retval);
}

int cat_header(char *sendbuf, const char *header, char *payload) {
    char *temp;

    temp = strcat(strcat(strcat(sendbuf, header), ":"), payload);
    if (temp == NULL) {
        return -1;
    }

    return 0;
}

int start_client() {
    struct thread_buf buf;
    const unsigned short port = PORT;
    int servsock;
    int temp;
    struct sockaddr_in serv_addr;

    servsock = socket(PF_INET, SOCK_STREAM, 0);

    if (servsock == -1) {
        fprintf(stderr, "socket open error : %s\n", strerror(errno));
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("10.10.0.119");
    serv_addr.sin_port = htons(port);

    temp = connect(servsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (temp == -1) {
        fprintf(stderr, "connect error : %s\n", strerror(errno));
        close(servsock);
        return -1;
    }

    struct thread_info thr_info;
    memset(&thr_info, 0, sizeof(thr_info));
    
    int thr_id = 0;
    void *status = NULL;
    thr_info.servsock = servsock;
    
    thr_id = pthread_create(&thr_info.thread_id, NULL, recv_thread, (void *)&thr_info);
    if (thr_id != 0) {
        fprintf(stderr, "thread create error : %s\n", strerror(errno));
        close(servsock);
        return -1;
    }
    
    while (! authen) {};
    unsigned char *CT = {0,};
    unsigned char iv[] = "aaaa";
    int cp_length = 0;
    while (1) {
        if (memset(buf.sendbuf, 0, DEFAULT) == NULL) {
            fprintf(stderr, "%s\n", strerror(errno));
            break;
        }
        
        fgets(buf.sendbuf, DEFAULT, stdin);
        
        if (strcmp(buf.sendbuf, "q\n") == 0 || strcmp(buf.sendbuf, "Q\n") == 0) {
            loop = 0;
            break;
        }
        
        
        cp_length = aes_ctr_encrypt(g_key, iv, (unsigned char *)buf.sendbuf, &CT);
        cat_header(buf.recvbuf, MSG, (char *)CT);
        write(servsock, buf.recvbuf, cp_length);
    }
    pthread_join(thr_info.thread_id, &status);
    printf("%d\n", *((int *)status));

    close(servsock);
    return 0;
}
