#ifndef LOG_HPP
#define LOG_HPP

#include <stdint.h>
#include <cstring>
#include <fstream>
#include <mutex>

#ifndef CURR_LEVEL
  #define CURR_LEVEL  3
#endif

#define INFO_LEVEL    0
#define DEBUG_LEVEL		3
#define WARN_LEVEL		2
#define ERR_LEVEL		  1

class Logger
{
	private:
		static Logger* instance;
		static std::ofstream logFile;
		std::mutex fileLock;
		Logger();

	public:
		~Logger();
		static int LoggerInit(std::string fileName);
		static Logger* getInstance();
		void log(char const *, int);
		void log(std::string str,int lvl);
};

#endif
