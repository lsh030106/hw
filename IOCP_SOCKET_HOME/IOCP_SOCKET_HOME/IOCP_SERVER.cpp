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

// IOCP ��ü�� ���� ��� �� ����
typedef struct _SocketInfo
{
	SOCKET hSocket;
	SOCKADDR_IN tAddr;
}Socket_Info, * PSocket_Info;

// IOCP ��ü�� ���Ϻ� ������ �����Ѵ�
typedef struct _DataInfo
{
	OVERLAPPED tOverlapped;
	char Buffer[PACKET_LENGTH];
	WSABUF wsaBuf;
	int rwMode;
	char* next_ptr;
	char* header;
	char* payload;
	// ����ü ������, Ŭ���� �����ڶ� ����ϴ�
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
IOCP ��ü�� ��ϵ� ������ �˰� �ְ�����
�� ������ Ŭ���̾�Ʈ�� �� ���� ����
fd_set�� ������ Ŭ���̾�Ʈ�� �ڵ��� �޴´�
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

	/*-CP object �� �����ϰ� ������ CP�� ����ϴ� �Լ� CreateIoCopletionPort
		(   [in] HANDLE CreateIoCompletionPort(HANDLE FileHandle(���� �ڵ�),
			[in, opt] HANDLE ExistingCompletionPort(Completion Port ����),
			[in] ULONG_PTR Completionkey(���޵Ǵ� ���ڰ�, UNIQUE �ϰ� �����ϱ⸸ �ϸ� �ȴ� ���ϸ��� �ٸ��� ��ߵȴٴ� ��),
			[in[ DWORD NumberOfConcurrentThreads(CP object�� �Ҵ�Ǿ� �Ϸ�� IO�� ó���� �� �ִ� ������ ����, 0���� �� ���ÿ� ���� ������ �ִ� ��)
		);

	CP object ����->HANDLE hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);*/

	HANDLE hComport = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (hComport == NULL)
	{
		ErrorHandler("CreateIoCompletionPort Error");
		WSACleanup();
		return 1;
	}

	// system info ����ü�� ����� ����� ���μ����� ������ ���Ѵ� (thread pooling)
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
			// _beginthreadex �Լ��� ������ �����尡 ����Ǹ� _endthreadex�� �ڵ� ȣ���Ѵ� 
		}
	}

	// IPPROTO_HOPOPTS �� ���� 0�̴�. ���� ���ڸ� �������� ����, SOCK_STREAM�̴� TCP
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

		// IOCP �𵨿��� main thread �� ������ accept, Ŭ���̾�Ʈ�� ������ ����ϴ� ������ �����Ѵ�
		hSocket = accept(hserv_sock, (SOCKADDR*)&tClntAddr, &Size);

		if (hSocket == INVALID_SOCKET)
		{
			ErrorHandler("Accept Error");
			continue;
		}

		// ====================================================================
		// ���⼭ RSA Ű �ְ� sym Ű �ޱ� �����Ǹ� �Ʒ��� �ȵǸ� continue;

		FD_SET(hSocket, &clnt_fd);
		printf("%d has accept\n", hSocket);
		printf("%d this room count\n", clnt_fd.fd_count);

		// ���ο� Ŭ���̾�Ʈ�� ��ϵƴ�. Socket_info ����ü�� �Ҵ����ش�
		PSocket_Info pSockInfo = new Socket_Info;
		pSockInfo->hSocket = hSocket;
		memcpy(&pSockInfo->tAddr, &tClntAddr, Size);

		// 26���� �ּ� ����
		HANDLE hTempVar = CreateIoCompletionPort((HANDLE)hSocket, hComport, (DWORD)pSockInfo, 0);

		if (hTempVar == NULL)
		{
			ErrorHandler("CreateIoCompletionPort Error");
			continue;
		}

		// Recv ȣ��� ����ü ���� �Ҵ����ش�. ������ �̰��� ����ȴ�
		PData_Info pDataInfo = new Data_Info;
		pDataInfo->wsaBuf.len = PACKET_LENGTH;
		pDataInfo->wsaBuf.buf = pDataInfo->Buffer;

		DWORD dwFlag = 0;
		DWORD dwRecvBytes = 0;

		tempVar = WSARecv(pSockInfo->hSocket, &pDataInfo->wsaBuf, 1, &dwRecvBytes, &dwFlag, &pDataInfo->tOverlapped, NULL);
		if (tempVar == SOCKET_ERROR)
		{
			/*
			error code 997 ��� �Ϸ��� �� ���� ������ �۾�
			Overlapped I/O �� ���������� ����ǰ� �ִٴ� ��
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
	// hComport�� IOCP ��ü�� �ڵ��̴� IOCP�� Ŀ�ο�����Ʈ�� �ڵ��� �ʿ��ϴ�.
	HANDLE hComport = (HANDLE)pArg;
	DWORD dwByteTransfer = 0;
	PSocket_Info pSocketInfo = NULL;
	PData_Info pDataInfo = NULL;
	DWORD dwFlag = 0;
	char message[PACKET_LENGTH];

	//PULONG_PTR -> ULONG_PTR�� ���� ������ DWORD�� unsigned long�̴� PDWORD_PTR�� ����
	while (true)
	{
		/*
		-�Ϸ�� IO�� Ȯ��
		BOOL GetQueuedCompletionStatus([In]HANDLE CompletionPort(�Ϸ�� IO�� ��ϵ� CP �ڵ� ����)
		LPDWORD lpNumberOfBytesTrasferred(�ۼ��ŵ� �������� ũ�⸦ ������ ���� �ּ�)
		[out]PULONG_PTR lpCompletionKey(IOCP ������ �����ϴ� Ű��)
		[out]LPOVERLAPPED *lpOverlapped(WSASend, WSARecv �� ������ OVERLAPPED ����ü)
		[in]DWORD dwMilliseconds(Timeout ����, INFINITE ���� �� �Ϸ�� IO�� CP�� ��ϵɶ����� blocking ���·� ���)
		);
		GetQueuedCompletionStatus�� 5��° ���ڷ� INFINITE�� ������ �� �Լ��� Blocking �Լ�
		�ش� �����带 ���� Completion Port���� �Ϸ�ñ׳��� �������� ��� �˻��Ѵ�
		*/
		tempVar = GetQueuedCompletionStatus(hComport, &dwByteTransfer, (PULONG_PTR)&pSocketInfo, (LPOVERLAPPED*)&pDataInfo, INFINITE);
		// �Լ� ȣ�⿡ ���� �Ǵ� ������ ������ ���
		/* zero(FALSE)�� ��ȯ�ϹǷ� �Ʒ� ó�� ������ ȣ����� ��
		�׹�° �Ű����� *lpOverlapped�� NULL�� �ɶ�, GetLastError �� ERROR_ABANDONED_WAIT_0�� ��ȯ �Ҷ��� ���ܸ� ó���� �� �ִ�*/

		if (pDataInfo->rwMode == RECV)
		{
			if (tempVar == 0 || dwByteTransfer == 0)
			{
				printf("%d has out of connect\n", (int)pSocketInfo->hSocket);
				closesocket(pSocketInfo->hSocket);
				FD_CLR(pSocketInfo->hSocket, &clnt_fd);
				// �翬�� ������ �׾����� �ش� ������ ��ü���� ���� ����ü���� �Ҵ��� �������ش�.
				// C++ �����̴� malloc, free ���� new, delete �� �� ���ٰ� �Ѵ�
				delete pSocketInfo;
				delete pDataInfo;
				continue;
			}
			// ���⼭ client ���� wsasend
			memcpy(message, pDataInfo->wsaBuf.buf, PACKET_LENGTH);
			message[dwByteTransfer] = '\0';

			printf("%s", message);

			// ���⼭ strtok
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
			Ŭ���̾�Ʈ ������ ����ü ������ �ٲ�����Ѵ�.
			pDataInfo ����ü�� �ش� ������ ������ �����Ѵ�
			���� �ٸ� ���ϰ� ��ø�ؼ� ����ϸ� �ȵȴ�.
			send �� receive ����ü�� �Բ� ��� �Ҽ� ����
			���� �Ҵ�� ������ ��� �� �����̴� free list, available list �� ����ϴ°� ���ٰ� �Ѵ�
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
				error code 997 ��� �Ϸ��� �� ���� ������ �۾�
				Overlapped I/O �� ���������� ����ǰ� �ִٴ� ��
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
			GetQueuedCompletionStatus �Լ��� �����带 completion port �� ������ �ش�
			_DataInfo structure �� ���� �����ڷ� rmMod�� RECV �� �ʱ�ȭ�Ѵ�
			rmMod�� SEND��� ���� � �۾��� ����ƴ� ����ü��� ��. �������ش�
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
���� - MicroSoft Docs, �������� ���� TCP/IP ���� ���α׷��� 23��
*/