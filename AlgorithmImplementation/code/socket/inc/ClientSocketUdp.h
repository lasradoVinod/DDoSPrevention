// Definition of the ClientSocket class

#ifndef CLIENT_SOCKET_UDP
#define CLIENT_SOCKET_UDP

#include "SocketUdp.h"


class ClientSocketUdp : private SocketUdp
{
 public:

  ClientSocketUdp (std::string host, uint16_t port );
  virtual ~ClientSocketUdp(){};

  const ClientSocketUdp& operator << ( const std::string& ) const;
  const ClientSocketUdp& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size, std::string & ip, uint16_t &port );
  const void close ();

};


#endif

