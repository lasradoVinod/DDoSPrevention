#include <iostream>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include "log.hpp"

Logger* Logger::instance;
std::ofstream Logger::logFile;

int Logger::LoggerInit(std::string fileName)
{
  logFile.open(fileName.c_str());
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
  if (!logFile.is_open())
  {
    LoggerInit(".log.txt");
  }
}

Logger::~Logger()
{
	logFile.close();
}

void Logger::log(char const * str,int lvl)
{
  fileLock.lock();
  switch(CURR_LEVEL)
  {
  case DEBUG_LEVEL:
    if (lvl==DEBUG_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "DEB: " << str << std::endl;
    }

  case WARN_LEVEL:
    if (lvl==WARN_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "WARN: " << str << std::endl;
    }

  case ERR_LEVEL:
    if (lvl==ERR_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "ERR: " << str << std::endl;
    }

  case INFO_LEVEL:
    if (lvl==INFO_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "INFO: " << str << std::endl;
    }

  default:
      break;
  }
  fileLock.unlock();
}

void Logger::log(std::string str,int lvl)
{
  fileLock.lock();
  switch(CURR_LEVEL)
  {
  case DEBUG_LEVEL:
    if (lvl==DEBUG_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "DEB: " << str << std::endl;
    }

  case WARN_LEVEL:
    if (lvl==WARN_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "WARN: " << str << std::endl;
    }

  case ERR_LEVEL:
    if (lvl==ERR_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "ERR: " << str << std::endl;
    }

  case INFO_LEVEL:
    if (lvl==INFO_LEVEL)
    {
      std::cout << str << std::endl;
      logFile << "INFO: " << str << std::endl;
    }

  default:
      break;
  }
  fileLock.unlock();
}
