#pragma once
#pragma comment(lib,"ws2_32.lib")

#include <thread>
#include <string>
#include <string.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <vector>
#include <iostream>

#include "OSCMessage.h"

class SimpleOSCClient
{
	int udpsock;
	int sock;
	sockaddr_in si_other;
	int slen = sizeof(si_other);
	WSADATA wsa;
	void closesock();
	void opensock(std::string chost, unsigned short cport);
	void sendbuffer(char buf[], unsigned int len);
public:
	void send(OSCMessage message);
	void send(OSCAddress address, OSCFloat f);
	void send(OSCAddress address, OSCInt i);

	SimpleOSCClient(std::string serverhostname, unsigned short serverport){
		opensock(serverhostname, serverport);
	}
	~SimpleOSCClient(){
		closesock();
	}
};