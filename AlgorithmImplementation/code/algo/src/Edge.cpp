#include <iostream>
#include <Edge.h>
#include <stdint.h>
#include <thread>
#include <math.h>

#include "AlgoUtils.h"
#include "exception.hpp"
#include "log.hpp"
#include "AlgoTypes.h"

/*Dereferencer function for timer tasks*/
void dereferencer(void * helper_arg)
{
  derefHelper * helper = (derefHelper*)helper_arg;
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
/*Currently this function will not be called.*/
void Edge::getNewEdgeConfigParams()
{
  EdgeConfigParams tempParams;
  getEdgeConfigParams(&(tempParams));

  edgeParamsLock.lock();
  edgeParams = tempParams;
  sampHelper.CalculateAlpha(edgeParams.samplingTime,edgeParams.samplingDuration);
  edgeParamsLock.unlock();
  samplingParamsLock.lock();
  sampleParams.inputPackets = edgeParams.initialAvergeInput;
  samplingParamsLock.unlock();
}

/*This function handles sampling at each instance*/
void Edge::getSample()
{
  Logger * logger = Logger::getInstance();
  uint32_t countRecv = 0;
  uint32_t currentSize = 0;
  std::list <Process *>::iterator pIt;
  std::list <Process *> toDelete;
  /*Go through the process vector and get packet count*/
  processLock.lock();
  for (pIt = pro.begin(); pIt != pro.end(); ++pIt)
  {
    /*lock*/
    ((*pIt)->m_lock).lock();
    /*Get size of data received*/
    if ((*pIt)->dataRecieved != 0)
    {
      currentSize += ((*pIt)->dataRecieved);
      ((*pIt)->dataRecieved);
      countRecv ++;
    }
    logger->log(std::string ("dataRecieved") + std::to_string(((*pIt)->dataRecieved)),DEBUG_LEVEL);
    /*set to zero*/
    ((*pIt)->dataRecieved) = 0;
    /*unlock*/
    ((*pIt)->m_lock).unlock();
    /*if connected mark for deletion*/
    if ((*pIt)->connected == false)
    {
      toDelete.push_back(*pIt);
    }
  }

  if (countRecv > 0)
  {
    average_t tempAverage = (average_t)currentSize/(average_t)countRecv;
    logger->log(std::string("Average ")+std::to_string(tempAverage),INFO_LEVEL);

    /*Do average Calculations*/
    float tempAlpha = sampHelper.getAlpha();
    tempAlpha = (tempAlpha * tempAverage) + ((1-tempAlpha) * sampleParams.inputPackets);
    logger->log(std::string("average ")+std::to_string(tempAlpha),DEBUG_LEVEL);
    samplingParamsLock.lock();
    sampleParams.inputPackets = tempAlpha;
    samplingParamsLock.unlock();
    logger->log(std::string("LongTermAverage ")+std::to_string(sampleParams.inputPackets),INFO_LEVEL);

    average_t deviation = tempAverage - sampleParams.inputPackets;

    /*threshold Input is the variance*/
    if (deviation > edgeParams.threasholdInput)
    {
      if (burstySamples < edgeParams.burstySampleTime)
      {
        burstySamples ++;
      }
      else
      {
        eState = enStateAlarm;
        turnOffTimers();
      }
    }
    else
    {
      burstySamples = 0;
    }

    logger->log(std::string("Bursty Sample Count")+std::to_string(burstySamples),DEBUG_LEVEL);
  }
    /*Delete*/
  pIt = pro.begin();
  std::list <Process *>::iterator toDeleteIterator = toDelete.begin();
  std::list <Process *>::iterator tempIt;
  while(pIt != pro.end() && toDeleteIterator != toDelete.end())
  {
    logger->log("Deleting Process",INFO_LEVEL);
    if (*pIt == *toDeleteIterator)
    {
      tempIt = pIt++;
      pro.erase(tempIt);
      toDeleteIterator ++;
      continue;
    }
    pIt ++;
  }
  if (eState == enStateAlarm)
  {
    for (pIt = pro.begin(); pIt != pro.end();pIt ++)
    {
      (*pIt)->eState = enStateAlarm;
    }
  }
  processLock.unlock();
}
/*This function sends the sampling data*/
void Edge::sendSample()
{
  uint8_t * message;
  uint16_t length;
  MessageCreator msgCreator;
  msgCreator.setEdgeNumber(myEdgeNum);
  msgCreator.setMessageType(SEND_SAMPLES);
  samplingParamsLock.lock();
  msgCreator.setMessage((void *)&sampleParams,sizeof(SamplingParams));
  samplingParamsLock.unlock();
  message = msgCreator.getMessage(length);
  cSockLock.lock();
  c_sock->send(message,length);
  cSockLock.unlock();
}

/*Class used to calculate alpha*/
SamplingHelper::SamplingHelper()
{
	alpha = 0;
}
/*Setter for alpha*/
void SamplingHelper::CalculateAlpha(uint32_t samplingTime, uint32_t samplingDuration)
{
  double s = (double)samplingDuration/ (double)samplingTime;
  alpha = (alpha_t)1.0-(alpha_t)(pow(10,-3/s));
}
/*Getter for alpha*/
alpha_t SamplingHelper::getAlpha()
{
	return alpha;
}

/*Constructor for edge*/
Edge::Edge(std::string mIp,uint16_t pt, std::string cip, uint16_t cpn)
{
  burstySamples = 0;
  manPort = MANAGEMENT_PORT;
  myIp = mIp;
  sampTimStruct = {TIMER_SAMPLER,this};
  sampDurationTimStruct = {TIMER_GET_PARAMS,this};
  sampSendStruct = {TIMER_SEND_SAMPLER,this};
  cloudIpAddr = cip;
  cloudIpPortNum = cpn;
  eState = enStateNormal;
  port = pt;
  myEdgeNum = UNDEFINED;
  samplingSendTimer = NULL;
  samplingDurationTimer = NULL;
  samplingTimer = NULL;
}
/*Turns on the timers based on the edge config parameters*/
void Edge::turnOnTimers()
{
  Logger * log = Logger::getInstance();
  log->log("Starting Timer for sampling",DEBUG_LEVEL);
  samplingTimer = timerBase.createTimer(edgeParams.samplingTime,dereferencer,(void *)&sampTimStruct);
  log->log("Created Timer for sampling",DEBUG_LEVEL);

  log->log("Starting Timer for Send Sampling",DEBUG_LEVEL);
  samplingSendTimer = timerBase.createTimer(edgeParams.sampleSendTime,dereferencer,(void *)&sampSendStruct);
  log->log("Created Timer for Send Sampling",DEBUG_LEVEL);

  log->log("Starting Timer for Duration",DEBUG_LEVEL);
  samplingDurationTimer = timerBase.createTimer(edgeParams.samplingDuration,dereferencer,(void *)&sampDurationTimStruct);
  log->log("Created Timer for Duration",DEBUG_LEVEL);

}
/*Turns off Timers*/
void Edge::turnOffTimers()
{
  timerBase.stopTimer(samplingTimer);
  timerBase.stopTimer(samplingSendTimer);
  timerBase.stopTimer(samplingDurationTimer);
}
/*Initializing edge*/
void Edge::initEdge()
{
  try
  {
    Logger * log = Logger::getInstance();
    log->log("Getting Params",DEBUG_LEVEL);
    getEdgeConfigParams(&edgeParams);

    sampHelper.CalculateAlpha(edgeParams.samplingTime,edgeParams.samplingDuration);

    sampleParams.inputPackets = edgeParams.initialAvergeInput;
    turnOnTimers();

    log->log("Creating Port for management functions",DEBUG_LEVEL);
    std::unique_ptr<ServerSocketUdp> tempPtrMan (new ServerSocketUdp(myIp,manPort));
    manageSock = std::move(tempPtrMan);
    std::thread th (&Edge::edgeManager,this);
    th.detach();

    std::unique_ptr<ServerSocketTcp> temp (new ServerSocketTcp(myIp, port));
    tcpServSock = std::move(temp);
    while(1)
    {
      ServerSocketTcp * accepted = new ServerSocketTcp();
      Process * tempProcess;
      tcpServSock->accept(*accepted);
      tempProcess = new Process(accepted,eState,cloudIpAddr,cloudIpPortNum,this);
      processLock.lock();
      pro.push_back(tempProcess);
      processLock.unlock();
      std::thread * t = new std::thread(&Process::ProcessLoop,tempProcess);
      t->detach();
    }
  }
  catch(Exception &e)
  {
    std::cerr << __FUNCTION__ <<  e.getMessage() << std::endl;
    exit(0);
  }
}
/*Registering Edge with shadow net*/
int Edge::RegisterWithShadowNet(std::string ip,uint16_t port, std::string location)
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
    std::cout << myEdgeNum << std::endl;
  }
  else
    return -1;
  try
  {
    msgCreator.setEdgeNumber(myEdgeNum);
    msgCreator.setMessageType(REGISTER_MANAGEMENT_PORT);
    msgCreator.setMessage(&manPort,sizeof (uint16_t));
    message = msgCreator.getMessage(length);
    c_sock->send(message,length);
    length = (uint16_t)c_sock->recv(recvbuffer,MAX_BUFFER_SIZE,ipResponse,portResponse);
  }
  catch(Exception& ex)
  {
    std::cerr << __FUNCTION__ << ex.getMessage() << std::endl;
    return -1;
  }
  msgParser.parseMessage(recvbuffer,length);
  msgParser.getMessageType(&msgType);
  if(msgType != REGISTER_MANAGEMENT_PORT)
  {
    return -1;
  }
	return 0;
}
/*Gets Edge Parameters from shadownet*/
int Edge::getEdgeConfigParams(EdgeConfigParams * params)
{
  Logger * log = Logger::getInstance();
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
    std::cerr << __FUNCTION__ << ex.getMessage() << std::endl;
    return -1;
  }
  msgParser.parseMessage(recvbuffer,length);
  msgParser.getMessageType(&msgType);
  if(msgType == GET_PARAMS)
  {
    uint32_t tempVal;
    msgParser.getEdgeNumber(&tempVal);
    if (tempVal == myEdgeNum)
    {
      msgParser.copyMessageTo(params);
    }
    else
    {
      log->log("Mismatch in edge number",ERR_LEVEL);
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
    cSockLock.lock();
    c_sock->send(message,length);
    cSockLock.unlock();
  }
  catch(Exception& ex)
  {
    std::cerr << ex.getMessage() << std::endl;
    return -1;
  }
  return 0;
}
/*Sending alarm packets*/
void Process::SendAlarmPacket(Edge * edge, ShadowPacket sp)
{
  uint8_t * buffer;
  uint16_t lengthPack;
  MessageCreator msgCreator;
  msgCreator.setMessageType(ALARM_PACKETS);
  msgCreator.setEdgeNumber(edge->myEdgeNum);
  msgCreator.setMessage((void*)&sp,sizeof (SamplingParams));
  buffer = msgCreator.getMessage(lengthPack);
  (edge->cSockLock).lock();
  edge->c_sock->send(buffer,lengthPack);
  (edge->cSockLock).unlock();
}

Process::Process(ServerSocketTcp * sock,EdgeState state,
                std::string cloudip, uint16_t cloudport,Edge * edge)
                :m_client_sock(ClientSocketTcp(cloudip,cloudport)),myParentEdge(edge)
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
  uint16_t length;
  try
  {
    while(1)
    {
      length = (uint16_t)m_serv_sock->recv(buffer,MAXRECV_TCP);
      m_serv_sock->send(buffer,length);

      m_lock.lock();
      dataRecieved += length;
      m_lock.unlock();
      m_client_sock.send(buffer,length);
      m_client_sock.recv(buffer,MAXRECV_TCP);

      if (eState != enStateNormal)
      {
        ShadowPacket sp;
        sp.size = length;
        std::cout << sp.size << std::endl;
        SendAlarmPacket(myParentEdge,sp);
      }
    }
  }
  catch(Exception &e)
  {
    std::cerr << __FUNCTION__ << e.getMessage() << std::endl;
    connected = false;
  }
}
/*delete socket*/
Process::~Process()
{
  delete (m_serv_sock);
}

void Edge::edgeManager()
{
  Logger * logger = Logger::getInstance();
  uint8_t eMBuffer[MAXRECV_UDP + 1];
  std::string recvIp;
  uint16_t recvPort;
  uint16_t recvLength;
  uint8_t eMMessageType;
  MessageParser msgParser;
  std::list<Process*>::iterator pIt;
  while(1)
  {
    recvLength = (uint16_t) manageSock->recv(eMBuffer,MAXRECV_UDP,recvIp,recvPort);
    msgParser.parseMessage(eMBuffer,recvLength);
    msgParser.getMessageType(&eMMessageType);
    switch (eMMessageType)
    {
      case CHANGE_TO_NORMAL:
        logger->log("Got relax Message",DEBUG_LEVEL);
        if (eState != enStateNormal)
        {
          eState = enStateNormal;
          getNewEdgeConfigParams();
          processLock.lock();
          for(pIt = pro.begin();pIt != pro.end();pIt ++)
          {
            (*pIt)->eState = enStateNormal;
            (*pIt)->dataRecieved = 0;
          }
          burstySamples = 0;
          processLock.unlock();
          logger->log("turning on timers",INFO_LEVEL);
          turnOnTimers();
        }
        break;

      default:
        break;
    }
  }
}
