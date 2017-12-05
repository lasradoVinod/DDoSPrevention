#include <iostream>
#include <ClientSocket.h>
#include <sys/time.h>
#include "exception.hpp"
#include "string.h"
#include "AlgoUtils.h"

using namespace std;

ClientSocket * client;

unsigned char * data;
uint16_t pktSize;

uint64_t countPkt;
void myFunc(int,short,void*)
{
	try
	{
		client->send(data,pktSize);
	}
	catch(Exception &e)
	{
		cerr << e.getMessage() << endl;
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	Timer timBase;
	if (argc < 4)
	{
		cout << "usage ./bulk <inf-ip> <inf-port> <packet-size>" << endl;
		exit (0);
	}
	try
	{
		client = new ClientSocket(string(argv[1]),atoi(argv[2]));
		pktSize = (uint16_t)atoi(argv[3]);
		data = (unsigned char*)calloc(pktSize,1);
		if(data == NULL)
			throw Exception("data not allocated",0);
		memset(data,1,pktSize);
		struct event * timeEvent = timBase.createTimer(1,myFunc,NULL);
		while(1);
	}
	catch(Exception &e)
	{
		cout << e.getMessage();
	}	
}
