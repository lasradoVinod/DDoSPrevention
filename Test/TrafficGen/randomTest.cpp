#include <iostream>
#include <ClientSocket.h>
#include <string.h>
#include <time.h>
#include "exception.hpp"
using namespace std;

int main(int argc, char *argv[])
{
	struct timespec curr_time, prev_time,ph1;
	time_t offTime;
	if (argc < 6)
	{
		cout << "usage ./onOff <inf-ip> <inf-port> <avgPktSize> <deltaP> <avgDelay(us)> <deltaT(us)>" << endl;
		exit (0);
	}
	try{
	ClientSocket client(string(argv[1]),atoi(argv[2]));
	uint32_t pktSize = atoi(argv[3]);
	uint32_t deltaPkt = atoi(argv[4]);
	uint32_t maxDelay = atoi(argv[5]);
	uint32_t deltaTime = atoi(argv[6]);

	maxDelay -= deltaTime;
	deltaTime *= 2;
	pktSize -= deltaPkt;
	deltaPkt *= 2;

	unsigned char * data = (unsigned char*)calloc(500,1);
	memset(data,1,500);

	clock_gettime(CLOCK_MONOTONIC,&ph1);
	clock_gettime(CLOCK_MONOTONIC,&curr_time);
	while(curr_time.tv_sec < ph1.tv_sec + 30 )
	{	
		uint32_t temp = pktSize + (rand()%deltaPkt);
		cout << temp << endl;
		client.send(data,temp);
		clock_gettime(CLOCK_MONOTONIC,&prev_time);
		clock_gettime(CLOCK_MONOTONIC,&curr_time);
		offTime = rand() % deltaTime;
		offTime += maxDelay;  	
		while(curr_time.tv_sec - prev_time.tv_sec < 1 
			&& (curr_time.tv_nsec - prev_time.tv_nsec)/(long)1e3 < offTime)
		{
			clock_gettime(CLOCK_MONOTONIC,&curr_time);
		}
	}
	while(1)
	{
    uint32_t temp = pktSize + (rand()%deltaPkt);
    cout << temp << endl;
    client.send(data,temp);
    clock_gettime(CLOCK_MONOTONIC,&prev_time);
    clock_gettime(CLOCK_MONOTONIC,&curr_time);
    offTime = rand() % deltaTime;
    offTime += maxDelay;
    while(curr_time.tv_sec - prev_time.tv_sec < 1
      && (curr_time.tv_nsec - prev_time.tv_nsec)/(long)1e3 < (offTime /2 ))
    {
      clock_gettime(CLOCK_MONOTONIC,&curr_time);
    }

	}
}
catch(Exception &e)
{
	cout << e.getMessage() << endl;
}	
}
