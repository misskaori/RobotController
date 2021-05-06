#include <stdio.h>
#include <math.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)��
using namespace std;

SOCKET buildTcpConnection(_In_z_ const char FAR * ip, int port) {
	WORD sockVerson = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVerson, &wsaData) != 0)
		return 0;

	//�����ͻ���socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("�׽��ִ���ʧ��");
		WSACleanup();
		return clientSocket;
	}

	//����Ҫ���ӵķ�������ַ
	sockaddr_in addrConServer;
	addrConServer.sin_family = AF_INET;
	addrConServer.sin_port = htons(port);
	addrConServer.sin_addr.S_un.S_addr = inet_addr(ip);
	if (connect(clientSocket, (SOCKADDR*)&addrConServer, sizeof(addrConServer)) == SOCKET_ERROR)
	{
		printf("�ͻ��˽�������ʧ�ܣ�\n");
		closesocket(clientSocket);
		WSACleanup();
		return clientSocket;
	}
	else {
		printf("�ͻ��˽������ӳɹ���׼���������ݣ�\n");
		return clientSocket;
	}
}