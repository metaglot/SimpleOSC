#include "stdafx.h"
#include "SimpleOSCClient.h"


void SimpleOSCClient::closesock() {
		closesocket(udpsock);
		WSACleanup();
}

void SimpleOSCClient::opensock(std::string chost, unsigned short cport) {
		closesock();

		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
			//failed
			return;
		}

		if ((udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
			return;
		}

		memset((char*)&si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(cport);
		//si_other.sin_addr.S_un.S_addr = inet_addr(chost.c_str());
		inet_pton(AF_INET,  chost.c_str(), &si_other.sin_addr.s_addr);
		//InetPton()
		
}

void SimpleOSCClient::sendbuffer(char buf[], unsigned int len) {
		if (sendto(udpsock, buf, len, 0, (struct sockaddr *) &si_other, sizeof(si_other)) == SOCKET_ERROR)
			return;
}

void SimpleOSCClient::send(OSCMessage message) {
		auto m = message.formattedByteArray();
		sendbuffer(&m[0], m.size());
}


void SimpleOSCClient::send(OSCAddress address, OSCFloat f) {
		OSCMessage msg;
		msg.setAddress(address);
		msg.addf(f);
		send(msg);
}

void SimpleOSCClient::send(OSCAddress address, OSCInt i) {
		OSCMessage msg;
		msg.setAddress(address);
		msg.addi(i);
		send(msg);
}