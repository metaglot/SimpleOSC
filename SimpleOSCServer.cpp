#include "stdafx.h"
#include "SimpleOSCServer.h"


void SimpleOSCServer::close() {
	closesocket(sock);
	WSACleanup();
}

void SimpleOSCServer::udp_server() {

	struct sockaddr_in server, si_other;
	int slen, recv_len;
	const unsigned int BUFLEN = 65536;
	char buf[BUFLEN];
	WSADATA wsa;

	slen = sizeof(si_other);


	//initializing winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		//failed: WSAGetLastError();
		return;
	}

	//initialized

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
		//coult not create socket: WSAGetLastError();
		return;
	}

	//socket created


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
		//bind failed: WSAGetLastError();
		return;
	}

	//socket bound

	memset(buf, '\0', BUFLEN);

	while (true) {
		//fflush
		if ((recv_len = recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR) {
			//failed: WSAGetLastError();
			break;
		}

		//data from:
		if (recv_len > 0) {
			//std::string adr( inet_ntoa( si_other.sin_addr ) );
			//unsigned short prt( ntohs( si_other.sin_port ) );

			OSCMessage m;
			m.setHost("...");
			//m.set
			m.parseOSCMessage(buf, recv_len);
			callback(m);
		}


	}

	close();
}

void* SimpleOSCServer::thread_launch(void* thisPtr) {
		((SimpleOSCServer*)thisPtr)->udp_server();
		return NULL;
}

