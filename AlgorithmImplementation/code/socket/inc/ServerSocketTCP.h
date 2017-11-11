// Definition of the ServerSocket class

#ifndef SERVER_SOCKET_TCP
#define SERVER_SOCKET_TCP

#include "SocketTCP.h"


class ServerSocketTcp : private SocketTcp
{
 public:

  ServerSocketTcp (std::string ip, uint16_t port );
  ServerSocketTcp (){};
  virtual ~ServerSocketTcp();

  const ServerSocketTcp& operator << ( const std::string& ) const;
  const ServerSocketTcp& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size ) ;

  void accept ( ServerSocketTcp& );

};


#endif

