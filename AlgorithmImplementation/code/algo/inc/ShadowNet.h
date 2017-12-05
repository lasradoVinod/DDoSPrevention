/*This defines the functions for shadow net*/
#include <map>
#include <string>
#include <mutex>
#include <list>
#include <thread>
#include <memory>
#include <sys/time.h>

#include "AlgoTypes.h"
#include "MessageHelper.h"
#include "ServerSocketUdp.h"

#define NUM_BURSTY_THRESHOLD           4
/*Declare DDOS when the percentage of edge in alarm state is more than this value */
#define NUM_EDGE_THRESHOLD            0.3f
/*Time for which to receive alarm packets before deciding to turn off alarm state*/
#define TIME_VAL                      (3) //s
/*Inter arrival time threshold in us*/
#define INTER_ARRIVAL_TIME_THRESHOLD  700
#define MIN_INT_ARR_TIME              100
#define ZERO_LENGTH                    0u

typedef struct __EdgeParams
{
  /*Whether the edge is under DDoS*/
  bool underDDOS;
  /*Location of edge. Usage not implemented yet*/
  std::string loc;
  /*Determines if state is in alarm or not*/
  EdgeState eState;
  /*Average of all parameter in the current epoch*/
  SamplingParams averageState;
  /*This is the port on which edge management functions will run*/
  uint16_t managementPort;
  /*Ip address of the edge*/
  std::string ip;
  /*This is thresholding done at the edge. Some other parameter can be used*/
  uint8_t numPacketsAboveThreshold;
  /*Time since previous shadow packet*/
  timeval prevShadowPacket;
  /*bursty samples*/
  uint32_t numBurstySamples;
  /*List of IP addresses to block*/
  std::list<struct sockaddr_in> block;
  /*Time since alarm started*/
  timeval timeStart;
}EdgeParams;


class ShadowNet
{
private:
  uint32_t numEdgesAlarm;
  uint32_t numEdgesDDoS;
  uint32_t maxEdgeRegistered;
  std::map <uint32_t,EdgeParams> edgeLocMap;
  ServerSocketUdp servSock;
  std::unique_ptr<std::thread> mainLoopThread;
  bool mainStatus;
  /*Update the appropriate paramets in the EdgeParams structure*/
  void processPacket(uint8_t * data, uint16_t size , std::string ip, uint16_t port);
  EdgeConfigParams getParams(uint32_t edgeNum);
  void ShadowNetMain();
  int RelaxEdge(uint32_t edgeNum);
public:
  /*Just open Socket*/
  ShadowNet(std::string ip, uint16_t port);
  /*Create a thread and call ShadowNetMain*/
  int StartShadowNet();
  /*Send message to all edge nodes that the DDOS attack has stopped*/
  int RelaxNetwork();
};

