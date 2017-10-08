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

class SimpleOSCServer
{
	void(&callback)(OSCMessage);
	unsigned short port;
	std::thread * t;
	SOCKET sock;
	void close();
	void udp_server();
public:
	static void* thread_launch(void* thisPtr);
	SimpleOSCServer(void(&messagecallback)(OSCMessage message), unsigned short portnumber)
		: callback(messagecallback)
		, port(portnumber) {
		t = new std::thread(&SimpleOSCServer::thread_launch, (void*)this);
	}
	~SimpleOSCServer() { close(); }
};