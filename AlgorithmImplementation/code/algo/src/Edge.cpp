#include <iostream>
#include <Edge.h>
#include <stdint.h>
#include <thread>
#include <math.h>

#include "AlgoUtils.h"
#include "exception.hpp"
#include "log.hpp"
#include "AlgoTypes.h"

/*TODO: Create another socket for management functions*/

void dereferencer(int, short, derefHelper * helper)
{
  switch(helper->type)
  {
    case TIMER_SAMPLER:
       (static_cast<Edge*>(helper->edge))->getSample();
       break;

    case TIMER_SEND_SAMPLER:
      (static_cast<Edge*>(helper->edge))->sendSample();
      break;

    case TIMER_GET_PARAMS:
      (static_cast<Edge*>(helper->edge))->getNewEdgeConfigParams();
      break;
  }
}
/*This function handles end of sampling time*/
void Edge::getNewEdgeConfigParams()
{
  EdgeConfigParams tempParams;
  getEdgeConfigParams(&(tempParams));

  /*get Lock*/
  edgeParams = tempParams;
}

/*This function handles sampling at each instance*/
void Edge::getSample()
{
  Logger * logger = Logger::getInstance();
  uint32_t currentSize;
  std::list <Process *>::iterator pIt;
  std::list <Process *> toDelete;
  /*Go through the process vector and get packet count*/
  processLock.lock();
  for (pIt = pro.begin(); pIt != pro.end(); ++pIt)
  {
    /*lock*/
    ((*pIt)->m_lock).lock();
    /*Get size of data received*/
    logger->log("DataRecieved",DEBUG_LEVEL);
    currentSize += ((*pIt)->dataRecieved);
    logger->log(std::to_string(((*pIt)->dataRecieved)).c_str(),DEBUG_LEVEL);
    /*set to zero*/
    ((*pIt)->dataRecieved) = 0;
    /*if state is alarm, for more than burstySampleTime set Edge state Alarm.*/

    /*unlock*/
    ((*pIt)->m_lock).lock();
    /*Do average Calculations*/
    /*Remember to divide by number of connections*/
    /*Set Alarm state for all connections*/

    /*if connected mark for deletion*/
    if ((*pIt)->connected == false)
    {
      toDelete.push_back(*pIt);
      continue;
    }

  }
    /*Delete*/
  pIt = pro.begin();
  std::list <Process *>::iterator toDeleteIterator = toDelete.begin();

  while(pIt != pro.end() && toDeleteIterator != toDelete.end())
  {
    logger->log("Deleting Process",DEBUG_LEVEL);
    if (*pIt == *toDeleteIterator)
    {
      pro.erase(pIt++);
      toDeleteIterator ++;
      continue;
    }
    pIt ++;
  }
  processLock.unlock();
}
/*This function sends the sampling data*/
void Edge::sendSample()
{
  /*copy setSampling Params*/
  /*Call send sampling params*/
}

SamplingHelper::SamplingHelper()
{
	alpha = 0;
}

void SamplingHelper::CalculateAlpha(uint32_t samplingTime, uint32_t samplingDuration)
{
  double s = (double)samplingDuration/ (double)samplingTime;
  alpha = (alpha_t)1.0-(alpha_t)(pow(10,-3/s));
}

alpha_t SamplingHelper::getAlpha()
{
	return alpha;
}

Edge::Edge(uint16_t pt, std::string cip, uint16_t cpn)
{
  sampTimStruct = {TIMER_SAMPLER,this};
  sampDurationTimStruct = {TIMER_GET_PARAMS,this};
  sampSendStruct = {TIMER_SEND_SAMPLER,this};
  cloudIpAddr = cip;
  cloudIpPortNum = cpn;
  eState = enStateNormal;
  datarateAverage = 0;
  port = pt;
  myEdgeNum = UNDEFINED;
  samplingSendTimer = NULL;
  samplingDurationTimer = NULL;
  samplingTimer = NULL;
}

void Edge::initEdge()
{
  Logger * log = Logger::getInstance();
  log->log("Getting Params",DEBUG_LEVEL);
  getEdgeConfigParams(&edgeParams);
  log->log("Got Params",DEBUG_LEVEL);

  log->log("Starting Timer for sampling",DEBUG_LEVEL);
  samplingTimer = timerBase.createTimer(edgeParams.samplingTime,(event_callback_fn)dereferencer,(void *)&sampTimStruct);
  log->log("Created Timer for sampling",DEBUG_LEVEL);

  log->log("Starting Timer for Send Sampling",DEBUG_LEVEL);
  samplingSendTimer = timerBase.createTimer(edgeParams.sampleSendTime,(event_callback_fn)dereferencer,(void *)&sampSendStruct);
  log->log("Created Timer for Send Sampling",DEBUG_LEVEL);

  log->log("Starting Timer for Duration",DEBUG_LEVEL);
  samplingDurationTimer = timerBase.createTimer(edgeParams.samplingDuration,(event_callback_fn)dereferencer,(void *)&sampDurationTimStruct);
  log->log("Created Timer for Duration",DEBUG_LEVEL);


  std::unique_ptr<ServerSocketTcp> temp (new ServerSocketTcp(port));
  tcpServSock = std::move(temp);
  while(1)
  {
    ServerSocketTcp * accepted;
    Process * tempProcess;
    tcpServSock->accept(*accepted);
    tempProcess = new Process(*accepted,eState,cloudIpAddr,cloudIpPortNum);
    processLock.lock();
    pro.push_back(tempProcess);
    processLock.unlock();
    std::thread t(&Process::ProcessLoop,tempProcess);
  }
}

/*TODO: Send port number for port number for management functions*/
int Edge::RegisterWithShadowNet(std::string ip,uint16_t port, std::string location, uint16_t managementPort)
{
  MessageCreator msgCreator;
  MessageParser msgParser;
  uint8_t recvbuffer [MAX_BUFFER_SIZE];
  /*Ignored for now, can be used in the future to improve security*/
  std::string ipResponse;
  uint16_t portResponse;

  uint8_t * message;
  uint16_t length;
  uint8_t msgType;
  msgCreator.setMessageType(REGISTER);
  msgCreator.setEdgeNumber((uint32_t)UNDEFINED);
  msgCreator.setMessage((void*)location.c_str(),(uint16_t)location.size());
  try
  {
    std::unique_ptr<ClientSocketUdp> tempPtr (new ClientSocketUdp(ip,port));
    c_sock = std::move(tempPtr);
    message = msgCreator.getMessage(length);
    c_sock->send(message,length);
    length = (uint16_t)c_sock->recv(recvbuffer,MAX_BUFFER_SIZE,ipResponse,portResponse);
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
  MessageCreator msgCreator;
  MessageParser msgParser;
  uint8_t recvbuffer [MAX_BUFFER_SIZE];
    /*Ignored for now, can be used in the future to improve security*/
  std::string ip;
  uint16_t port;

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
    length = (uint16_t)c_sock->recv(recvbuffer,MAX_BUFFER_SIZE,ip,port);
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
  MessageCreator msgCreator;
  MessageParser msgParser;
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


void SendAlarmPacket(Edge * edge, ShadowPacket sp)
{
  /*In friend class make sure that alarm packets are sent only when edge state not process state is alarm*/
}

Process::Process(ServerSocketTcp sock,EdgeState state,
                std::string cloudip, uint16_t cloudport)
                :m_client_sock(ClientSocketTcp(cloudip,cloudport))
{
  /*When this is false, delete it*/
  connected = true;
  eState = state;
  dataRecieved = 0;
  m_serv_sock = sock;
}
/*This is the server's main loop for one connection*/
void Process::ProcessLoop()
{
  uint8_t buffer[MAXRECV_TCP] = {0};
  uint32_t length;
  try
  {
    length = m_serv_sock.recv(buffer,MAXRECV_TCP);
    m_lock.lock();
    dataRecieved += length;
    m_lock.unlock();
    m_client_sock.send(buffer,length);
  }
  catch(Exception &e)
  {
    connected = false;
  }
}
Process::~Process()
{
  delete (&m_serv_sock);
}

