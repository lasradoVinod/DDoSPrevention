#include <iostream>
#include <unistd.h>
#include "Edge.h"
#include <time.h>
#include "AlgoUtils.h"

using namespace std;

int main(int argc, char ** argv)
{
  if (argc < 8)
  {
    std::cout << "Usage ./edge <edge-ip-address> <edge-port> <cloud-ip-address> <cloud-port> <shadownet-ip> <shadownet-port> <location>" << std::endl;
    exit(0);
  }
  Edge edge(std::string(argv[1]),(uint16_t)atoi(argv[2]),std::string(argv[3]),(uint16_t)atoi(argv[4]));
  edge.RegisterWithShadowNet(std::string(argv[5]),(uint16_t)atoi(argv[6]),std::string(argv[7]));
  edge.initEdge();
  return 0;
}
