#include <ClientSocketUdp.h>
#include <iostream>
#include <time.h>

using namespace std;

int main(int argc, char *argv[])
{
	struct timespec curr_time, prev_time;
	time_t offTime;
	if (argc < 4)
	{
		cout << "usage ./onOff <inf-ip> <inf-port> <maxPktSize> <maxDelayPak(ms)>" << endl;
		exit (0);
	}

	ClientSocketUdp client(string(argv[1]),atoi(argv[2]));
	uint32_t pktSize = atoi(argv[3]);
	uint32_t maxDelay = atoi(argv[4]);
	
	unsigned char * data = (unsigned char*)calloc(pktSize,1);
		
	while(1)
	{
		client.send(data,rand()%pktSize);
		clock_gettime(CLOCK_MONOTONIC,&prev_time);
		clock_gettime(CLOCK_MONOTONIC,&curr_time);
		offTime = rand() % maxDelay; 	
		while(curr_time.tv_sec - prev_time.tv_sec < 1 
			&& (curr_time.tv_nsec - prev_time.tv_nsec)/(long)1e6 < offTime)
		{
			clock_gettime(CLOCK_MONOTONIC,&curr_time);
		}
	}	
}