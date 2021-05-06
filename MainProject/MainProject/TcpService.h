#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)；
using namespace std;

/**
	建立长连接
*/
SOCKET buildTcpConnection(_In_z_ const char FAR * ip, int port) {
	WORD sockVerson = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVerson, &wsaData) != 0)
		return 0;

	//建立客户端socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return clientSocket;
	}

	//定义要连接的服务器地址
	sockaddr_in addrConServer;
	addrConServer.sin_family = AF_INET;
	addrConServer.sin_port = htons(port);
	addrConServer.sin_addr.S_un.S_addr = inet_addr(ip);
	if (connect(clientSocket, (SOCKADDR*)&addrConServer, sizeof(addrConServer)) == SOCKET_ERROR)
	{
		closesocket(clientSocket);
		WSACleanup();
		return clientSocket;
	}
	else {
		return clientSocket;
	}
}

/**
对关节角进行16进制浮点数编码
*/
byte* encodeAngles(float angles[6]) {
	unsigned char *angle0 = (unsigned char *)&angles[0];
	unsigned char *angle1 = (unsigned char *)&angles[1];
	unsigned char *angle2 = (unsigned char *)&angles[2];
	unsigned char *angle3 = (unsigned char *)&angles[3];
	unsigned char *angle4 = (unsigned char *)&angles[4];
	unsigned char *angle5 = (unsigned char *)&angles[5];
	byte encodeAngle0[4] = { angle0[0], angle0[1], angle0[2], angle0[3] };
	byte encodeAngle1[4] = { angle1[0], angle1[1], angle1[2], angle1[3] };
	byte encodeAngle2[4] = { angle2[0], angle2[1], angle2[2], angle2[3] };
	byte encodeAngle3[4] = { angle3[0], angle3[1], angle3[2], angle3[3] };
	byte encodeAngle4[4] = { angle4[0], angle4[1], angle4[2], angle4[3] };
	byte encodeAngle5[4] = { angle5[0], angle5[1], angle5[2], angle5[3] };
	byte encodeAngles[30] = { 143, 17, 25,
		encodeAngle0[0], encodeAngle0[1], encodeAngle0[2], encodeAngle0[3],
		encodeAngle1[0], encodeAngle1[1], encodeAngle1[2], encodeAngle1[3],
		encodeAngle2[0], encodeAngle2[1], encodeAngle2[2], encodeAngle2[3],
		encodeAngle3[0], encodeAngle3[1], encodeAngle3[2], encodeAngle3[3],
		encodeAngle4[0], encodeAngle4[1], encodeAngle4[2], encodeAngle4[3],
		encodeAngle5[0], encodeAngle5[1], encodeAngle5[2], encodeAngle5[3],
		48, 0, 255 };
	return encodeAngles;
}

/**
   向Codesys发送多关节运动编码指令
*/
int sendJointAngle(SOCKET clientSocket, float angles[6]) {
	byte *orderCode = encodeAngles(angles);
	byte code[30];
	for (int i = 0; i < 30; i++)
	{
		code[i] = *(orderCode + i);
	}
	int sendRes = send(clientSocket, (char*)&code, sizeof(code), 0);
	if (sendRes == SOCKET_ERROR)
	{
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	else {
		return 1;
	}
}

/**
	读机械臂关节角状态；
*/
float* readJointAngle(SOCKET clientSocket) {
	byte jointAngleCode[16];

	//动态分配内存，否则在函数执行结束后内存会被自动释放, 使用完成后必须手动释放内存
	float *jointAngle = new float[6];
	for (int i = 0; i < 6; i++) {
		jointAngle[i] = 0;
	}
	//发送读指令
	byte code[30] = { 1, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0};
	int sendRes = send(clientSocket, (char*)&code, sizeof(code), 0);
	int recRes = recv(clientSocket, (char*)&jointAngleCode, sizeof(jointAngleCode), 0);
	
	//校验数据帧头尾，解码
	if (jointAngleCode[0] == 187 && jointAngleCode[1] == 187 && jointAngleCode[2] == 12 && jointAngleCode[15] == 13) {
		int index = 0;
		for (int i = 0; i < 6; i++) {
			int sign = jointAngleCode[4 + index] & (1 << 7);
			if (sign == 0) {
				sign = 1;
			}
			else {
				sign = -1;
			}
			int num = jointAngleCode[4 + index] & 127;
			jointAngle[i] = sign * (float)(num << 8 | jointAngleCode[3 + index]) / (float)100;
			index = index + 2;
		}
	}

	return jointAngle;
}
