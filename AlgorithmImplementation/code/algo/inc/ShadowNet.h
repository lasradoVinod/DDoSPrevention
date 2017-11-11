/*This defines the functions for shadow net*/
#include <map>
#include <string>
#include <mutex>
#include <list>
#include <thread>
#include <memory>

#include "AlgoTypes.h"
#include "MessageHelper.h"
#include "ServerSocketUdp.h"

/*Declare DDOS when the percentage of edge in alarm state is more than this value */
#define NUM_EDGE_THRESHOLD    0.3f
/*Time for which to recieve alarm packets before deciding to turn off alarm state*/
#define TIME_VAL              (120*1000) //ms

typedef struct __AverageState
{
  average_t averageDateRate;
}AverageState;

typedef struct __EdgeParams
{
  uint8_t underDDOS;
  /*Location of edge. Usage not implemented yet*/
  std::string loc;
  /*Determines if state is in alarm or not*/
  EdgeState eState;
  /*Average of all parameter in the currect epoch*/
  AverageState averageState;
  /*This is the port on which edge management functions will run*/
  uint16_t managementPort;
  /*Ip address of the edge*/
  std::string ip;
  /*This is thresholding done at the edge. Some other parameter can be used*/
  uint8_t numPacketsAboveThreshold;
  /*Time since previous shadow packet*/
  timeval prevShadowPacket;
  /*List of IP addresses to block*/
  std::list<struct sockaddr_in> block;
}EdgeParams;


class ShadowNet
{
private:
  uint32_t numEdgesAlarm;
  uint32_t maxEdgeRegistered;
  std::map <uint32_t,EdgeParams> edgeLocMap;
  /*To avoid race condition*/
  std::map <uint32_t,std::mutex> edgeLocMapMutex;
  ServerSocketUdp servSock;
  std::mutex mapLock; 
  std::unique_ptr<std::thread> mainLoopThread;
  bool mainStatus;
  /*Update the appropriate paramets in the EdgeParams structure*/
  void processPacket(uint8_t * data, uint16_t size , std::string ip, uint16_t port);
  EdgeConfigParams getParams(uint32_t edgeNum);
  void ShadowNetMain();
public:
  /*Just open Socket*/
  ShadowNet(std::string ip, uint16_t port);
  /*
  Create a thread for this
  The main loop.
  Start a new thread for processing the packet*/
  int StartShadowNet();
  /*Send message to all edge nodes that the DDOS attack has stopped*/
  int RelaxNetwork();
};

