#include <stdint.h>
#include <cstring>
#include "exception.hpp"

Exception::Exception(char const * str, uint8_t lvl)
{
	msg.append(str);
	level = lvl;
}

std::string Exception::getMessage()
{
	return msg;
}

uint8_t Exception::getLevel()
{
	return level;
}
