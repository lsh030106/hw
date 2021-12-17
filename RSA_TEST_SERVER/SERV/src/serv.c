/*
 Socket server certified

 1. client accept -> key exchange
 2. send the shared key to client (client public key encrypt)
 3. receive the shared key from client (server public key encrypt)
 4. save client ip in memory

 첫 인증후 서버는 클라이언트의 pubkey들을 파일로 저장
 클라이언트만 서버의 pubkey를 저장하고 전송, IP 인증완료 후 자신의 PUB key를 전송한다
 */
    
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_POOL 1024
#define SERVER_PORT 12345
#define EXIT "q\n"

int main(void) {
    int temp;
    int servsock, clntsock, lastsock;
    int sockopt = 1;
    int clntarr[MAX_POOL];
    struct sockaddr_in servaddr;
    fd_set clnt_fd, tem_fd;

    servsock = socket(PF_INET, SOCK_STREAM, 0);
    if (servsock == -1) {
        // error;
        return -1;
    }
    
    temp = setsockopt(servsock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt));
    if (temp == -1) {
        // error;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);
    serv.sin_addr.s_addr = INADDR_ANY;

    temp = bind(servsock, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    if (temp == -1) {
        // error
        close(servsock);
        return -1;
    }

    temp = listen(servsock, 5);
    if (temp == -1) {
        // error
        close(sersock);
        return -1;
    }

    
    


