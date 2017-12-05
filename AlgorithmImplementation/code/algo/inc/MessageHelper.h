/*Assumptions.
 * I am not serializing Data.
 * Packed all structures
 * Assuming server and client are on the same endianess
 * No bit errors. Therefore no checksum*/
#include <stdint.h>

#ifndef MESSAGE_CREATOR_H
#define MESSAGE_CREATOR_H
#define MAX_BUFFER_SIZE     128

/*Following macros define the message type*/
#define ERROR_MESSAGE            0x00u
/*Registration of edge with ShadowNet*/
#define REGISTER                 0x01u
/*Get Sampling parameters*/
#define GET_PARAMS               0x02u
/*Send Aggregated Values*/
#define SEND_SAMPLES             0x03u
/*Alarm ShadowNet Packets*/
#define ALARM_PACKETS            0x04u
/*Register the port Number of management port*/
#define REGISTER_MANAGEMENT_PORT 0x05u
/*Change to normal state*/
#define CHANGE_TO_NORMAL         0x06u
/*GENERIC FOR UNDEFINED*/
#define UNDEFINED           0xFFFFFFFFu


/*Message Format is a follows
 * MessageType - Defining the purpose of the message
 * EdgeNum - Unique Edge number alloted when registering with SN
 * length - Length of Payload
 * Payload - actual data
 */
class Message
{
protected:
  uint8_t messageType;
  uint16_t length;
  uint32_t edgeNum;
  uint8_t buffer[MAX_BUFFER_SIZE] = {0};
public:
  Message();
  int getLength(uint16_t *);
  int setMessageType(uint8_t);
  int setEdgeNumber(uint32_t);
  int getMessageType(uint8_t *);
  int getEdgeNumber(uint32_t *);
  int setMessage(void *,uint16_t length);
};


class MessageCreator:public Message
{
public:
  uint8_t * getMessage(uint16_t& length);
};

class MessageParser:public Message
{
public:
  int parseMessage(uint8_t * , uint16_t);
  int copyMessageTo(void *);
};
#endif
