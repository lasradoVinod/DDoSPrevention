#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdint.h>
#include <string>

class Exception
{
	private:
		uint8_t level;
		std::string msg;
	public:
		Exception(char const *, uint8_t);
		std::string getMessage();
		uint8_t getLevel();
};

#endif