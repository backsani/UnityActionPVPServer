#include "Server.h"

/*
��Ŷ �߰� �� ���� �����ڿ��� ��Ŷ ���Ϳ� �߰���������Ѵ�.
*/
Server::Server() {
	this->packet.push_back(std::make_shared<LoginPacketMaker>());
	this->packet.push_back(std::make_shared<MatchPacketMaker>());
	this->packet.push_back(std::make_shared<IngamePacketMaker>());
	// ���� �ʱ�ȭ
	mGameSessionManager = std::make_shared<GameSessionManager>(this, 4);
	mMatchManager = std::make_shared<MatchManager>(mGameSessionManager);
	
}

Server::~Server()
{
	// closesocket()
	closesocket(listenSocket);

	// ���� ����
	WSACleanup();
}

bool Server::InitSocket()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("[����] WSAStartup() �Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("[����] WSASocket() �Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("���� �ʱ�ȭ ����\n");
	return true;
}

bool Server::BindSocket()
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT); //���� ��Ʈ�� �����Ѵ�.				
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//������ ������ ���� �ּ� ������ cIOCompletionPort ������ �����Ѵ�.
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)))
	{
		printf("[����] bind()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	if (listen(listenSocket, 5))
	{
		printf("[����] listen()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	printf("���� ��� ����..\n");
	return true;
}

bool Server::StartServer(const UINT32 maxClientCount)
{
	Server::CreateClient(maxClientCount);

	mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
	if (mIOCPHandle == NULL)
	{
		printf("[����] CreateIoCompletionPort() �Լ� ����: %d\n", GetLastError());
		return false;
	}

	if (!CreateWokerThread()) {
		printf("[����] CreateWokerThread() �Լ� ����: %d\n", GetLastError());
		return false;
	}

	if (!CreateAccepterThread()) {
		printf("[����] CreateAccepterThread() �Լ� ����: %d\n", GetLastError());
		return false;
	}

	printf("���� ����\n");
	return true;
}

void Server::CreateClient(const UINT32 maxClientCount)
{
	for (UINT32 i = 0; i < maxClientCount; i++)
	{
		mClientInfos.emplace_back();
	}
}


bool Server::CreateWokerThread()
{
	unsigned int uiThreadId = 0;
	//WaingThread Queue�� ��� ���·� ���� ������� ���� ����Ǵ� ���� : (cpu���� * 2) + 1 
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	printf("WokerThread ����..\n");
	return true;
}

bool Server::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	printf("AccepterThread ����..\n");
	return true;
}

void Server::WokerThread()
{
	ClientInfo* pClientInfo = nullptr;
	BOOL bSuccess = TRUE;
	DWORD dwIoSize = 0;
	LPOVERLAPPED lpOverlapped = NULL;
	HeaderType currentHeader;
	Buffer_Converter bufferCon;
	int bufSize = 0;


	while (mIsWorkerRun)
	{
		bSuccess = GetQueuedCompletionStatus(mIOCPHandle,
			&dwIoSize,					// ������ ���۵� ����Ʈ
			(PULONG_PTR)&pClientInfo,		// CompletionKey
			&lpOverlapped,				// Overlapped IO ��ü
			INFINITE);					// ����� �ð�

		//����� ������ ���� �޼��� ó��..
		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client�� ������ ��������..			
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) ���� ����\n", (int)pClientInfo->GetSocket());
			CloseSocket(pClientInfo);
			continue;
		}


		auto pOverlappedEx = (mOverlappedEx*)lpOverlapped;

		currentHeader = bufferCon.GetHeader(pClientInfo->mRecvBuf);


		/*------------------
				������
		------------------*/


		//Overlapped I/O Recv�۾� ��� �� ó��
		if (IOOperation::RECV == pOverlappedEx->_operation)
		{
			//packet[currentHeader]->Deserialzed(pClientInfo->mRecvBuf);

			switch (currentHeader)
			{
			case HeaderType::ACCEPT:
				{
					auto loginPacket = static_cast<LoginPacketMaker*>(packet[currentHeader].get());

					loginPacket->Deserialzed(pClientInfo->mRecvBuf);

					//ConnectionState�� ���� �α������� ȸ���������� ó��

					pClientInfo->SetUserId(loginPacket->GetUserId());

					printf("[���� ����] msg : %s\n", pClientInfo->GetUserId());
					loginPacket->SetConnectionInfo(ConnectionState::LOGIN_SUCCESS);

					int sendLength = strlen(pClientInfo->GetUserId());
					memcpy(&pClientInfo->mSendBuf, pClientInfo->GetUserId(), sendLength);
				}
				break;
			case HeaderType::MATCH:
				{
					auto matchPacket = static_cast<MatchPacketMaker*>(packet[currentHeader].get());

					matchPacket->Deserialzed(pClientInfo->mRecvBuf);

					if (matchPacket->GetConnectionInfo() == MATCH_REQUEST)
					{
						printf("[��Ī �õ�] msg : %s\n", pClientInfo->GetUserId());

						//������ ����� ������ ��, 2���� ��Ī �Ǹ� ���� ���̵� ��ȯ, �ƴϸ� 0 ��ȯ
						int currentSessionId = mMatchManager->AddClientQueue(pClientInfo);

						//��Ī �۾� 0 ��ȯ �� 2�� ���� �� ����.
						if (currentSessionId != 0)
						{
							matchPacket->SetConnectionInfo(MATCH_ACCEPT);
							mGameSessionManager->ProcessingSessionPacket(currentSessionId, matchPacket);
							continue;
						}
						else {
							//��Ī ��� ��ȯ
							matchPacket->SetConnectionInfo(ConnectionState::MATCH_FIND);

							ConnectionState currentState = matchPacket->GetConnectionInfo();

							memset(&pClientInfo->mSendBuf, 0, strlen(pClientInfo->mSendBuf));
							memcpy(&pClientInfo->mSendBuf, &currentState, sizeof(currentState));
						}
						
					}

				
				}
				
				break;
			case HeaderType::INGAME:
				{
					auto ingamePacket = static_cast<IngamePacketMaker*>(packet[currentHeader].get());

					ingamePacket->Deserialzed(pClientInfo->mRecvBuf);
					pClientInfo->SetClientInfo(pClientInfo->mRecvBuf);

					mGameSessionManager->ProcessingSessionPacket(pClientInfo->GetSessionId(), ingamePacket);
				}
				break;
			default:
				printf("[����] not exists Packet\n");
				break;
			}

			/*strcpy(pClientInfo->mRecvBuf, packet[currentHeader]->GetBuffer());*/


			//Ŭ���̾�Ʈ�� �޼����� �����Ѵ�.
			int bufLength = packet[currentHeader]->Serialzed(pClientInfo->mSendBuf, strlen(pClientInfo->mSendBuf));
			//int bufLength = packet[currentHeader]->Serialaze(packet[currentHeader]->GetBuffer());

			SendMsg(pClientInfo, pClientInfo->mSendBuf, bufLength);

			BindRecv(pClientInfo);
		}
		//Overlapped I/O Send�۾� ��� �� ó��
		else if (IOOperation::SEND == pOverlappedEx->_operation)
		{
			printf("[�۽�] %d : %s\n", pClientInfo->GetSocket(), pClientInfo->mSendBuf);
		}
		//���� ��Ȳ
		else
		{
			printf("socket(%d)���� ���ܻ�Ȳ\n", (int)pClientInfo->GetSocket());
		}

	}
}


void Server::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	while (mIsAccepterRun)
	{
		//������ ���� ����ü�� �ε����� ���´�.
		ClientInfo* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("[����] Client Full\n");
			return;
		}

		//Ŭ���̾�Ʈ ���� ��û�� ���� ������ ��ٸ���.
		pClientInfo->SetSocket(accept(listenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen));
		if (INVALID_SOCKET == pClientInfo->GetSocket())
		{
			continue;
		}

		//I/O Completion Port��ü�� ������ �����Ų��.
		if (!BindIOCompletionPort(pClientInfo))
		{
			return;
		}

		//Recv Overlapped I/O�۾��� ��û�� ���´�.
		if (!BindRecv(pClientInfo))
		{
			return;
		}

		printf("Ŭ���̾�Ʈ ���� : SOCKET(%d)\n", (int)pClientInfo->GetSocket());

		//Ŭ���̾�Ʈ ���� ����
		//++mClientCnt;
	}
}

ClientInfo* Server::GetEmptyClientInfo()
{
	for (auto& client : mClientInfos)
	{
		if (INVALID_SOCKET == client.GetSocket())
		{
			return &client;
		}
	}

	return nullptr;
}

void Server::DestroyThread()
{
	mIsWorkerRun = false;
	CloseHandle(mIOCPHandle);

	for (auto& th : mWorkerThreads)
	{
		if (th.joinable())
		{
			th.join();
		}
	}

	mIsAccepterRun = false;
	closesocket(listenSocket);

	if (mAccepterThread.joinable())
	{
		mAccepterThread.join();
	}
}

bool Server::BindIOCompletionPort(ClientInfo* pClientInfo)
{
	//socket�� pClientInfo�� CompletionPort��ü�� �����Ų��.
	auto hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->GetSocket()
		, mIOCPHandle
		, (ULONG_PTR)(pClientInfo), 0);

	if (NULL == hIOCP || mIOCPHandle != hIOCP)
	{
		printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
		return false;
	}

	return true;
}


//WSARecv Overlapped I/O �۾��� ��Ų��.
bool Server::BindRecv(ClientInfo* pClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->_recvOverlappedEx._wsaBuf.len = MAX_SOCKBUF;
	pClientInfo->_recvOverlappedEx._wsaBuf.buf = pClientInfo->mRecvBuf;
	pClientInfo->_recvOverlappedEx._operation = IOOperation::RECV;

	int nRet = WSARecv(pClientInfo->GetSocket(),
		&(pClientInfo->_recvOverlappedEx._wsaBuf),
		1,
		&dwRecvNumBytes,
		&dwFlag,
		(LPWSAOVERLAPPED) & (pClientInfo->_recvOverlappedEx),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

//WSASend Overlapped I/O�۾��� ��Ų��.
bool Server::SendMsg(ClientInfo* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	//���۵� �޼����� ����
	CopyMemory(pClientInfo->mSendBuf, pMsg, nLen);
	pClientInfo->mSendBuf[nLen] = '\0';


	//Overlapped I/O�� ���� �� ������ ������ �ش�.
	pClientInfo->_sendOverlappedEx._wsaBuf.len = nLen;
	pClientInfo->_sendOverlappedEx._wsaBuf.buf = pClientInfo->mSendBuf;
	pClientInfo->_sendOverlappedEx._operation = IOOperation::SEND;

	int nRet = WSASend(pClientInfo->GetSocket(),
		&(pClientInfo->_sendOverlappedEx._wsaBuf),
		1,
		&dwRecvNumBytes,
		0,
		(LPWSAOVERLAPPED) & (pClientInfo->_sendOverlappedEx),
		NULL);

	//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}



void Server::CloseSocket(ClientInfo* pClientInfo)
{
	bool bIsForce = false;
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

	// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. ���� : ������ �ս��� ������ ���� 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
	shutdown(pClientInfo->GetSocket(), SD_BOTH);

	//���� �ɼ��� �����Ѵ�.
	setsockopt(pClientInfo->GetSocket(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//���� ������ ���� ��Ų��. 
	closesocket(pClientInfo->GetSocket());

	pClientInfo->SetSocket(INVALID_SOCKET);
}