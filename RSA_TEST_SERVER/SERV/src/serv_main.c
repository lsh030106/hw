#include <sys/socket.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

//#include "AES_CR.h" // AES 128 ecb key 파일 읽고 암복호화, 파라미터는 파일이름
//#include "AES_keygen.h" // AES 128 ecb key 생성 함수
#include "RSA_CR.h" // RSA 암복호화 파라미터는 마찬가지로 파일이름
#include "RSA_keygen.h" // RSA 키 생성함수
#include "BASE_64.h"

#define SERVER_PORT 10011
#define DEFAULT 2048
#define LIMIT 1024
#define EXIT "q\n"

#define OKL "OK?"
#define PUB "PUB"
#define SYM "SYM"
#define MSG "MSG"

/*
 소켓들은 IP 로 구분하며 IP 별 1회 인증, 사용자 본인인증은 클라이언트 프로그램에서 자체적으로 로그인을 해 확인한다.
 서버는 클라이언트의 Public key 만을 .pem 파일로 저장하며 pem파일의 이름은 상대방의 IP 주소,
 즉, 로그인 인증(AES ECB 암호화), RSA IKE, 공유키교환, IP 확인 4개의 인증이 있으며 하나라도
 일치하지 않을 시 이용이 제한된다

 모두 서버에 접속은 되지만 채팅은 인증이 완료된 사람에게만 보이게 했다
 마치 vlan처럼 같은 공간에 있지만 다른 공간으로 인식하게 했다
 하지만 인증되지 않은 사람이 많이 모인다면 서버가 혼잡해질 가능성이 다분하므로 
 총 4개의 배열을 이용한다

 접속 대기중인 소켓의 배열, 인증이 완료된 소켓의 배열, 접속 중인 소켓의 배열, 접속된 소켓의 IP
 이 네가지의 배열을 이용해 서버의 우선순위를 정한다 
 서버의 모델은 selete 모델이며 쓰레드풀을 쓰기엔 시간이 너무 걸려 select를 선택했다
 */


void indicerr(char *);
int generate_key(void);
int cat_header(char *sendbuf, const char *header, char *payload);

int main(void) {
	int servfd, clntfd, sockfd, maxfd;
	char *wclntarr[12];	// 접속 대기중인 소켓의 배열
    //int rclntarr[LIMIT];    // 인증 완료한 소켓의 배열
	int clntarr[LIMIT];    // 접속 중인 소켓의 배열
    char *iparr[12]; // 접속 된 소켓의 IP
    int sockcnt = 0;
	int state, len = 0;
	int clntmax = -1;
	int clntdn = 1;
    char *header;            

    char clnt_ip[12];
    char clnt_pubkey_filename[24];
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
    
    make_key("10.10.0.96");
    
    RSA_pubkey_read("public/10.10.0.96.pem", sendbuf);

    printf("%s", sendbuf);
    printf("start server...\n");

	while (1) {
		valfd = readfd;

		state = select(maxfd + 1, &valfd, 0, 0, 0);
		memset(buf, 0, DEFAULT);

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

				if (clntarr[sockcnt] < 0) {
					clntarr[sockcnt] = clntfd;
                    wclntarr[clntfd] = iparr[clntfd];
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

        
		// 클라이언트 접속 종료 예외 처리 및 키교환
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
                
                if (len > 0) {
                    header = strtok(buf, ":");
                    char load[DEFAULT] = {0,};
                    char *clnt_pay;
                    
                    printf("%s\n", sendbuf);
                    
                    // 클라이언트에서 요청시 공개키 전송
                    if (! (strcmp(header, OKL))) {
                        memset(load, 0, DEFAULT);
                        RSA_pubkey_read("public/10.10.0.96.pem", load);
                        cat_header(sendbuf, PUB, load);
                        send(sockfd, sendbuf, DEFAULT, 0);
                    }
                    
                    // 클라이언트에서 자신의 공개키 전송 시 저장 후 SYM 키 전송
                    if (! (strcmp(header, PUB))) {
                        char *base;

                        memset(clnt_pubkey_filename, 0, sizeof(clnt_pubkey_filename));
                        char *IP = iparr[sockfd];
                        sprintf(clnt_pubkey_filename, "public/%s.pem", IP); 
                        memset(load, 0, DEFAULT);
                    
                        header = strtok(NULL, ":");
                        clnt_pay = header;
                        
                        RSA_pubkey_write(clnt_pubkey_filename, clnt_pay);
                    
                        printf("%s\n", clnt_pay);
                        

                        unsigned char *msg = "test rsa test test rsa test";
                        public_enc(msg, (unsigned char *)load, clnt_pubkey_filename);
                        //base64Encode((const char *)load, &base);

                        //printf("%s\n", base);
                        cat_header(sendbuf, SYM, load);
                        printf("%s\n", sendbuf);
                        send(sockfd, sendbuf, DEFAULT, 0);
                    }
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
                /*char **protocol = NULL;

                char *temp = strtok_r(sendbuf, ":", protocol);
                // : 앞의 문자열으로 단계를 4단계로 나누어 handshake 한다*/


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

//int generate_key(void) {
    /* 서버 첫 시작 시 비대칭키와 공유키를 생성 후 해당 pem, key  파일 존재 시
       새로 생성하지 않고 가져오기만 한다
       */
//}

int cat_header(char *sendbuf, const char *header, char *payload) {
    char *temp;

    temp = strcat(strcat(strcat(sendbuf, header), ":"), payload);
    
    if (temp == NULL) {
        return -1;
    }

    return 0;
} 
