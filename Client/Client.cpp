#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

#define DEFAULT_PORT "5000"

int main(int argc, char *argv[]) {

	int iResult;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!
	
									  // Resolve the server address and port
	iResult = getaddrinfo("172.16.0.1", DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	Mat decodedMat;
	int byteReceived = 0;
	vector<uchar> buffer;
	for (;;)
	{

		int numOfByteToReceive;
		byteReceived = recv(ConnectSocket, (char*)&numOfByteToReceive, 4, 0);

		if (numOfByteToReceive == -1)
			break;

		buffer = vector<uchar>(numOfByteToReceive);

		byteReceived = 0;
		while (byteReceived < numOfByteToReceive)
		{
			byteReceived += recv(ConnectSocket, (char*)&buffer[0 + byteReceived], numOfByteToReceive - byteReceived, 0);
		}
		//printf("CLIENT RECEIVED: %d total bytes\n\n", buffer.size());

		decodedMat = imdecode(Mat(buffer), 1);
		imshow("Client", decodedMat);
		if (waitKey(50) == 27) break;

	}

	
	return 0;
}
