#include <iostream>
#include "exception.hpp"
#include "ServerSocket.h"

using namespace std;

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		cout << "usage ./sink <inf-ip> <inf-port>";
		exit (0);
	}
	ServerSocket ssa;

	ServerSocket ss = ServerSocket(argv[1],atoi(argv[2]));

	ss.accept(ssa);

	string p;

	int count = 0;
	while(1)
	{
		try
		{
		ssa >> p;
		}
		catch(Exception e)
		{
			cout << e.getMessage();
			exit(0);
		}
		cout << count++ << p.size() << endl;
	}
}