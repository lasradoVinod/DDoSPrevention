#include "ShadowNet.h"

/*Initialize this*/
EdgeConfigParams eConf;

ShadowNet::ShadowNet(std::string ip, uint16_t port) :
    servSock (ServerSocketUdp(ip.c_str(),port))
{
  numEdgesAlarm =0;
  maxEdgeRegistered = 0;
}


void ShadowNet::ShadowNetMain()
{

}

int ShadowNet::StartShadowNet()
{
 	/*Spawn a thread to ShadowNetMain*/
  
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
