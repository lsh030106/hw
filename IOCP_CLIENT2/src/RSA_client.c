#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "RSA_keygen.h"

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
#define DEFAULT 1024

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

int main(void) {
    struct thread_buf buf;
    memset(&buf, 0, sizeof(buf));

    cat_header(buf.sendbuf, OKL, (char *)OS);
    printf("%s\n", buf.sendbuf);
    return 0;
}

// MT-safe 함수들만 사용, mutex는 필요 없음
void *recv_thread(void *arg) {
    struct thread_info *tinfo = (thread_info *)arg;
    struct thread_buf buf;
    memset(&buf, 0, sizeof(buf));
    char *next_ptr;
    char *retval;
    char *header;
    int len;
    
    /*
       OKL 프로토콜로 클라이언트 정보 전송 (ex. 인코딩, 운영체제 등)
       현재 플랫폼 종류를 송신
     */
    cat_header(buf.sendbuf, OKL, (char *)OS);
    len = write(tinfo->servsock, buf.sendbuf, DEFAULT); 

    while (1) {
        memset(&buf, 0, sizeof(buf));

        len = read(tinfo->servsock, buf.recvbuf, DEFAULT);
        if (len < 0) {
            fprintf(stderr, "%s", strerror(errno));
            continue;
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
            
            retval = RSA_pubkey_write("public/%s.pem", buf.payload);
            if (retval == NULL) {
                printf("fail store server pubkey...\n");
                retval = strerror(errno);
                pthread_exit((void *)retval);
            }
            /*
            간단한 세션과 세션키의 정의
            프로그램 사이에서 서로를 인식해  통신을 시작하고 마치기까지 기간을 세션이라고 함
            세션키는 하나의 통신 세션 동안에만 사용하는 암호화 임시 키
             */

            
        }

    }
}

int cat_header(char *sendbuf, const char *header, char *payload) {
    char *temp;

    temp = strcat(strcat(strcat(sendbuf, header), ":"), payload);
    if (temp == NULL) {
        return -1;
    }

    return 0;
}
/*
int start_client(void) {

    return 0;
}*/
