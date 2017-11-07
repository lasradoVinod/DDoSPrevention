#include <iostream>
#include <cstring>
#include <fstream>
#include <stdlib.h>
#include "log.hpp"

Logger* Logger::instance;
bool Logger::initialized;

int Logger::LoggerInit()
{
	if (initialized == false)
	{
		logFile.open("./IO/logFile.txt");
		if (!logFile.is_open())
		{
			exit(255);
		}
		initialized = true;
		return 0;
	}
	return -1;
}

Logger* Logger::getInstance()
{
	if(Logger::instance)
	{
		return (Logger::instance);
	}
	Logger::initialized = false; 
	Logger::instance = new Logger();
	return (Logger::instance);
}

Logger::Logger()
{
}

Logger::~Logger()
{
	
}

void Logger::log(char  const * str,int lvl)
{
	if (initialized == true)
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
}