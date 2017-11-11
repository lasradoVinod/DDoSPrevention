#include <iostream>
#include <unistd.h>
#include <memory>
#include <thread>

#include "ShadowNet.h"
#include "exception.hpp"
#include "ClientSocketUdp.h"
#include "log.hpp"

/*Initialize this*/
EdgeConfigParams eConf;

ShadowNet::ShadowNet(std::string ip, uint16_t port) :
    servSock (ServerSocketUdp(ip.c_str(),port))
{
  mainStatus = false;
  numEdgesAlarm =0;
  maxEdgeRegistered = 1;
}


void ShadowNet::ShadowNetMain()
{
  std::string localIp;
  uint16_t localPort;
  uint8_t * buffer;
  uint16_t recvLength;
  try
  {
    while(1)
    {
      buffer = (uint8_t*)malloc(MAXRECV_UDP);
      recvLength = (uint16_t)servSock.recv(buffer,MAXRECV_UDP,localIp,localPort);
      std::thread * th = new std::thread (&ShadowNet::processPacket,this,buffer,recvLength,localIp,localPort);
    }
  }
  catch (Exception &e)
  {
    mainLoopThread = false;
    std::cerr << e.getMessage() << std::endl;
  }
}

int ShadowNet::StartShadowNet()
{
  mainStatus = true;
  std::unique_ptr<std::thread> th (new std::thread(&ShadowNet::ShadowNetMain,this));
  mainLoopThread = std::move(th);
  return 0;
}

EdgeConfigParams ShadowNet::getParams(uint32_t edgeNum)
{
  return eConf;
}

void ShadowNet::processPacket(uint8_t * buffer, uint16_t recvLength, std::string clientIP,uint16_t clientPort )
{
  /*Create a new socket to reply*/
  Logger *logger = Logger::getInstance();
  std::string logString;
  logString = std::string("Creating client socket ") + clientIP + std::to_string(clientPort);
  logger->log(logString.c_str(),DEBUG_LEVEL);
  ClientSocketUdp myClientSock(clientIP,clientPort);

  logger->log("Parsing message",DEBUG_LEVEL);
  MessageParser msgParser;
  msgParser.parseMessage(buffer,recvLength);

  uint32_t edgeNumber;
  uint8_t messageType;
  msgParser.getEdgeNumber(&edgeNumber);
  msgParser.getMessageType(&messageType);

  switch (messageType)
  {
    case REGISTER:
      if (edgeNumber == UNDEFINED)
      {
        /*This could be a possible attack in which an attacker keeps calling register
         *and in no time I'm out of memory. To solve this I will have to look if the
         *ip is registered if yes then reject the request. This in not currently
         *implemented*/
        EdgeParams initConfig;
        mapLock.lock();
        edgeLocMap[maxEdgeRegistered];
        maxEdgeRegistered ++;
        mapLock.unlock();
      }
      break;
    case SEND_SAMPLES:
      break;

    case GET_PARAMS:
      break;

    case ALARM_PACKETS:
      break;

    default:
      /*reply with error message*/
      break;
  }
  free(buffer);
}
