#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <errno.h>

#include "AES_CR.h"
#include "AES_keygen.h"
#include "RSA_CR.h"
#include "RSA_keygen.h"

#define SERVER_PORT 10000
#define DEFAULT 1024
#define LIMIT 2048
#define EXIT "q\n"

int m_server(char *IP);

int main(void) {
    m_server("10.10.0.96");

    return 0;
}

int m_server(char *IP) {
    /* 클라이언트 접속 관리*/
    int clntarr[LIMIT] = {0,};
    int sockcnt = 0;

    int temp;
    int serv_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    fd_set readfd, valfd;
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    
    int optflag = 1;
    temp = setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &optflag, sizeof(optflag));
    if (temp == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    temp = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (temp == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        close(serv_sock);
        return -1;
    }

    temp = listen(serv_sock, 5);
    if (temp == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        close(serv_sock);
        return -1;
    }
    
    // 클라이언트 소켓의 배열을 -1로 세팅
    for (sockcnt = 0; sockcnt < LIMIT; sockcnt++) {
        clntarr[sockcnt] = -1;
    }
    
    FD_ZERO(&readfd);
    FD_SET(0, &readfd);
    FD_SET(serv_sock, &readfd);
    // listen 까지 완료 

    printf("start server... \n");
    
    // 서버 실행간 필요 변수
    int max_sock = serv_sock;
    int clnt_sock = serv_sock;
    int temp_sock;
    int clnt_addr_len;
    int state;
    int clnt_max = -1;
    
    char recvbuf[DEFAULT], sendbuf[DEFAULT]; 
    char clnt_ip[12];
    char *iparr[12];

    while (1) {
        valfd = readfd;
        
        state = select(max_sock + 1, &valfd, 0, 0, 0);
        memset(recvbuf, 0, DEFAULT);
        
        if (FD_ISSET(0, &valfd)) {
            int len = 0;
            memset(sendbuf, 0, DEFAULT);
            len = read(0, recvbuf, DEFAULT);
            recvbuf[len] = '\0';
            sprintf(recvbuf, "%s", sendbuf);

            printf("%s", recvbuf);

            if (strstr(recvbuf, EXIT)) {
                printf("%s", recvbuf);
                for (sockcnt = 0; sockcnt <= clnt_max; sockcnt++) {
                    send(clntarr[sockcnt], recvbuf, DEFAULT, 0);
                }

                break;
            }
        }

        // 클라이언트 접속
        if (FD_ISSET(serv_sock, &valfd)) {
            clnt_addr_len = sizeof(clnt_addr);
            clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);

            // 접속한 클라이언트의 IP 주소 저장
            inet_ntop(AF_INET, &clnt_addr.sin_addr, clnt_ip, INET_ADDRSTRLEN);
            iparr[clnt_sock] = clnt_ip;
            
            printf("%s", clnt_ip);
            // client 소켓 베열에 저장
            for (sockcnt = 0; sockcnt < LIMIT; sockcnt++) {
                
                if (clntarr[sockcnt] < 0) {
                    clntarr[sockcnt] = clnt_sock;
                    break;
                }

                if (sockcnt == LIMIT) {
                    printf("too many client\n");
                    close(clnt_sock);
                }
                
                FD_SET(clnt_sock, &readfd);

                if (clnt_sock > max_sock) {
                    max_sock = clnt_sock;
                }

                if (sockcnt > clnt_max) {
                    clnt_max = sockcnt;
                }

                if (--state <= 0) {
                    continue;
                }
            }
        
            int len;
            // 클라이언트 접속 종료 예외 처리
            for (sockcnt = 0; sockcnt <= clnt_max; sockcnt++) {
            
                if ((temp_sock = clntarr[sockcnt]) == -1) {
                    continue;
                }

                if (FD_ISSET(temp_sock, &valfd)) {
                    memset(sendbuf, 0, DEFAULT);
                    memset(recvbuf, 0, DEFAULT);
    
                    if ((len = recv(temp_sock, recvbuf, DEFAULT, 0)) == -1) {
                        printf("close socket\n");
                        close(temp_sock);
                        FD_CLR(temp_sock, &readfd);
                        clntarr[sockcnt] = -1;
                    }

                    if (len == 0) {
                        close(temp_sock);
                        FD_CLR(temp_sock, &readfd);
                        clntarr[sockcnt] = -1;
                        memset(recvbuf, 0, DEFAULT);
                        sprintf(recvbuf, "%s", "out of connection");
                    }
    
                    if (! (strcmp(recvbuf, EXIT))) {
                        close(temp_sock);
                        FD_CLR(temp_sock, &readfd);
                        clntarr[sockcnt] = -1;
                        memset(recvbuf, 0, DEFAULT);
                        sprintf(recvbuf, "%s", "out of connection");
                    }
                    

                    printf("%s : %s\n", iparr[temp_sock], recvbuf);
                    sprintf(sendbuf, "%s : %s\n", iparr[temp_sock], recvbuf);
    
                    if (--state <= 0) {
                        break;
                    }
                }   
            }   
    
            // 수신 받은 메세지 분리

            send(clntarr[sockcnt], sendbuf, DEFAULT, 0);
        }
    }
    
    close(serv_sock);
    return 0;
}
