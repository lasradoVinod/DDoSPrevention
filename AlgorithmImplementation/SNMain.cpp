#include <iostream>
#include <string>
#include <stdlib.h>
#include "log.hpp"
#include "ShadowNet.h"

int main(int argc, char ** argv)
{
  if (argc < 3)
    std::cout << "Usage ./shadownet <ip-address> <port> [logfilename]" << std::endl;

  if (argc == 4)
    Logger::LoggerInit(argv[3]);
  uint16_t port = (uint16_t)atoi(argv[2]);
  ShadowNet shadowNet(std::string(argv[1]),port);
  shadowNet.StartShadowNet();
  while(1);
}
