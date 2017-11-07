#include <ServerSocketUdp.h>
#include <iostream>
#include "exception.hpp"

using namespace std;

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		cout << "usage ./sink <inf-ip> <inf-port>";
		exit (0);
	}
	try
	{
	

	ServerSocketUdp ss = ServerSocketUdp(argv[1],atoi(argv[2]));

	string p;

	int count = 0;
	while(1)
	{
		ss >> p;
		cout << count++ << " " << p.size() << endl;
	}
	}
	catch(Exception e)
	{
		cout << e.getMessage();
		exit(0);
	}
}
