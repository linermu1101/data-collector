#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifndef WINSOCKET_H_
#define WINSOCKET_H_

typedef  char		int8;
typedef  short		int16;
typedef  int		int32;
typedef  long long	int64;
typedef  unsigned char		uint8;
typedef  unsigned short		uint16;
typedef  unsigned int		uint32;
typedef  unsigned long long	uint64;

#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")
using  namespace std;

class WindowsSocket {
public:
	WindowsSocket();
	bool enableSocket();
	void disableSocket();
	void sendMsg();
private:

	SOCKET sock;
};

#endif