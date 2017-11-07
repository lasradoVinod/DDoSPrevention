/*
 * messageHelper.cpp
 *
 *  Created on: Oct 28, 2017
 *  Author: Vinod Lasrado
 */

#include <stdint.h>
#include <string.h>
#include "../inc/MessageHelper.h"

#define MESSAGE_OFFSET          7u
#define MESSAGE_TYPE_OFFSET     0u
#define MESSAGE_EDGENUM_OFFSET  1u
#define MESSAGE_LENGTH_OFFSET   5u

int Message::getLength(uint16_t * len)
{
  *len = length;
  return 0;
}
int Message::setMessageType(uint8_t msgType)
{
  messageType = msgType;
  return 0;
}
int Message::setEdgeNumber(uint32_t edgNum)
{
  edgeNum = edgNum;
  return 0;
}
int Message::getMessageType(uint8_t *msgType)
{
  *msgType = messageType;
  return 0;
}
int Message::getEdgeNumber(uint32_t * edgNum)
{
  *edgNum = edgeNum;
  return 0;
}

int Message::setMessage(void * msg,uint16_t len)
{
  length = len;
  if (len != 0)
  memcpy(&buffer[MESSAGE_OFFSET],msg,length);
  return 0;
}

uint8_t * MessageCreator::getMessage(uint16_t & len)
{
  buffer[MESSAGE_TYPE_OFFSET] = messageType;
  (*((uint16_t *)(buffer + MESSAGE_LENGTH_OFFSET))) = length;
  (*((uint32_t *)(buffer + MESSAGE_EDGENUM_OFFSET))) = edgeNum;
  len = (uint16_t)(length + (uint16_t)MESSAGE_OFFSET);
  return buffer;
}

int MessageParser::parseMessage(uint8_t * packet, uint16_t len)
{
  messageType = packet[MESSAGE_TYPE_OFFSET];
  length = (uint16_t)(*((uint16_t *)(packet + MESSAGE_LENGTH_OFFSET)));
  edgeNum = *(uint32_t *)(packet + MESSAGE_EDGENUM_OFFSET);
  memcpy(&buffer[MESSAGE_OFFSET],&packet[MESSAGE_OFFSET],length);
  return 0;
}

int MessageParser::copyMessageTo(void * data)
{
  memcpy(data,&buffer[MESSAGE_OFFSET],length);
 return 0;
}
