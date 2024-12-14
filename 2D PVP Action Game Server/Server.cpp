#include "Server.h"

/*
패킷 추가 시 서버 생성자에서 패킷 벡터에 추가시켜줘야한다.
*/
Server::Server() {
	this->packet.push_back(std::make_shared<LoginPacketMaker>());
	this->packet.push_back(std::make_shared<MatchPacketMaker>());
	this->packet.push_back(std::make_shared<IngamePacketMaker>());
	// 윈속 초기화
	mGameSessionManager = std::make_shared<GameSessionManager>(this, 4);
	mMatchManager = std::make_shared<MatchManager>(mGameSessionManager);
	
}

Server::~Server()
{
	// closesocket()
	closesocket(listenSocket);

	// 윈속 종료
	WSACleanup();
}

bool Server::InitSocket()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("[에러] WSAStartup() 함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("[에러] WSASocket() 함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("소켓 초기화 성공\n");
	return true;
}

bool Server::BindSocket()
{
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT); //서버 포트를 설정한다.				
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//위에서 지정한 서버 주소 정보와 cIOCompletionPort 소켓을 연결한다.
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)))
	{
		printf("[에러] bind()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	if (listen(listenSocket, 5))
	{
		printf("[에러] listen()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	printf("서버 등록 성공..\n");
	return true;
}

bool Server::StartServer(const UINT32 maxClientCount)
{
	Server::CreateClient(maxClientCount);

	mIOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, MAX_WORKERTHREAD);
	if (mIOCPHandle == NULL)
	{
		printf("[에러] CreateIoCompletionPort() 함수 실패: %d\n", GetLastError());
		return false;
	}

	if (!CreateWokerThread()) {
		printf("[에러] CreateWokerThread() 함수 실패: %d\n", GetLastError());
		return false;
	}

	if (!CreateAccepterThread()) {
		printf("[에러] CreateAccepterThread() 함수 실패: %d\n", GetLastError());
		return false;
	}

	printf("서버 시작\n");
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
	//WaingThread Queue에 대기 상태로 넣을 쓰레드들 생성 권장되는 개수 : (cpu개수 * 2) + 1 
	for (int i = 0; i < MAX_WORKERTHREAD; i++)
	{
		mWorkerThreads.emplace_back([this]() { WokerThread(); });
	}

	printf("WokerThread 시작..\n");
	return true;
}

bool Server::CreateAccepterThread()
{
	mAccepterThread = std::thread([this]() { AccepterThread(); });

	printf("AccepterThread 시작..\n");
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
			&dwIoSize,					// 실제로 전송된 바이트
			(PULONG_PTR)&pClientInfo,		// CompletionKey
			&lpOverlapped,				// Overlapped IO 객체
			INFINITE);					// 대기할 시간

		//사용자 쓰레드 종료 메세지 처리..
		if (TRUE == bSuccess && 0 == dwIoSize && NULL == lpOverlapped)
		{
			mIsWorkerRun = false;
			continue;
		}

		if (NULL == lpOverlapped)
		{
			continue;
		}

		//client가 접속을 끊었을때..			
		if (FALSE == bSuccess || (0 == dwIoSize && TRUE == bSuccess))
		{
			printf("socket(%d) 접속 끊김\n", (int)pClientInfo->GetSocket());
			CloseSocket(pClientInfo);
			continue;
		}


		auto pOverlappedEx = (mOverlappedEx*)lpOverlapped;

		currentHeader = bufferCon.GetHeader(pClientInfo->mRecvBuf);


		/*------------------
				공사중
		------------------*/


		//Overlapped I/O Recv작업 결과 뒤 처리
		if (IOOperation::RECV == pOverlappedEx->_operation)
		{
			//packet[currentHeader]->Deserialzed(pClientInfo->mRecvBuf);

			switch (currentHeader)
			{
			case HeaderType::ACCEPT:
				{
					auto loginPacket = static_cast<LoginPacketMaker*>(packet[currentHeader].get());

					loginPacket->Deserialzed(pClientInfo->mRecvBuf);

					//ConnectionState를 보고 로그인인지 회원가입인지 처리

					pClientInfo->SetUserId(loginPacket->GetUserId());

					printf("[유저 접속] msg : %s\n", pClientInfo->GetUserId());
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
						printf("[매칭 시도] msg : %s\n", pClientInfo->GetUserId());

						//세션이 만들어 졌으면 즉, 2명이 매칭 되면 세션 아이디 반환, 아니면 0 반환
						int currentSessionId = mMatchManager->AddClientQueue(pClientInfo);

						//매칭 작업 0 반환 시 2명 아직 안 모임.
						if (currentSessionId != 0)
						{
							matchPacket->SetConnectionInfo(MATCH_ACCEPT);
							mGameSessionManager->ProcessingSessionPacket(currentSessionId, matchPacket);
							continue;
						}
						else {
							//매칭 결과 반환
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
				printf("[오류] not exists Packet\n");
				break;
			}

			/*strcpy(pClientInfo->mRecvBuf, packet[currentHeader]->GetBuffer());*/


			//클라이언트에 메세지를 에코한다.
			int bufLength = packet[currentHeader]->Serialzed(pClientInfo->mSendBuf, strlen(pClientInfo->mSendBuf));
			//int bufLength = packet[currentHeader]->Serialaze(packet[currentHeader]->GetBuffer());

			SendMsg(pClientInfo, pClientInfo->mSendBuf, bufLength);

			BindRecv(pClientInfo);
		}
		//Overlapped I/O Send작업 결과 뒤 처리
		else if (IOOperation::SEND == pOverlappedEx->_operation)
		{
			printf("[송신] %d : %s\n", pClientInfo->GetSocket(), pClientInfo->mSendBuf);
		}
		//예외 상황
		else
		{
			printf("socket(%d)에서 예외상황\n", (int)pClientInfo->GetSocket());
		}

	}
}


void Server::AccepterThread()
{
	SOCKADDR_IN		stClientAddr;
	int nAddrLen = sizeof(SOCKADDR_IN);

	while (mIsAccepterRun)
	{
		//접속을 받을 구조체의 인덱스를 얻어온다.
		ClientInfo* pClientInfo = GetEmptyClientInfo();
		if (NULL == pClientInfo)
		{
			printf("[에러] Client Full\n");
			return;
		}

		//클라이언트 접속 요청이 들어올 때까지 기다린다.
		pClientInfo->SetSocket(accept(listenSocket, (SOCKADDR*)&stClientAddr, &nAddrLen));
		if (INVALID_SOCKET == pClientInfo->GetSocket())
		{
			continue;
		}

		//I/O Completion Port객체와 소켓을 연결시킨다.
		if (!BindIOCompletionPort(pClientInfo))
		{
			return;
		}

		//Recv Overlapped I/O작업을 요청해 놓는다.
		if (!BindRecv(pClientInfo))
		{
			return;
		}

		printf("클라이언트 접속 : SOCKET(%d)\n", (int)pClientInfo->GetSocket());

		//클라이언트 갯수 증가
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
	//socket과 pClientInfo를 CompletionPort객체와 연결시킨다.
	auto hIOCP = CreateIoCompletionPort((HANDLE)pClientInfo->GetSocket()
		, mIOCPHandle
		, (ULONG_PTR)(pClientInfo), 0);

	if (NULL == hIOCP || mIOCPHandle != hIOCP)
	{
		printf("[에러] CreateIoCompletionPort()함수 실패: %d\n", GetLastError());
		return false;
	}

	return true;
}


//WSARecv Overlapped I/O 작업을 시킨다.
bool Server::BindRecv(ClientInfo* pClientInfo)
{
	DWORD dwFlag = 0;
	DWORD dwRecvNumBytes = 0;

	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
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

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSARecv()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}

	return true;
}

//WSASend Overlapped I/O작업을 시킨다.
bool Server::SendMsg(ClientInfo* pClientInfo, char* pMsg, int nLen)
{
	DWORD dwRecvNumBytes = 0;

	//전송될 메세지를 복사
	CopyMemory(pClientInfo->mSendBuf, pMsg, nLen);
	pClientInfo->mSendBuf[nLen] = '\0';


	//Overlapped I/O을 위해 각 정보를 셋팅해 준다.
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

	//socket_error이면 client socket이 끊어진걸로 처리한다.
	if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
	{
		printf("[에러] WSASend()함수 실패 : %d\n", WSAGetLastError());
		return false;
	}
	return true;
}



void Server::CloseSocket(ClientInfo* pClientInfo)
{
	bool bIsForce = false;
	struct linger stLinger = { 0, 0 };	// SO_DONTLINGER로 설정

	// bIsForce가 true이면 SO_LINGER, timeout = 0으로 설정하여 강제 종료 시킨다. 주의 : 데이터 손실이 있을수 있음 
	if (true == bIsForce)
	{
		stLinger.l_onoff = 1;
	}

	//socketClose소켓의 데이터 송수신을 모두 중단 시킨다.
	shutdown(pClientInfo->GetSocket(), SD_BOTH);

	//소켓 옵션을 설정한다.
	setsockopt(pClientInfo->GetSocket(), SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

	//소켓 연결을 종료 시킨다. 
	closesocket(pClientInfo->GetSocket());

	pClientInfo->SetSocket(INVALID_SOCKET);
}