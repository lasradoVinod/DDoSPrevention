#ifndef LOG_HPP
#define LOG_HPP

#include <stdint.h>
#include <cstring>
#include <fstream>

#define DEBUG_LEVEL		0
#define WARN_LEVEL		1	
#define ERR_LEVEL		2

class Logger
{
	private:
		static Logger* instance;
		std::ofstream logFile;
		static bool initialized;
		Logger();

	public:
		~Logger();
		int LoggerInit();
		static Logger* getInstance();
		void log(char const *, int);
};

#endif