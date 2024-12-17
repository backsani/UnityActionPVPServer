#pragma once
#include "pch.h"
#include <mutex>

enum class IOOperation
{
	RECV,
	SEND
};

struct mOverlappedEx
{
	WSAOVERLAPPED _wsaOverlapped;
	WSABUF _wsaBuf;
	IOOperation _operation;
};

class ClientInfo
{
	SOCKET _socketClient;
	char			mUserId[20];
	int				mSessionId;

	Transform		mTransform;
	float			Hp;
	int				mSessionUserIndex;


public:
	char			mRecvBuf[MAX_SOCKBUF]; //데이터 버퍼
	char			mSendBuf[MAX_SOCKBUF]; //데이터 버퍼
	bool isConnected;

	std::mutex mtx;


	mOverlappedEx _recvOverlappedEx;
	mOverlappedEx _sendOverlappedEx;

	ClientInfo() : mSessionId(-1), Hp(0), mSessionUserIndex(0), isConnected(true)
	{
		ZeroMemory(&_recvOverlappedEx, sizeof(mOverlappedEx));
		ZeroMemory(&_sendOverlappedEx, sizeof(mOverlappedEx));
		memset(mRecvBuf, 0, sizeof(mRecvBuf));
		memset(mSendBuf, 0, sizeof(mSendBuf));
		memcpy(this->mUserId, mUserId, sizeof(mUserId));
		_socketClient = INVALID_SOCKET;
		
	}

	void SetSocket(SOCKET socket) { _socketClient = socket; }
	void SetUserId(const char* userId) { 
		size_t len = strlen(userId);
		memcpy(mUserId, userId, len + 1);
	}

	SOCKET GetSocket() { return _socketClient; }
	char* GetUserId() { return mUserId; }

	int GetSessionId() { return mSessionId; }
	int GetsessionUserIndex() { return mSessionUserIndex; }
	float GetUserHp() { return Hp; }
	Transform GetUserTransform() { return mTransform; }

	void SetsessionUserIndex(int sessionUserIndex) { mSessionUserIndex = sessionUserIndex; }
	void SetSessionId(int sessionId) { mSessionId = sessionId; }
	void SetUserHp(float hp) { Hp = hp; }
	void SetUserTransform(Transform transform) { mTransform = transform; }

	char* GetClientInfo(char* clientInfo)
	{
		//세션 유저 아이디 | 좌표 | 체력 으로 배치
		int Length = 0;

		memcpy(clientInfo + Length, &mSessionUserIndex, sizeof(mSessionUserIndex));
		Length += sizeof(mSessionUserIndex);

		memcpy(clientInfo + Length, &mTransform, sizeof(mTransform));
		Length += sizeof(mTransform);

		memcpy(clientInfo + Length, &Hp, sizeof(mSessionId));
		Length += sizeof(Hp);

		return clientInfo;
	}

	int GetClientInfoLength()
	{
		return sizeof(mTransform) + sizeof(Hp) + sizeof(mSessionUserIndex);
	}

	void SetClientInfo(char* buffer)
	{
		int Length = 0;

		memcpy(&mSessionUserIndex, buffer + Length, sizeof(mSessionUserIndex));
		Length += sizeof(mSessionUserIndex);

		memcpy(&mTransform.xPos, buffer + Length, sizeof(mTransform.xPos));
		Length += sizeof(mTransform.xPos);

		memcpy(&mTransform.yPos, buffer + Length, sizeof(mTransform.yPos));
		Length += sizeof(mTransform.yPos);

		memcpy(&mTransform.rotation, buffer + Length, sizeof(mTransform.rotation));
		Length += sizeof(mTransform.rotation);

		memcpy(&Hp, buffer + Length, sizeof(Hp));
		Length += sizeof(Hp);
		return;
	}
};