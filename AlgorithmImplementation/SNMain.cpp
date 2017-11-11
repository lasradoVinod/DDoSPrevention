#include <iostream>
#include <string>
#include <stdlib.h>
#include "ShadowNet.h"

int main(int argc, char ** argv)
{
  if (argc < 3)
    std::cout << "Usage ./SNMain <ip-address> <port>" << std::endl;

  uint16_t port = (uint16_t)atoi(argv[2]);
  ShadowNet shadowNet(std::string(argv[1]),port);
  shadowNet.StartShadowNet();
  while(1);
}
