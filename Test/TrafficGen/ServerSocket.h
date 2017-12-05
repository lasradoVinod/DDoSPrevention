// Definition of the ServerSocket class

#ifndef ServerSocket_class
#define ServerSocket_class

#include "Socket.h"


class ServerSocket : private Socket
{
 public:

  ServerSocket (const char* ip, int port );
  ServerSocket (){};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  const void send ( const unsigned char * s, int size ) ;
  const int recv ( unsigned char * s , int size ) ;

  void accept ( ServerSocket& );

};


#endif

