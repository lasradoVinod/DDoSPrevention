/*This defines the functions for shadow net*/
#include <map>
#include <string>
#include <mutex>

#include "AlgoTypes.h"
#include "MessageHelper.h"
#include "ServerSocketUdp.h"

typedef struct __AverageState
{
  average_t averageDateRate;
}AverageState;

typedef struct __EdgeParams
{
  std::string loc;
  EdgeState eState;
  AverageState averageState;
  uint16_t managementPort;
  std::string ip;
}EdgeParams;


class ShadowNet
{
private:
  uint32_t numEdgesAlarm;
  uint32_t maxEdgeRegistered;
  std::map <uint32_t,EdgeParams> edgeLocMap;
  ServerSocketUdp servSock;
public:
  /*Just open Socket*/
  ShadowNet(std::string ip, uint16_t port);
  /*The main loop.
   */
  int startShadowNet();
  void processPacket(uint8_t * data, uint32_t size);
  int registerEdge(std::string loc);
  EdgeConfigParams getParams(uint32_t edgeNum);

};

