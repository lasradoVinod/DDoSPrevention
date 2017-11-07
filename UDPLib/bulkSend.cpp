#include <ClientSocketUdp.h>
#include <iostream>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 4)
	{
		cout << "usage ./bulk <inf-ip> <inf-port> <packet-size>" << endl;
		exit (0);
	}

	ClientSocketUdp client(string(argv[1]),atoi(argv[2]));
	uint16_t pktSize = (uint16_t)atoi(argv[3]);
	unsigned char * data = (unsigned char*)calloc(pktSize,1);
	memset(data,1,pktSize);
	while(1)
	{
		client.send(data,pktSize);
	}

}
