// Definition of the ServerSocket class

#ifndef SERVER_SOCKET_UDP
#define SERVER_SOCKET_UDP

#include "SocketUdp.h"


class ServerSocketUdp : private SocketUdp
{
 public:

  ServerSocketUdp (const char* ip, int port );
  ServerSocketUdp (){};
  virtual ~ServerSocketUdp();

  const ServerSocketUdp& operator << ( const std::string& ) const;
  const ServerSocketUdp& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size ) ;

  void accept ( ServerSocketUdp& );

};


#endif

