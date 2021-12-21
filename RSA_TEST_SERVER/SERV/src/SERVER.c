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
#define DEFAULT 2048
#define LIMIT 1024
#define EXIT "q\n"

int m_server(char *IP);

int main(void) {
    m_server("10.10.0.96");

    return 0;
}

int m_server(char *IP) {

    int temp;
    int serv_sock;
    struct sockaddr_in serv_addr;
    
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

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    temp = bind(serv_sock, (struct sockaddr *)&serv, sizeof(serv));
    if (temp == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        close(serv_sock);
        return -1;
    }

    temp = listen(serv_sock, 5)

    return 0;
}
