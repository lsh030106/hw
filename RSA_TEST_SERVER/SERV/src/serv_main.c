#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_PORT 10000
#define DEFAULT 2048
#define LIMIT 1024
#define EXIT "q\n"

void indicerr(char *);

int main(void) {
	int servfd, clntfd, sockfd, maxfd;
	int wclntarr[LIMIT];	// 접속 대기중인 소켓의 배열
	int clntarr[LIMIT];    // 접속 중인 소켓의 배열
    char *iparr[12]; // 접속 된 소켓의 IP
    int sockcnt = 0;
	int state, len = 0;
	int clntmax = -1;
	int clntdn = 1;
    int fd;                 

    char clnt_ip[12];
	char buf[DEFAULT], sendbuf[DEFAULT];

	struct sockaddr_in serv, clnt;
	
	fd_set readfd, valfd;

	socklen_t clnt_len = sizeof(clnt);

	if ((servfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		indicerr("socket open error");
		return -1;
	}
	
	if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &clntdn, sizeof(clntdn)) == -1) {
		fprintf(stderr, "setsockopt : %s\n", strerror(errno));
		return -1;
	}

	memset(&serv, 0, sizeof(serv));

	serv.sin_family = AF_INET;
	serv.sin_port = htons(SERVER_PORT);
	serv.sin_addr.s_addr = INADDR_ANY;

	if (bind(servfd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
		indicerr("socket bind error");
		close(servfd);
		return -1;
	}

	if (listen(servfd, 5) == -1) {
		indicerr("socket listen error");
		close(servfd);
		return -1;
	}
	
	clntfd = maxfd = servfd;
	
	for (sockcnt = 0; sockcnt < LIMIT; sockcnt++) {
		clntarr[sockcnt] = -1;
	}

	FD_ZERO(&readfd);
	FD_SET(0, &readfd);
	FD_SET(servfd, &readfd);

	while (1) {
		valfd = readfd;

		state = select(maxfd + 1, &valfd, 0, 0, 0);
		memset(buf, 0, DEFAULT);

		if (FD_ISSET(0, &valfd)) {
			int len = 0;
			memset(sendbuf, 0, DEFAULT);
			len = read(0, buf, DEFAULT);
			buf[len] = '\0';
			sprintf(buf, "%s", sendbuf);
			
			printf("%s", buf);

			if (strstr(buf, EXIT)) {
				printf("hear %s", buf);
				for (sockcnt = 0; sockcnt <= clntmax; sockcnt++) {
					send(clntarr[sockcnt], buf, DEFAULT, 0);
				}
			
			break;
			}
		}
        // 클라이언트 접속 부분
		if (FD_ISSET(servfd, &valfd)) {
			clnt_len = sizeof(clnt);
            memset(clnt_ip, 0, sizeof(clnt_ip));
			clntfd = accept(servfd, (struct sockaddr *)&clnt, &clnt_len); 
	        
            // 접속 클라이언트 IP fail 시 NULL 반환 에러 처리 x
            inet_ntop(AF_INET, &clnt.sin_addr, clnt_ip, INET_ADDRSTRLEN);
            iparr[clntfd] = clnt_ip;
           
            // handshake 

            /* 키교환이 완료된 클라이언트를 따로 배열에 저장해 완료되지 않은
             클라이언트는 격리 시킨다. 키교환 여부는 IP를 기준으로 서버에서 비밀키로 보낸 
             메세지가 정확히 도착하는지로 판단한다*/

            // wait queue 에 클라이언트를 차례로 넣는다
			for (sockcnt = 0; sockcnt < LIMIT; sockcnt++) {

				if (wclntarr[sockcnt] < 0) {
					wclntarr[sockcnt] = clntfd;
					break;
				}
			}
		

			if (sockcnt == LIMIT) {
				indicerr("too many client");
			}
	
			FD_SET(clntfd, &readfd);

			if (clntfd > maxfd) {
				maxfd = clntfd;
			}

			if (sockcnt > clntmax) {
				clntmax = sockcnt;
			}

			if (--state <= 0) {
				continue;
			}
		}
			
		// 클라이언트 접속 종료 예외 처리
        for (sockcnt = 0; sockcnt <= clntmax; sockcnt++) {
	
			if ((sockfd = clntarr[sockcnt]) == -1) {
				continue;
			}

			if (FD_ISSET(sockfd, &valfd)) {
				memset(sendbuf, 0, DEFAULT);
				memset(buf, 0, DEFAULT);
				
				if ((len = recv(sockfd, buf, DEFAULT, 0)) == -1) {
					indicerr("close socket");
					close(sockfd);
					FD_CLR(sockfd, &readfd);
					clntarr[sockcnt] = -1;
				}

				if (len == 0) {
					close(sockfd);
					FD_CLR(sockfd, &readfd);
					clntarr[sockcnt] = -1;
					memset(buf, 0, DEFAULT);
					sprintf(buf, "%s", "out of connection");
				}
		
				if (! (strcmp(buf, EXIT))) {
					close(sockfd);
					FD_CLR(sockfd, &readfd);
					clntarr[sockcnt] = -1;
					memset(buf, 0, DEFAULT);
					sprintf(buf, "%s", "out of connection");
				}

				printf("%s : %s\n", iparr[sockfd], buf);
				sprintf(sendbuf, "%s : %s", iparr[sockfd], buf);

				if (--state <= 0) {
					break;
				}
			}
		}
		
        // 해당 부분에서 접속된 클라이언트와 접속이 되지 않은 클라이언트를 나눠 계산한다
        
		for (sockcnt = 0; sockcnt <= clntmax; sockcnt++) {
            
            // wait queue에서 대기중인 클라이언트
            if (sockfd == wclntarr[sockcnt]) {
                char *protocol = NULL;

                char *temp = strtock_s(sendbuf, "$$", protocol);
                // $$ 앞의 문자열으로 단계를 4단계로 나누어 handshake 한다
            }


			if (sockfd == clntarr[sockcnt]) {
				continue;
			}
            // 키교환이 완료되어 접속중인 클라이언트 채팅
			send(clntarr[sockcnt], sendbuf, DEFAULT, 0);
            
		}

	}
	close(servfd);
	return 0;
}

void indicerr(char *msg) {
	fprintf(stderr, "%s : %s\n", msg, strerror(errno));
}
