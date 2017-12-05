#include <iostream>
#include "exception.hpp"
#include "ServerSocket.h"
#include <sys/time.h>
#include <thread>
#include <mutex>
#include <stdint.h>
#include "AlgoUtils.h"
#include "log.hpp"
#include <sys/time.h>

#define BURSTY_COUNT 		1
using namespace std;

std::mutex avgLock;
uint64_t data;
uint64_t threshold;
int burstyCount;

#define TCP_RECV_BUFFER     1024

void func (int, short, void*)
{
  Logger * logger = Logger::getInstance();
//  cout << data << endl;
  if (data > threshold)
  {
    if (burstyCount > BURSTY_COUNT)
    {
      timeval temp;
      gettimeofday(&temp,0);
      logger->log("DDOS! " + std::to_string(temp.tv_sec) + " " + std::to_string(temp.tv_usec),INFO_LEVEL);
    }
    else
    {
      burstyCount ++;
    }
  }
  else
  {
  	burstyCount = 0;
  }
  //cout << burstyCount << endl;
  //logger->log("Data Recieved " + std::to_string(data) + " "  + std::to_string(burstyCount),DEBUG_LEVEL);
  data = 0;
}

void hearAllSayNothing(ServerSocket * ssa)
{
  uint8_t recvBuff[TCP_RECV_BUFFER]  = {0};
  int val;
  while(1)
	{
		val = ssa->recv(recvBuff,(int)TCP_RECV_BUFFER);
		ssa->send(recvBuff,val);
		
		if (val > 0)
		{
		  avgLock.lock();
		  data += val;
		  avgLock.unlock();
		}
	}
}

int main(int argc, char * argv[])
{
  Logger::LoggerInit("Cloud_Detection.txt");

  Timer timerBase;

  timerBase.createTimer(100,func,NULL);

	if (argc < 4)
	{
		cout << "usage ./cloud <inf-ip> <inf-port> <threshold>";
		exit (0);
	}
	threshold = atol(argv[3]);
	cout  << threshold << endl;
	try
	{
		ServerSocket ss = ServerSocket(argv[1],atoi(argv[2]));
		while (1)
		{
			ServerSocket * ssa = new ServerSocket();
			ss.accept(*ssa);
			std::thread * th = new std::thread(hearAllSayNothing,ssa);
			th->detach();
		}
	}
	catch (Exception& e) {
	  cerr << e.getMessage() << endl;
  }
}
