// Definition of the ClientSocket class

#ifndef CLIENT_SOCKET_UDP
#define CLIENT_SOCKET_UDP

#include "SocketUdp.h"


class ClientSocketUdp : private SocketUdp
{
 public:

  ClientSocketUdp (std::string host, int port );
  virtual ~ClientSocketUdp(){};

  const ClientSocketUdp& operator << ( const std::string& ) const;
  const ClientSocketUdp& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size );
  const void close ();

};


#endif

