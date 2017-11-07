// Definition of the ClientSocket class

#ifndef ClientSocket_class
#define ClientSocket_class

#include "SocketTCP.h"


class ClientSocketTcp : private SocketTcp
{
 public:

  ClientSocketTcp ( std::string host, uint16_t port );
  virtual ~ClientSocketTcp(){};

  const ClientSocketTcp& operator << ( const std::string& ) const;
  const ClientSocketTcp& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size );
  const void close ();

};


#endif

