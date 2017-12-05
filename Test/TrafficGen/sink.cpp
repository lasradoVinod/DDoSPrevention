#include <iostream>
#include "exception.hpp"
#include "ServerSocket.h"
#include <sys/time.h>
using namespace std;

int main(int argc, char * argv[])
{
	int count = 0;
	timeval curr, prev,res;
	if (argc < 3)
	{
		cout << "usage ./sink <inf-ip> <inf-port>";
		exit (0);
	}
	try
		{
	
	ServerSocket ssa;

	ServerSocket ss = ServerSocket(argv[1],atoi(argv[2]));

	ss.accept(ssa);

	string p;



		while(1)
		{
			gettimeofday(&prev,0);
			ssa >> p;
			gettimeofday(&curr,0);
			timersub(&curr,&prev,&res);
			cout << count++ << " " << p.size() << " "<< res.tv_usec << endl;
		}
	}
		catch(Exception e)
		{
			cout << e.getMessage();
			//exit(0);
		}

	cout << "Total Data Recv" << count * 250 << endl;
	cout << "Average Data Rate" << (count * 500)/10 << endl;
}