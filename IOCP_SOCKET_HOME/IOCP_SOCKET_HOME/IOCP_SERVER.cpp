#include <WinSock2.h>
#include <stdio.h>
#include <process.h>
#include <Windows.h>
#include <vector>

#define PORT 10000
#define PACKET_LENGTH 1024
#define SEND 0
#define RECV 1
#define PUB "PUB"
#define SYM "SYM"
#define MSG "MSG"
#define EXT "EXT"

#pragma comment (lib, "Ws2_32.lib.")

// IOCP 객체에 소켓 등록 및 관리
typedef struct _SocketInfo
{
	SOCKET hSocket;
	SOCKADDR_IN tAddr;
}Socket_Info, * PSocket_Info;

// IOCP 객체가 소켓별 정보를 저장한다
typedef struct _DataInfo
{
	OVERLAPPED tOverlapped;
	char Buffer[PACKET_LENGTH];
	WSABUF wsaBuf;
	int rwMode;
	char* next_ptr;
	char* header;
	char* payload;
	// 구조체 생성자, 클래스 생성자랑 비슷하다
	_DataInfo()
	{
		memset(&tOverlapped, 0, sizeof(tOverlapped));
		memset(Buffer, 0, PACKET_LENGTH);
		memset(&wsaBuf, 0, sizeof(wsaBuf));
		rwMode = RECV;
		next_ptr = 0;
		header = 0;
		payload = 0;
	}
}Data_Info, * PData_Info;

int tempVar;

/*
IOCP 객체는 등록된 소켓을 알고 있겠지만
난 접속한 클라이언트를 알 수가 없다
fd_set에 접속한 클라이언트의 핸들을 받는다
*/
fd_set clnt_fd;

void ErrorHandler(const char*);
unsigned int __stdcall IocpThread(void*);

int main()
{
	WSADATA wsa;
	tempVar = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (tempVar != 0)
	{
		ErrorHandler("Initalize Error");
		return 1;
	}

	/*-CP object 를 생성하고 소켓을 CP에 등록하는 함수 CreateIoCopletionPort
		(   [in] HANDLE CreateIoCompletionPort(HANDLE FileHandle(소켓 핸들),
			[in, opt] HANDLE ExistingCompletionPort(Completion Port 전달),
			[in] ULONG_PTR Completionkey(전달되는 인자값, UNIQUE 하게 설정하기만 하면 된다 소켓마다 다르게 써야된다는 뜻),
			[in[ DWORD NumberOfConcurrentThreads(CP object에 할당되어 완료된 IO를 처리할 수 있는 쓰레드 전달, 0전달 시 동시에 실행 가능한 최대 수)
		);

	CP object 생성->HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);*/

	HANDLE hComport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (hComport == NULL)
	{
		ErrorHandler("CreateIoCompletionPort Error");
		WSACleanup();
		return 1;
	}

	// system info 구조체를 사용해 사용할 프로세서의 개수를 구한다 (thread pooling)
	SYSTEM_INFO tInfo;
	GetSystemInfo(&tInfo);

	for (DWORD i = 0; i < tInfo.dwNumberOfProcessors; i++)
	{
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, IocpThread, (HANDLE)hComport, 0, NULL);

		if (hThread == (HANDLE)0)
		{
			printf("thread pooling Error : %d", errno);
			WSACleanup();
			CloseHandle(hThread);
			return 1;
			// _beginthreadex 함수는 생성한 쓰레드가 종료되면 _endthreadex를 자동 호출한다 
		}
	}

	// IPPROTO_HOPOPTS 가 정수 0이다. 앞의 인자를 기준으로 결정, SOCK_STREAM이니 TCP
	SOCKET hserv_sock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (hserv_sock == INVALID_SOCKET)
	{
		ErrorHandler("create socket error");
		WSACleanup();
		return 1;
	}
	FD_ZERO(&clnt_fd);

	SOCKADDR_IN tAddr;
	memset(&tAddr, 0, sizeof(tAddr));
	tAddr.sin_family = AF_INET;
	tAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tAddr.sin_port = htons(PORT);

	tempVar = bind(hserv_sock, (SOCKADDR*)&tAddr, sizeof(tAddr));

	if (tempVar == SOCKET_ERROR)
	{
		ErrorHandler("bind error");
		closesocket(hserv_sock);
		WSACleanup();
		return 1;
	}

	// socket max connect == SOMAXCONN
	tempVar = listen(hserv_sock, SOMAXCONN);

	if (tempVar == SOCKET_ERROR)
	{
		ErrorHandler("listen error");
		closesocket(hserv_sock);
		WSACleanup();
		return 1;
	}

	printf("server start...\n");

	while (true)
	{
		SOCKET hSocket = INVALID_SOCKET;
		SOCKADDR_IN tClntAddr = { 0, };
		int Size = sizeof(tClntAddr);

		// IOCP 모델에서 main thread 의 역할은 accept, 클라이언트와 접속을 담당하는 역할을 수행한다
		hSocket = accept(hserv_sock, (SOCKADDR*)&tClntAddr, &Size);

		if (hSocket == INVALID_SOCKET)
		{
			ErrorHandler("Accept Error");
			continue;
		}

		// ====================================================================
		// 여기서 RSA 키 주고 sym 키 받기 인증되면 아래로 안되면 continue;

		FD_SET(hSocket, &clnt_fd);
		printf("%d has accept\n", hSocket);
		printf("%d this room count\n", clnt_fd.fd_count);

		// 새로운 클라이언트가 등록됐다. Socket_info 구조체를 할당해준다
		PSocket_Info pSockInfo = new Socket_Info;
		pSockInfo->hSocket = hSocket;
		memcpy(&pSockInfo->tAddr, &tClntAddr, Size);

		// 26라인 주석 참고
		HANDLE hTempVar = CreateIoCompletionPort((HANDLE)hSocket, hComport, (DWORD)pSockInfo, 0);

		if (hTempVar == NULL)
		{
			ErrorHandler("CreateIoCompletionPort Error");
			continue;
		}

		// Recv 호출용 구조체 또한 할당해준다. 정보는 이곳에 저장된다
		PData_Info pDataInfo = new Data_Info;
		pDataInfo->wsaBuf.len = PACKET_LENGTH;
		pDataInfo->wsaBuf.buf = pDataInfo->Buffer;

		DWORD dwFlag = 0;
		DWORD dwRecvBytes = 0;

		tempVar = WSARecv(pSockInfo->hSocket, &pDataInfo->wsaBuf, 1, &dwRecvBytes, &dwFlag, &pDataInfo->tOverlapped, NULL);
		if (tempVar == SOCKET_ERROR)
		{
			/*
			error code 997 즉시 완료할 수 없는 겹쳐진 작업
			Overlapped I/O 가 정상적으로 실행되고 있다는 것
			*/
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				ErrorHandler("RECV Error");
			}
		}
	}

	CloseHandle(hComport);
	closesocket(hserv_sock);
	WSACleanup();
	return 0;
}

unsigned int __stdcall IocpThread(void* pArg)
{
	// hComport는 IOCP 객체의 핸들이다 IOCP는 커널오브젝트다 핸들이 필요하다.
	HANDLE hComport = (HANDLE)pArg;
	DWORD dwByteTransfer = 0;
	PSocket_Info pSocketInfo = NULL;
	PData_Info pDataInfo = NULL;
	DWORD dwFlag = 0;
	char message[PACKET_LENGTH];

	//PULONG_PTR -> ULONG_PTR에 대한 포인터 DWORD가 unsigned long이니 PDWORD_PTR과 같다
	while (true)
	{
		/*
		-완료된 IO를 확인
		BOOL GetQueuedCompletionStatus([In]HANDLE CompletionPort(완료된 IO가 등록된 CP 핸들 전달)
		LPDWORD lpNumberOfBytesTrasferred(송수신된 데이터의 크기를 저장할 변수 주소)
		[out]PULONG_PTR lpCompletionKey(IOCP 연결을 구분하는 키값)
		[out]LPOVERLAPPED *lpOverlapped(WSASend, WSARecv 시 전달한 OVERLAPPED 구조체)
		[in]DWORD dwMilliseconds(Timeout 정보, INFINITE 전달 시 완료된 IO가 CP에 등록될때까지 blocking 상태로 대기)
		);
		GetQueuedCompletionStatus의 5번째 인자로 INFINITE가 들어갔으니 이 함수는 Blocking 함수
		해당 스레드를 통해 Completion Port에서 완료시그널이 들어오는지 계속 검사한다
		*/
		tempVar = GetQueuedCompletionStatus(hComport, &dwByteTransfer, (PULONG_PTR)&pSocketInfo, (LPOVERLAPPED*)&pDataInfo, INFINITE);
		// 함수 호출에 실패 또는 연결이 끊겼을 경우
		/* zero(FALSE)를 반환하므로 아래 처럼 했으나 호출실패 시
		네번째 매개변수 *lpOverlapped가 NULL이 될때, GetLastError 가 ERROR_ABANDONED_WAIT_0을 반환 할때로 예외를 처리할 수 있다*/

		if (pDataInfo->rwMode == RECV)
		{
			if (tempVar == 0 || dwByteTransfer == 0)
			{
				printf("%d has out of connect\n", (int)pSocketInfo->hSocket);
				closesocket(pSocketInfo->hSocket);
				FD_CLR(pSocketInfo->hSocket, &clnt_fd);
				// 당연히 소켓이 죽었으니 해당 소켓이 객체에서 가진 구조체들의 할당을 해제해준다.
				// C++ 문법이다 malloc, free 보다 new, delete 가 더 낫다고 한다
				delete pSocketInfo;
				delete pDataInfo;
				continue;
			}
			// 여기서 client 에게 wsasend
			memcpy(message, pDataInfo->wsaBuf.buf, PACKET_LENGTH);
			message[dwByteTransfer] = '\0';

			printf("%s", message);

			// 여기서 strtok
			pDataInfo->header = strtok_s(message, ":", &pDataInfo->next_ptr);
			if (!(strcmp(pDataInfo->header, MSG))) {
				printf("message recieve\n");
				pDataInfo->header = strtok_s(NULL, ":", &pDataInfo->next_ptr);
				pDataInfo->payload = pDataInfo->header;
				printf("message : %s\n", pDataInfo->payload);
				//memset(message, 0, PACKET_LENGTH);
				//memcpy(message, pDataInfo->payload, PACKET_LENGTH);
				//printf("%d\n", (int)sizeof(pDataInfo->payload));
				//message[(int)sizeof(pDataInfo->payload)] = '\0';
			}

			else if (!(strcmp(pDataInfo->header, PUB))) {
				printf("PUB recieve\n");
				pDataInfo->header = strtok_s(NULL, ":", &pDataInfo->next_ptr);
				pDataInfo->payload = pDataInfo->header;
				printf("PUB : %s\n", pDataInfo->payload);
				delete pDataInfo;
				continue;
			}

			else if (!(strcmp(pDataInfo->header, SYM))) {
				printf("SYM recieve\n");
				pDataInfo->header = strtok_s(NULL, ":", &pDataInfo->next_ptr);
				pDataInfo->payload = pDataInfo->header;
				printf("SYM : %s\n", pDataInfo->payload);
				delete pDataInfo;
				continue;
			}

			else if (!(strcmp(pDataInfo->header, EXT))) {
				printf("EXT recieve\n");
				pDataInfo->header = strtok_s(NULL, ":", &pDataInfo->next_ptr);
				pDataInfo->payload = pDataInfo->header;
				printf("EXT : %s\n", pDataInfo->payload);
				delete pDataInfo;
				continue;
			}
			/*
			클라이언트 데이터 구조체 정보를 바꿔줘야한다.
			pDataInfo 구조체가 해당 소켓의 정보를 저장한다
			절대 다른 소켓과 중첩해서 사용하면 안된다.
			send 와 receive 구조체도 함께 사용 할수 없다
			위의 할당과 해제는 모두 그 때문이다 free list, available list 를 사용하는게 좋다고 한다
			*/
			delete pDataInfo;

			for (int i = 0; i < (int)clnt_fd.fd_count; i++)
			{
				pDataInfo = (PData_Info)new Data_Info;
				pDataInfo->wsaBuf.len = (int)strlen(message);
				strcpy_s(pDataInfo->Buffer, message);
				pDataInfo->wsaBuf.buf = pDataInfo->Buffer;
				pDataInfo->rwMode = SEND;

				if (dwByteTransfer == 0)
				{
					printf("%d has out of connect\n", (int)clnt_fd.fd_array[i]);
					closesocket(pSocketInfo->hSocket);
					FD_CLR(clnt_fd.fd_array[i], &clnt_fd);
					delete pSocketInfo;
					delete pDataInfo;
					continue;
				}

				if (clnt_fd.fd_array[i] == pSocketInfo->hSocket)
				{
					continue;
				}

				tempVar = WSASend(clnt_fd.fd_array[i], &(pDataInfo->wsaBuf), 1, &dwByteTransfer, 0, &(pDataInfo->tOverlapped), NULL);

				if (tempVar == SOCKET_ERROR)
				{
					ErrorHandler("SEND Error");
				}
			}

			pDataInfo = (PData_Info)new Data_Info;
			pDataInfo->wsaBuf.len = PACKET_LENGTH;
			pDataInfo->wsaBuf.buf = pDataInfo->Buffer;

			tempVar = WSARecv(pSocketInfo->hSocket, &(pDataInfo->wsaBuf), 1, &dwByteTransfer, &dwFlag, &(pDataInfo->tOverlapped), NULL);

			if (tempVar == SOCKET_ERROR)
			{
				/*
				error code 997 즉시 완료할 수 없는 겹쳐진 작업
				Overlapped I/O 가 정상적으로 실행되고 있다는 것
				*/

				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					ErrorHandler("RECV Error");
				}
			}
		}
		else
		{
			/*
			GetQueuedCompletionStatus 함수가 스레드를 completion port 와 연결해 준다
			_DataInfo structure 를 보면 생성자로 rmMod를 RECV 로 초기화한다
			rmMod가 SEND라는 것은 어떤 작업이 진행됐던 구조체라는 것. 해제해준다
			*/
			delete pDataInfo;
		}
	}
	return 0;
}

void ErrorHandler(const char* msg)
{
	printf("%s : %d\n", msg, GetLastError());
}

/*
참조 - MicroSoft Docs, 윤성우의 열혈 TCP/IP 소켓 프로그래밍 23장
*/