	/*This file declares the functions for Edge*/
#include "AlgoUtils.h"
#include "MessageHelper.h"
#include <string>
#include "AlgoTypes.h"
#include <list>
#include <mutex>
#include <memory>

#include "ClientSocketTCP.h"
#include "ServerSocketTCP.h"
#include "ClientSocketUdp.h"
#include "ServerSocketUdp.h"

#ifndef EDGE_H
	#define EDGE_H

#define TIMER_SAMPLER       0x10
#define TIMER_SEND_SAMPLER  0x11
#define TIMER_GET_PARAMS    0x12

class Edge;

typedef struct __derefHelper
{
  uint8_t type;
  Edge * edge;
}derefHelper;

class SamplingHelper
{
public:
	SamplingHelper();
	alpha_t getAlpha();
	void CalculateAlpha(uint32_t samplingTime, uint32_t samplingDuration);
private:
	alpha_t alpha;
};

class Process
{
private:
  /*When this is false, delete it*/
  EdgeState eState;
  ServerSocketTcp m_serv_sock;
  ClientSocketTcp m_client_sock;
public:
  bool connected;
  std::mutex m_lock;
  uint32_t dataRecieved;

  Process(ServerSocketTcp sock,EdgeState state,std::string cloudip, uint16_t cloudport);
	/*This is the server's main loop for one connection*/
	void ProcessLoop();
	~Process();
};

class Edge
{
public:
	Edge(uint16_t port, std::string cloudIP, uint16_t cloudport);
	/*Not storing this information.
	Connect with the ShadowNet once
	Store edgeNumber returned
	location since parameters depend on location.(Not implemented for now)
	*/
	int RegisterWithShadowNet(std::string ip,uint16_t port,std::string location,uint16_t managementPort);
	/*This function must be called after Registering with ShadowNet
	 * The function opens a TCP socket and listens.
	 * Spanws a thread for each connection and Calls Process with the socket.
	 * The Process class then calculates average data rate.
	 * This function does not return
	 * This function will also spawn a thread which looks into all timer related
	 * things*/
	void initEdge();
	/*This is a friend function that is used in case of alarms
	 * I don't want to use it, but I can't figure out a way to avoid it
	 * with the current design that I have chosen*/
  friend void SendAlarmPacket(Edge *,ShadowPacket);

  /*The following functions are timer callbacks*/

  /*This function handles end of sampling time*/
  void getNewEdgeConfigParams();
  /*This function handles sampling at each instance*/
  void getSample();
  /*This function sends the sampling data*/
  void sendSample();

private:
  /*Gets Edge Parameters from shadownet*/
  int getEdgeConfigParams(EdgeConfigParams * params);
  /*Sends Sampling parameters to the ShadowNet*/
  int sendSamplingParams(SamplingParams * params);

	Timer timerBase;

	std::string cloudIpAddr;
	uint16_t cloudIpPortNum;
	/*Timers for various functions*/
	struct event * samplingTimer;
	struct event * samplingDurationTimer;
	struct event * samplingSendTimer;

	derefHelper sampTimStruct;
	derefHelper sampDurationTimStruct;
	derefHelper sampSendStruct;
	/*Lock for accessing */
	std::mutex edgeParamsLock;
	/*Lock for accessing average parameter*/
	std::mutex averageLock;
	/*Lock for messing with pro*/
	std::mutex processLock;
	/*current average*/
	average_t datarateAverage;
  uint16_t port;
	std::unique_ptr<ClientSocketUdp> c_sock;
	/*My socket*/
	std::unique_ptr<ServerSocketTcp> tcpServSock;
	/*Edge number assigned by the shadownet server*/
	uint32_t myEdgeNum;
	/*Current average*/
	SamplingParams sampleParams;
	/*Current edge parameters*/
	EdgeConfigParams edgeParams;
	/*List of processes*/
	std::list <Process *> pro;
	/*The state of the current edge*/
	EdgeState eState;
};



#endif
