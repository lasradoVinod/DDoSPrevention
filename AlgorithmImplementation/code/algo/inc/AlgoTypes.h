#ifndef TYPES_H
	#define TYPES_H

#include <stdint.h>
#include <netinet/in.h>
#include <sys/time.h>

typedef struct timeval timeval;  
/*Beta is the threashold value*/
typedef float beta_t;

/*Alpha is the filter parameter*/
typedef float alpha_t;

/*Type for initial average*/
typedef float average_t;

/*This structure is the one that is going to be exchanged during each
 * initial time period.
 */
typedef struct __attribute__((packed)) __EdgeConfigParams
{
  /*This detemined the time above threshold after which alarm is sounded*/
  uint32_t burstySampleTime;
  /*Threshold for input data rate*/
  beta_t threasholdInput;
  average_t initialAvergeInput;
  /*Currently we are implementing only for input average*/
#if 0
  beta_t threasholdOutput;
  average_t initialAvergeOutput;
#endif
  /*Time in ms*/
  uint32_t samplingTime;
  /*Get new parameters after samplingDuration ms*/
  uint32_t samplingDuration;
  /*Send @SamplingParameters every sampleSendTime ms*/
  uint32_t sampleSendTime;
}EdgeConfigParams;

/*This is the data structure that will be sent to the shadownet every @sampleSendTime */
typedef struct __attribute__((packed)) __SamplingParams
{ 
/*Currently we are implementing it only for data rate*/
#if 0
  /*Connection time*/
	average_t ConnectionTime;
#endif
	/*Data Received*/
	average_t inputPackets;
}SamplingParams;


typedef enum
{
  enStateNormal,
  enStateAlarm
}EdgeState;

typedef struct __attribute__((packed)) __ShadowPacket
{
  /*This is not implemented for lack of time*/
#if 0
  struct sockaddr_in sock;
#endif
  uint16_t size;
}ShadowPacket;

#endif
