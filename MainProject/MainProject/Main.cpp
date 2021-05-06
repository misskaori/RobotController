//  (C) 2001-2019 Force Dimension
//  All Rights Reserved.
//
//  Version 3.8.0


#include "TcpService.h"
using namespace std;

int main(int  argc, char **argv)
{
	SOCKET clientSocket = buildTcpConnection("127.0.0.1", 4711);

	float angles[6] = {-30, -60, 60, -90, -30, 20.9};
	//sendJointAngle(clientSocket, angles);
	float *a = readJointAngle(clientSocket);
	for (int i = 0; i < 6; i++) {
		cout << a[i] << endl;
	}
	delete[] a;

	closesocket(clientSocket);
	system("pause");
	return 0;
}