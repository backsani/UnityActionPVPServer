#include "LogManager.h"
#include "ClientInfo.h"

LogManager::LogManager()
{
	hFile = CreateFile(_T("logFile.txt"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	offset = 0;
	dwWroteBytes = 0;
}

void LogManager::WirteLog(std::shared_ptr<ClientInfo> client, std::string Log)
{
	char clientSocket[64];
	char clientName[20];

	snprintf(clientSocket, sizeof(clientSocket), "Socket Handle: %llu", (unsigned long long)client->GetSocket());

	// 현재 날짜와 시간 가져오기
	SYSTEMTIME st;
	GetLocalTime(&st);

	// 날짜와 시간을 문자열로 포맷팅
	char dateTime[64];
	snprintf(dateTime, sizeof(dateTime), "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	memcpy(clientName, client->GetUserId(), 20);
	
	std::string logMessage = "[LOG] %s : %s";
	
	char logMessageBuffer[256];
	snprintf(logMessageBuffer, sizeof(logMessageBuffer), "\n[LOG] %s %s(%s) - %s", dateTime, clientSocket, clientName, Log.c_str());

	// 파일 끝으로 이동
	SetFilePointer(hFile, 0, NULL, FILE_END);
	DWORD dataSize = strlen(logMessageBuffer) * sizeof(char);

	BOOL blsOK = WriteFile(hFile, logMessageBuffer, dataSize, &dwWroteBytes, NULL);
	if (!blsOK || dwWroteBytes != offset * sizeof(wchar_t)) {
		std::cout << "데이터 작성 성공, 작성한 데이터 크기: " << dwWroteBytes << " bytes\n" << std::endl;
	}
	else {
		std::cout << "데이터 작성 실패, code: \n" << GetLastError() << std::endl;
		CloseHandle(hFile);
		return;
	}
}

LogManager::~LogManager()
{
}
