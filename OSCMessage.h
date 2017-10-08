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

//#include "SimpleOSCClient.h"

typedef std::string OSCAddress;
typedef int OSCInt;
typedef float OSCFloat;
typedef std::string OSCString;

//class SimpleOSCClient;

class OSCMessage
{
private:
	
	union fourcharfloat
	{
		OSCFloat flo;
		char chr[4];
	};
	union fourcharint
	{
		OSCInt integ;
		char chr[4];
	};
	struct oscarg
	{
		char typechar;
		OSCString data_string = "";
		OSCFloat data_float;
		OSCInt data_int;
	};
	std::vector<char> cache;
	bool rendered;
	OSCAddress oscaddress;
	std::string host;
	unsigned short port;
	std::vector<oscarg> oscargs;
	//SimpleOSCClient * attached_client = nullptr;
public:
	OSCMessage() {}
	/*
	OSCMessage(SimpleOSCClient & cli) { attachClient(cli); }
	inline void attachClient(SimpleOSCClient & cli) {
		attached_client = &cli;
	}
	inline void send() {
		if (attached_client != nullptr) {
			(*attached_client).send(*this);
			attached_client = nullptr; //one-time
		}
	}*/
	std::string getHost();
	void setHost(std::string h);
	OSCAddress prependAddress(OSCAddress pre);
	OSCAddress appendAddress(OSCAddress post);
	OSCAddress setAddress(OSCAddress osc_address);
	OSCAddress getAddress();
	void addArgument(OSCFloat f);
	void addArgument(OSCInt i);
	void addArgument(OSCString s);
	void addf(OSCFloat f);
	void addi(OSCInt i);
	void adds(OSCString s);
	OSCFloat getf(unsigned int index);
	OSCInt geti(unsigned int index);
	OSCString gets(unsigned int index);
	void clearargs();
	unsigned int argsize();
	char argtype(unsigned int index);
	std::vector<char> formattedByteArray();
	void render();

	OSCMessage & parseOSCMessage(char incoming[], unsigned int length);
};