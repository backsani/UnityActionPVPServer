#pragma once
#include <WinSock2.h>
#include <string>

#define PORT 9000
#define MAX_WORKERTHREAD 4
#define MAX_SOCKBUF 256
#define BUFSIZE 256

struct Transform
{
	float xPos;
	float yPos;
	float rotation;

	Transform()
	{
		xPos = 0.0f;
		yPos = 0.0f;
		rotation = 0.0f;
	}
};