#include "ShadowNet.h"

/*Initialize this*/
EdgeConfigParams eConf;

ShadowNet::ShadowNet(std::string ip, uint16_t port) :
    servSock (ServerSocketUdp(ip.c_str(),port))
{
  numEdgesAlarm =0;
  maxEdgeRegistered = 0;
}

int ShadowNet::startShadowNet()
{
  /* On receipt of packet give it to processPacket on a new thread*/
  return 0;
}

int ShadowNet::registerEdge(std::string loc)
{
  maxEdgeRegistered ++;

  return 0;
}
EdgeConfigParams ShadowNet::getParams(uint32_t edgeNum)
{
  return eConf;
}

void processPacket()
{
  /*Create a new socket to reply*/
}
