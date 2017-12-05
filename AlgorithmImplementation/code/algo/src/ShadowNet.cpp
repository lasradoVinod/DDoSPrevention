#include <iostream>
#include <unistd.h>
#include <memory>
#include <thread>
#include <sys/time.h>

#include "ShadowNet.h"
#include "exception.hpp"
#include "ClientSocketUdp.h"
#include "log.hpp"

/*This code might just survive in a scenario where each user it communicates with is trusted*/

/*Initialize this*/
EdgeConfigParams eConf = {
		  .burstySampleTime = 4,
		  .threasholdInput =100,
		  .initialAvergeInput = 4000,//250000,
		  .samplingTime = 100,
		  .samplingDuration = 600000,
		  .sampleSendTime = 30000
};


ShadowNet::ShadowNet(std::string ip, uint16_t port) :
    servSock (ServerSocketUdp(ip.c_str(),port))
{
  numEdgesDDoS = 0;
  mainStatus = false;
  numEdgesAlarm =0;
  maxEdgeRegistered = 1;
}

int ShadowNet::RelaxNetwork()
{
  return 0;
}

void ShadowNet::ShadowNetMain()
{
  std::string localIp;
  uint16_t localPort;
  uint8_t * buffer;
  uint16_t recvLength;
  Logger * logger = Logger::getInstance();
  logger->log("In Main",DEBUG_LEVEL);
  try
  {
    while(1)
    {
      buffer = (uint8_t*)malloc(MAXRECV_UDP);
      recvLength = (uint16_t)servSock.recv(buffer,MAXRECV_UDP,localIp,localPort);
      /*An optimisation would be the use of threadpool*/
      processPacket(buffer,recvLength,localIp,localPort);
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

  MessageCreator msgCreator;
  timeval temp;
  gettimeofday(&temp,0);
  logger->log("Parsing message" + std::to_string(temp.tv_sec) + " " + std::to_string(temp.tv_usec) ,DEBUG_LEVEL);
  MessageParser msgParser;
  msgParser.parseMessage(buffer,recvLength);

  uint32_t edgeNumber;
  uint8_t messageType;
  uint16_t lengthMsg;
  msgParser.getEdgeNumber(&edgeNumber);
  msgParser.getMessageType(&messageType);
  msgParser.getLength(&lengthMsg);
  uint8_t * message;
  EdgeConfigParams tempEdge;
  EdgeParams initConfig;
  SamplingParams params;
  ShadowPacket sp;
  /*Should check if the edge exists but I'm living in an Ideal bubble.
   * Please don't burst the bubble*/
  switch (messageType)
  {
    case REGISTER:
        /*This could be a possible attack in which an attacker keeps calling register
         *and in no time I'm out of memory. To solve this I will have to look if the
         *ip is registered if yes then reject the request. This in not currently
         *implemented*/
        uint32_t edgeNumRet;
        message = (uint8_t*)calloc(lengthMsg + 1,1);
        msgParser.copyMessageTo(message);
        initConfig.loc = (char*)message;
        free(message);
        initConfig.ip= clientIP;
        initConfig.eState = enStateNormal;
        initConfig.underDDOS = false;
        initConfig.numBurstySamples = 0;
        gettimeofday(&initConfig.prevShadowPacket,0);
        edgeLocMap[maxEdgeRegistered] = initConfig;
        edgeNumRet = maxEdgeRegistered ++;
        /*Did not create mutex for it because it is created using the default ctor
         * when I call it. This is the behaviour of map*/
        msgCreator.setMessageType(REGISTER);
        msgCreator.setEdgeNumber(edgeNumRet);
        msgCreator.setMessage(NULL,ZERO_LENGTH);
        message = msgCreator.getMessage(lengthMsg);
        myClientSock.send(message,lengthMsg);
      break;

    case SEND_SAMPLES:
      msgParser.copyMessageTo((void*)&(edgeLocMap[edgeNumber].averageState));
      logger->log(std::string("Send Samples") + std::to_string(edgeLocMap[edgeNumber].averageState.inputPackets),DEBUG_LEVEL);
      break;

    case GET_PARAMS:
      msgCreator.setMessageType(GET_PARAMS);
      msgCreator.setEdgeNumber(edgeNumber);
      tempEdge = getParams(edgeNumber);
      msgCreator.setMessage((void *)&tempEdge,sizeof (EdgeConfigParams));
      message = msgCreator.getMessage(lengthMsg);
      myClientSock.send(message,lengthMsg);
      break;

    case ALARM_PACKETS:
      msgParser.copyMessageTo((void*)&sp);
      timeval res;
      if (edgeLocMap[edgeNumber].eState == enStateNormal)
      {
        logger->log("Edge " + std::to_string(edgeNumber) + " in Alarm state" ,INFO_LEVEL);
        numEdgesAlarm ++;
        edgeLocMap[edgeNumber].eState = enStateAlarm;
        edgeLocMap[edgeNumber].prevShadowPacket = temp;
        edgeLocMap[edgeNumber].timeStart = temp;
      }
      else
      {
        timersub(&temp,&edgeLocMap[edgeNumber].prevShadowPacket,&res);
        edgeLocMap[edgeNumber].prevShadowPacket = temp;
        logger->log("Time diff" + std::to_string(res.tv_usec),DEBUG_LEVEL);
        if (res.tv_usec < INTER_ARRIVAL_TIME_THRESHOLD && res.tv_usec > MIN_INT_ARR_TIME)
        {
          if (edgeLocMap[edgeNumber].numBurstySamples < NUM_BURSTY_THRESHOLD)
          {
            edgeLocMap[edgeNumber].numBurstySamples ++;
          }
          else
          {
            edgeLocMap[edgeNumber].underDDOS = true;
            numEdgesDDoS ++;
            float tempCmp = (float)numEdgesDDoS / (float) maxEdgeRegistered;
            if (tempCmp > NUM_EDGE_THRESHOLD)
            {
              logger->log("DDOS! " + std::to_string(temp.tv_sec) + " " + std::to_string(temp.tv_usec),INFO_LEVEL);
            }
          }
        }
        else
        {
          if (edgeLocMap[edgeNumber].underDDOS == false)
          {
            edgeLocMap[edgeNumber].numBurstySamples = 0;
          }
        }
      }

      if (edgeLocMap[edgeNumber].eState == enStateAlarm && edgeLocMap[edgeNumber].underDDOS == false)
      {

        timersub(&temp,&edgeLocMap[edgeNumber].timeStart,&res);
        if (res.tv_sec > TIME_VAL)
        {
          edgeLocMap[edgeNumber].eState = enStateNormal;
          RelaxEdge(edgeNumber);
        }
      }
      break;

    case REGISTER_MANAGEMENT_PORT:
      if (lengthMsg == sizeof(uint16_t) && edgeNumber != UNDEFINED)
      {
        msgParser.copyMessageTo((void*)&(edgeLocMap[edgeNumber].managementPort));
      }
      msgCreator.setMessageType(REGISTER_MANAGEMENT_PORT);
      msgCreator.setEdgeNumber(edgeNumber);
      msgCreator.setMessage(NULL,ZERO_LENGTH);
      message = msgCreator.getMessage(lengthMsg);
      myClientSock.send(message,lengthMsg);
      break;

    default:
      /*reply with error message*/
      msgCreator.setMessageType(ERROR_MESSAGE);
      msgCreator.setEdgeNumber(UNDEFINED);
      msgCreator.setMessage(NULL,ZERO_LENGTH);
      message = msgCreator.getMessage(lengthMsg);
      myClientSock.send(message,lengthMsg);
      break;
  }
  free(buffer);
}

int ShadowNet::RelaxEdge(uint32_t edgeNum)
{
  Logger * logger = Logger::getInstance();

  logger->log("Sending relax message",INFO_LEVEL);

  uint8_t * msg;
  uint16_t length;
  MessageCreator msgCreator;
  msgCreator.setMessageType(CHANGE_TO_NORMAL);
  msgCreator.setEdgeNumber(edgeNum);
  ClientSocketUdp mEdgeSock(edgeLocMap[edgeNum].ip,edgeLocMap[edgeNum].managementPort);
  msg = msgCreator.getMessage(length);
  mEdgeSock.send(msg,length);
  return 0;
}
