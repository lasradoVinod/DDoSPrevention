// Implementation of the ServerSocket class

#include <ServerSocketUdp.h>
#include "exception.hpp"


ServerSocketUdp::ServerSocketUdp (std::string ip, uint16_t port )
{
  if ( ! SocketUdp::create() )
    {
      throw Exception ( "Could not create server socket.",2 );
    }

  if ( ! SocketUdp::bind (ip, port ) )
    {
      throw Exception ( "Could not bind to port.", 2);
    }

}

ServerSocketUdp::~ServerSocketUdp()
{
}


const ServerSocketUdp& ServerSocketUdp::operator << ( const std::string& s ) const
{
  if ( ! SocketUdp::send ( s ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }

  return *this;
}


const ServerSocketUdp& ServerSocketUdp::operator >> ( std::string& s ) const
{
  if ( ! SocketUdp::recv ( s ) )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return *this;
}

const void ServerSocketUdp::send ( const unsigned char * s, int size ) 
{
  if ( ! SocketUdp::send ( s , size ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }
}


const int ServerSocketUdp::recv ( unsigned char * s , int size, std::string & ip, uint16_t &port) 
{
  int readSize;
  if ( (readSize = SocketUdp::recv ( s,size,ip,port )) )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return readSize;
}
