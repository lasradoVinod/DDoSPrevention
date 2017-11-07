#include <iostream>
#include <Edge.h>
#include "AlgoUtils.h"
#include "exception.hpp"
#include <stdint.h>
#include "AlgoTypes.h"

/*TODO: Create another socket for management functions*/

SamplingHelper::SamplingHelper()
{
	alpha = 0;
}

void SamplingHelper::CalculateAlpha(uint32_t samplingTime, uint32_t samplingDuration)
{

}

alpha_t SamplingHelper::getAlpha()
{
	return alpha;
}

Edge::Edge(uint16_t pt)
{
  eState = enStateNormal;
  datarateAverage = 0;
  port = pt;
  c_sock = NULL;
  myEdgeNum = UNDEFINED;
}

void Edge::initEdge()
{
  /*Connect with ShandowNet*/
  /*Get current params*/
  /*Start thread with timer management functions
   * */
  /*TO do this refer
   * https://stackoverflow.com/questions/17472827/create-thread-inside-class-with-function-from-same-class*/
  /*Start Server*/
  /*loop
   * {
   * listen
   * create new Process
   * Add process to list
   * Start process
   * }*/
}

/*TODO: Send port number for port number for management functions*/
int Edge::RegisterWithShadowNet(std::string ip,uint16_t port, std::string location, uint16_t managementPort)
{
  uint8_t recvbuffer [MAX_BUFFER_SIZE];
  uint8_t * message;
  uint16_t length;
  uint8_t msgType;
  msgCreator.setMessageType(REGISTER);
  msgCreator.setEdgeNumber((uint32_t)UNDEFINED);
  msgCreator.setMessage((void*)location.c_str(),(uint16_t)location.size());
  try
  {
    c_sock = new ClientSocketUdp(ip,port);
    message = msgCreator.getMessage(length);
    c_sock->send(message,length);
    length = (uint16_t)c_sock->recv(recvbuffer,MAX_BUFFER_SIZE);
  }
  catch(Exception& ex)
  {
    std::cerr << ex.getMessage() << std::endl;
    return -1;
  }
  msgParser.parseMessage(recvbuffer,length);
  msgParser.getMessageType(&msgType);
  if(msgType == REGISTER)
  {
    msgParser.getEdgeNumber(&myEdgeNum);
  }
	return 0;
}
/*Gets Edge Parameters from shadownet*/
int Edge::getEdgeConfigParams(EdgeConfigParams * params)
{
  uint8_t recvbuffer [MAX_BUFFER_SIZE];
  uint8_t * message;
  uint16_t length;
  uint8_t msgType;
  msgCreator.setMessageType(GET_PARAMS);
  msgCreator.setEdgeNumber(myEdgeNum);
  msgCreator.setMessage(NULL,(uint16_t)0);
  try
  {
    message = msgCreator.getMessage(length);
    c_sock->send(message,length);
    length = (uint16_t)c_sock->recv(recvbuffer,MAX_BUFFER_SIZE);
  }
  catch(Exception& ex)
  {
    std::cerr << ex.getMessage() << std::endl;
    return -1;
  }
  msgParser.parseMessage(recvbuffer,length);
  msgParser.getMessageType(&msgType);
  if(msgType == GET_PARAMS)
  {
    uint32_t tempVal;
    msgParser.getEdgeNumber(&tempVal);
    if (tempVal != myEdgeNum)
    {
      msgParser.copyMessageTo(&params);
    }
  }
  else
    return -1;
  return 0;
}
/*Sends Sampling parameters to the ShadowNet*/
int Edge::sendSamplingParams(SamplingParams * params)
{
  uint8_t * message;
  uint16_t length;
  msgCreator.setMessageType(SEND_SAMPLES);
  msgCreator.setEdgeNumber(myEdgeNum);
  msgCreator.setMessage(params,(uint16_t)sizeof(SamplingParams));
  try
  {
    message = msgCreator.getMessage(length);
    c_sock->send(message,length);
  }
  catch(Exception& ex)
  {
    std::cerr << ex.getMessage() << std::endl;
    return -1;
  }
  return 0;
}
/*This function handles end of sampling time*/
void Edge::getNewEdgeConfigParams()
{
  /*Call getEdgeConfigParams*/
  /*Update the params*/
}
/*This function handles sampling at each instance*/
void Edge::getSample()
{
  /*Go through the process vector and get packet count*/
  /*lock*/
  /*Get size of data received*/
  /*set to zero*/
  /*if state is alarm, for more than burstySampleTime set Edge state Alarm.*/
  /*if connected mark for deletion*/
  /*unlock*/
  /*Do average Calculations*/
  /*Remember to divide by number of connections*/
  /*Set Alarm state for all connections*/
  /*Delete*/
}
/*This function sends the sampling data*/
void Edge::sendSample()
{
  /*copy setSampling Params*/
  /*Call send sampling params*/
}


void SendAlarmPacket(ShadowPacket)
{
  /*In friend class make sure that alarm packets are sent only when edge state not process state is alarm*/
}

Process::Process(ServerSocketTcp sock,EdgeState state,
                std::string cloudip, uint16_t cloudport)
                :m_client_sock(ClientSocketTcp(cloudip,cloudport))
{
  /*When this is false, delete it*/
  connected = true;
  eState = enStateNormal;
  dataRecieved = 0;
  m_serv_sock = sock;
}
/*This is the server's main loop for one connection*/
void ProcessLoop()
{
  /*accept
   * while(connection is active)
   * get packet
   * update dataReceived
   * if state is alarm
   * call send Shadowpacket
   * */
}
Process::~Process()
{

}

