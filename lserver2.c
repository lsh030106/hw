#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_PORT 10000
#define DEFAULT 1024
#define LIMIT 32

void indicerr(char *);

int main(void) {
	int servfd, clntfd, sockfd, maxfd;
	int clntarr[LIMIT];	// 소켓의 배열
	int sockcnt = 0;
	int state = 0;
	int clntmax = -1;
	int clntdn = 1;

	char buf[DEFAULT];

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
	FD_SET(servfd, &readfd);

	while (1) {
		valfd = readfd;

		// select함수는 file descriptor 수를 반환
		state = select(maxfd + 1, &valfd, 0, 0, 0);

		if (FD_ISSET(servfd, &valfd)) {
			clnt_len = sizeof(clnt);
			clntfd = accept(servfd, (struct sockaddr *)&clnt, &clnt_len); 
		
			for (sockcnt = 0; sockcnt < LIMIT; sockcnt++) {

				if (clntarr[sockcnt] < 0) {
					clntarr[sockcnt] = clntfd;
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
		
		for (sockcnt = 0; sockcnt <= clntmax; sockcnt++) {
	
			if ((sockfd = clntarr[sockcnt]) == -1) {
				continue;
			}

			if (FD_ISSET(sockfd, &valfd)) {
			
				memset(buf, 0, DEFAULT);
				
				if (recv(sockfd, buf, DEFAULT, 0) == -1) {
					indicerr("close socket");
					close(sockfd);
					FD_CLR(sockfd, &readfd);
					clntarr[sockcnt] = -1;
				}

				printf("%d : %s\n", sockfd, buf);
				
				send(sockfd, buf, DEFAULT, 0);

				if (--state <= 0) {
					break;
				}
			}
		}

		for (sockcnt = 0; sockcnt <= clntmax; sockcnt++) {
			
			if (

			send(clntarr[sockcnt], buf, DEFAULT, 0);
		}

	}
	close(servfd);
	return 0;
}




void indicerr(char *msg) {
	fprintf(stderr, "%s : %s\n", msg, strerror(errno));
}
