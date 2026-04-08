#include "Winsocket.h"

const char* IP = "192.168.0.20";
const uint16 PORT = 8080;

DWORD revMsg(LPVOID lpParameter);

WindowsSocket::WindowsSocket() 
{
	WORD wSockVersion = MAKEWORD(2, 2);
	WSADATA wsadata;
	int ret = WSAStartup(wSockVersion,&wsadata);
	if (ret != 0) {
		return;
	}

	if (wsadata.wVersion != wSockVersion) {
		cout << "version error!" << endl;
		return;
	}
}

bool WindowsSocket::enableSocket()
{
	int ret;
	sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (sock == INVALID_SOCKET) {
		cout << "create sock error!" << endl;
		return false;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);
	ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		cout << "bind sock error!" << endl;
		return false;
	}
	HANDLE revThread = CreateThread(NULL,0,revMsg,&sock,0,NULL);
	//此处调用CloseHandle没有立即关闭线程，是因为CreateThread之后引用计数为2
	//而CloseHandle只是引用计数-1，并不是释放线程本身，只有当引用计数为0时，线程会自动销毁
	CloseHandle(revThread);
	return true;
}

void WindowsSocket::disableSocket()
{
	closesocket(sock);
}

void WindowsSocket::sendMsg()
{
	sockaddr_in desAddr;
	int len = 666;
	desAddr.sin_addr.S_un.S_addr = inet_addr("192.168.0.3");
	desAddr.sin_family = AF_INET;
	desAddr.sin_port = htons(8080);
	const char sendBuf[100] = "hello world!\r\n";
	int ret = sendto(sock, sendBuf, sizeof(sendBuf), 0, (struct sockaddr*)&desAddr,len);
	cout << "sendlength:" << ret << endl;
}

DWORD revMsg(LPVOID lpParameter)
{
	sockaddr_in clientAddr;
	int len = 666;
	char* buf;
	buf = (char*)malloc(60000);
	while (1) {
		int ret = recvfrom(*(SOCKET*)lpParameter, buf, 60000, 0, (sockaddr*)&clientAddr, &len);
		if (ret) {
			buf[ret] = '\0';
			cout << "addr:" << inet_ntoa(clientAddr.sin_addr) << "  "
				<< "port:" << ntohs(clientAddr.sin_port) << "  "
				<< "length:" << ret << "  "
				<< "data:" << buf << endl;
			//break;
		}
	}
	free(buf);
	return 1;
}



