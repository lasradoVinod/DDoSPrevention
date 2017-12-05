#include <iostream>
#include <ClientSocket.h>
#include <time.h>

using namespace std;

int main(int argc, char *argv[])
{
	struct timespec curr_time, prev_time;
	time_t onTime, offTime;
	unsigned char flag = 0;
	if (argc < 4)
	{
		cout << "usage ./onOff <inf-ip> <inf-port> <onTime> <offTime>" << endl;
		exit (0);
	}

	ClientSocket client(string(argv[1]),atoi(argv[2]));
	uint16_t pktSize = 1000;
	onTime = atoi(argv[3]);
	offTime = atoi(argv[4]);
	
	unsigned char * data = (unsigned char*)calloc(pktSize,1);
		
	clock_gettime(CLOCK_MONOTONIC,&prev_time);
	while(1)
	{
		if (flag == 0)
			client.send(data,pktSize);
		clock_gettime(CLOCK_MONOTONIC,&curr_time);
		cout << curr_time.tv_sec << " " << curr_time.tv_nsec << endl;
		if(flag == 0 && (curr_time.tv_sec - prev_time.tv_sec) > onTime)
		{
			cout << "Stopped Sending" << endl;
			flag = 1;
			prev_time = curr_time;
		}

		if(flag == 1 && curr_time.tv_sec - prev_time.tv_sec > offTime)
		{
			flag = 0;
			prev_time = curr_time;
		}

	}
	
}