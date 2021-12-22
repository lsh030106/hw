#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>
#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h>
#include <openssl/md5.h>
#include <fcntl.h>

#define BUFMAX 3080

typedef struct _userInfo {
	int clientSock;
	char userSymKey[100];
} userInfo;

int g_clientCnt;
userInfo g_clientSocks[100];
char g_svrPrivateKey[2048];
char g_svrPublicKey[512];
RSA *keypair;

void closeClientsSocks(void);
int userIn(int clientSock, struct sockaddr_in clientAddr);
int base64_encode(const char *buffer, char *result, int length);
int base64_decode(char *input, char *result, int length);
int getRsaKey(void);
int broadcast(char *msg, int client);
int aes128_decrypt(char *in, char *key, char *out);
int aes128_encrypt(char *in, char *key, char *out);
int RSADec(int enclen, unsigned char *ciphertxt, unsigned char *decrypt);
void *thread_main(void *sock);

int main(void) {
	int serverSock;
	int clientSock;
	int val;
	int fdcheck;
	int i, j;
	char encmsg[BUFMAX] = {0,};
	int sender = 0;
	
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;

	fd_set readfds;
	fd_set allfds;

	socklen_t clientAddrSize = sizeof(clientAddr);

	serverSock = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSock == -1) {
		printf("%s. %s(%d)\n", "socket error", strerror(errno), errno);
	}

	struct linger ling;
	ling.l_onoff = 1;
	ling.l_linger = 0;
	setsockopt(serverSock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(10000);

	val = bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

	if (val == -1) {
		printf("%s. %s(%d)\n", "bind error", strerror(errno), errno);
		close(serverSock);
		return -1;
	}

	printf("server connected\n");

	val = listen(serverSock, 100);

	if (val == -1) {
		printf("%s. %s(%d)\n", "listen error", strerror(errno), errno);
		close(serverSock);
		return -1;
	}

	// select 비동기 통신을 위한 fd 세팅
	FD_ZERO(&readfds);
	FD_SET(serverSock, &readfds);
	int maxfd = serverSock;
	val = getRsaKey();

	if (val != 0) {
		return -1;
	}

	while (1) {
		allfds = readfds;
		val = select(maxfd + 1, &allfds, NULL, NULL, NULL);

		if (val == -1) {
			printf("%s. %s(%d)\n", "select error", strerror(errno), errno);
			closeClientsSocks();
			close(serverSock);
			return -1;
		}

		// fdset에 저장된 소켓 중 server socket에 변화가 생겼을 경우 
		// 새로운 유저가 접속한 것이므로 accept 함수를 호출함.
		if (FD_ISSET(serverSock, &allfds)) {
			clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientAddrSize);
			if (clientSock == -1) {
				printf("%s. %s(%d)\n", "accept error", strerror(errno), errno);
				continue;
			}

			val = userIn(clientSock, clientAddr);

			// userIn 함수 내의 에러
			if (val == -1) {
				close(clientSock);
				continue;
			}

			FD_SET(clientSock, &readfds);
			maxfd = clientSock;
			continue;
		}

		for (i = 0; i < g_clientCnt; i++) {

			if (FD_ISSET(g_clientSocks[i].clientSock, &allfds)) {
				val = read(g_clientSocks[i].clientSock, encmsg, BUFMAX);

				if (val < 1) {
					close(g_clientSocks[i].clientSock);
					g_clientSocks[i].clientSock = 0;
					memset(g_clientSocks[i].userSymKey, 0, 100);
					g_clientCnt--;
					
					for (j = i; j < g_clientCnt; j++) {
						g_clientSocks[i] = g_clientSocks[j + 1];
					}		

					FD_ZERO(&readfds);
					FD_SET(serverSock, &readfds);

					for (i = 0; i < g_clientCnt; i++) {
						FD_SET(g_clientSocks[i].clientSock, &readfds);
						allfds = readfds;
					}

					printf("user out\n");
					break;
				}

				sender = i + 1;
				broadcast(encmsg, sender);
			}
		}
	}

	return 0;
}

int base64_encode(const char *buffer, char *result, int length) {
	BIO *bmem = NULL;	
	BIO *b64 = NULL;	
	BUF_MEM *bptr;		
	char *buff = NULL;
	b64 = BIO_new(BIO_f_base64());	 

	if (b64 == NULL) {
		printf("b64 BIO_new error\n");
		return -1;
	}

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);	
												
	bmem = BIO_new(BIO_s_mem());			

	if (bmem == NULL) {
		printf("bmem BIO_new error\n");
		BIO_free_all(b64);
		return -1;
	}

	b64 = BIO_push(b64, bmem);			
										

	BIO_write(b64, buffer, length);		
	BIO_flush(b64);					
	BIO_get_mem_ptr(b64, &bptr);

	BIO_set_close(b64, BIO_NOCLOSE);	
	buff = (char *)malloc(bptr -> length + 1);

	if (buff == NULL) {
		printf("%s. %s(%d)\n", "malloc error in b64 encoding; error", strerror(errno), errno);
		BIO_free_all(b64);
		return -1;
	}

	memcpy(buff, bptr -> data, bptr -> length);	
	buff[bptr -> length] = 0;
	BIO_free_all(b64);		

	strcpy(result, buff);
	free(buff);

	return 0;
}

// base64  decode 
int base64_decode(char *input, char *result, int length) {
	BIO *b64 = NULL;
	BIO *bmem = NULL;
	char *buffer = NULL;
	buffer = (char *)malloc(length);

	if (buffer == NULL) {
		printf("%s. %s(%d)\n", "malloc error in b64 decoding; error", strerror(errno), errno);
		return -1;
	}

	memset(buffer, 0, length);
	b64 = BIO_new(BIO_f_base64());

	if (b64 == NULL) {
		printf("b64 BIO_new error\n");
		free(buffer);
		return -1;
	}

	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	bmem = BIO_new_mem_buf(input, length);	
	bmem = BIO_push(b64, bmem);
	BIO_read(bmem, buffer, length);
	BIO_free_all(bmem);
	
	strcpy(result, buffer);
	free(buffer);

	return 0;
}

int aes128_decrypt(char *in, char *key, char *out) {
	int ret = 0;
	int len = 0;
	int len1 = 0;
	int len2 = 0;		

	unsigned char *result = NULL;
	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	if (ctx == NULL) {
		printf("EVP_CIPHER_CTX error\n");
		free(result);
		return 0;
	}

	ret = EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key, NULL);	// ret 변수에 encrypt init.

	if (ret != 1) {
		printf("EVP_DecryptInit_ex error\n");
		EVP_CIPHER_CTX_free(ctx);
		return 0;
	}

	result = (unsigned char *)malloc(AES_BLOCK_SIZE * 64);

	if (result == NULL) {
		EVP_CIPHER_CTX_free(ctx);
		fprintf(stderr, "fail malloc\n");
		return 0;
	}

    ret = EVP_DecryptUpdate(ctx, result, &len1, (const unsigned char*)in, strlen(in));

	if (ret != 1) {
		printf("EVP_DecryptUpdate error\n");
		EVP_CIPHER_CTX_free(ctx);
		free(result);
		return 0;
	}

	ret = EVP_DecryptFinal_ex(ctx, result + len1, &len2);

	if (ret != 1) {
		printf("EVP_DecryptFinal_ex error\n");
		EVP_CIPHER_CTX_free(ctx);
		free(result);
		return 0;
	}

	while (len < (len1 + len2)) {
		out[len] = result[len];
		len++;
	}

	EVP_CIPHER_CTX_free(ctx);
	free(result);
	return 1;
} 

int getRsaKey() {
	
    int keypair_;
	BIO *pri;
	BIO *pub;
	BIGNUM *bn;

	keypair = RSA_new();
	bn = BN_new();
	BN_set_word(bn, RSA_F4);

	keypair_ = RSA_generate_key_ex(keypair, 2048, bn, NULL);
	if (keypair_ == 0) {
		printf("RSAkeyerror\n");
		return -1;
	}

	pri = BIO_new(BIO_s_mem());
	if (pri == NULL) {
		printf("pri allocate error\n");
		return -1;
	}
		
	pub = BIO_new(BIO_s_mem());
	if (pub == NULL) {
		printf("pub allocate error\n");
		return -1;
	}

	PEM_write_bio_RSAPrivateKey(pri, keypair, NULL, NULL, 0, NULL, NULL);
	PEM_write_bio_RSAPublicKey(pub, keypair);

	size_t pri_len = BIO_pending(pri);
	size_t pub_len = BIO_pending(pub);

	BIO_read(pri, g_svrPrivateKey, (int)pri_len);
	BIO_read(pub, g_svrPublicKey, (int)pub_len);

	g_svrPrivateKey[pri_len] = '\0';
	g_svrPublicKey[pub_len] = '\0';

	return 0;
}

int userIn(int clientSock, struct sockaddr_in clientAddr) {
	char msg[BUFMAX];
	int msglen;
	char clientSymKey[256] = {0,};
	char decmsg[BUFMAX] = {0,};
	int val;
	int i;

	while(1) {
		// recv msg from client #hello
		memset(msg, 0, BUFMAX);
		val = read(clientSock, msg, BUFMAX);
		
		if (val == -1) {
			printf("%s. %s(%d)\n", "read error", strerror(errno), errno);
			return -1;
		}

		printf("%s\n", msg);

		if ((strncmp(msg, "#hello", 6)) == 0) {
			char okpubkey[BUFMAX] = {0,};
			strcat(okpubkey, "#OK");
			strcat(okpubkey, g_svrPublicKey);

			val = send(clientSock, okpubkey, BUFMAX, 0);

			if (val == -1) {
				printf("%s. %s(%d)\n", "send pubkey error", strerror(errno), errno);
				return -1;
			}
		}
		
		if (msg[1] == 'S' && msg[2] == 'Y' && msg[3] == 'M') {

			for (i = 0; msg[i + 3] != '\0'; ++i) {
				msg[i] = msg[i + 4];
			}

			msglen = RSA_private_decrypt(256, (unsigned char*)msg, (unsigned char*)clientSymKey, keypair, RSA_PKCS1_OAEP_PADDING);

			if (msglen != -1) {
				val = send(clientSock, "OK", 3, 0);

				if (val == -1) {
					printf("%s. %s(%d)\n", "send OK error", strerror(errno), errno);
					return -1;
				}

				userInfo temp;
				temp.clientSock = clientSock;
				strcpy(temp.userSymKey, clientSymKey);
				g_clientSocks[g_clientCnt++] = temp;

				printf("user sym key saved\n");
				return msglen;
			}

			break;
		}
	}

	printf("user in fail\n");
	return -1;
}

int RSADec(int enclen, unsigned char *ciphertxt, unsigned char *decrypt) {
	int declen = RSA_private_decrypt(enclen, (unsigned char*)ciphertxt, decrypt, keypair, RSA_PKCS1_OAEP_PADDING);

	if (declen == -1) {
		printf("%s. %s(%d)\n", "RSA decryption error", strerror(errno), errno);
		return -1;
	}

	return declen;
}

void closeClientsSocks() {
	int i;

	for (i = 0; i < g_clientCnt; i++) {
		close(g_clientSocks[i].clientSock);
	}
}

int broadcast(char *msg, int client) {
	int i;
	int val;
	char msgtemp[BUFMAX] = {0,};

	if (msg[0] == '#' && msg[1] == 'm' && msg[2] == 's' && msg[3] == 'g') {

		for (i = 0; msg[i + 3] != '\0'; i++) {
			msg[i] = msg[i + 4];
		}

		val = aes128_decrypt(msg, g_clientSocks[client - 1].userSymKey, msgtemp);

		if (val == -1) {
			return -1;
		}

		printf("recv message : %s\n", msgtemp);

		char *bmsg = (char *)malloc(sizeof(char) * strlen(msgtemp) + 15);
		memset(msg, 0, BUFMAX);
		memset(bmsg, 0, strlen(msgtemp) + 15);
		sprintf(bmsg, "client %d : ", client);
		strcat(bmsg, msgtemp);

		for (i = 0; i < g_clientCnt; i++) {
			aes128_encrypt(bmsg, g_clientSocks[i].userSymKey, msg);
			val = send(g_clientSocks[i].clientSock, msg, strlen(msg), 0);
			
			if (val == -1) {
				printf("%s. %s(%d)\n", "send error", strerror(errno), errno);
				printf("to client %d\n", i);
				return i;
			}
		}

		free(bmsg);
		return 0;
	}

	printf("invalid message\n");
	return -1;
}

int aes128_encrypt(char *in, char *key, char *out) {
	int ret = 0;
	int len = 0;
	int len1 = 0;
	int len2 = 0;	
	unsigned char *result = NULL;			// 암호화된 데이터 저장할 변수.
	EVP_CIPHER_CTX *ctx;				// cipher context 생성(openssl/evp.h)
	ctx = EVP_CIPHER_CTX_new();			// ctx에 새로운 메모리 할당 실패 시 NULL return

	if (ctx == NULL) {
		printf("EVP_CIPHER_CTX error\n");
		free(result);
		return 0;
	}
	ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)key, NULL);
	
    if (ret != 1) {		
		printf("EVP_EncryptInit_ex error\n");
		EVP_CIPHER_CTX_free(ctx);	// 구조체 메모리 해제
		return 0;
	}
    	
	result = (unsigned char *)malloc(AES_BLOCK_SIZE * 64);	// AES_BLOCK_SIZ = 16.
	
	if (result == NULL) {
		fprintf(stderr, "fail malloc\n");
		return 0;
	}

	ret = EVP_EncryptUpdate(ctx, result, &len1, (const unsigned char*)in, strlen(in)); 

	if (ret != 1) {
		printf("EVP_EncryptUpdate error\n");
		EVP_CIPHER_CTX_free(ctx);
		free(result);
		return 0;
	}

	ret = EVP_EncryptFinal_ex(ctx, result + len1, &len2);	

	if (ret != 1) {
		printf("EVP_EncryptFinal_ex error\n");
		EVP_CIPHER_CTX_free(ctx);
		free(result);
		return 0;
	}

	while (len < (len1 + len2)) {		 
		out[len] = result[len];		
        len++;				
	}

	EVP_CIPHER_CTX_free(ctx);
	free(result);
	return (len1 + len2);		
}
