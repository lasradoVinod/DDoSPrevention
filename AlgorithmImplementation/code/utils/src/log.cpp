#include <iostream>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include "log.hpp"

Logger* Logger::instance;

int Logger::LoggerInit()
{
  logFile.open(".logFile.txt");
  if (!logFile.is_open())
  {
    std::cout << "Cannot open LogFile" << std::endl;
    exit(255);
  }
  return 0;
}

Logger* Logger::getInstance()
{
	if(Logger::instance)
	{
		return (Logger::instance);
	}
	Logger::instance = new Logger();
	return (Logger::instance);
}

Logger::Logger()
{
  LoggerInit();
}

Logger::~Logger()
{
	logFile.close();
}

void Logger::log(char  const * str,int lvl)
{
  if ((uint8_t)lvl <= WARN_LEVEL)
  {
    std::cout << str << std::endl;
    if ((uint8_t)lvl < DEBUG_LEVEL)
    {
      logFile << "LOG: " << str << std::endl;
      return;
    }
    logFile << "WARN: " << str << std::endl;
    return;
  }
  else
  {
    std::cerr << str << std::endl;
    logFile << "ERR: " << str << std::endl;
  }
}
