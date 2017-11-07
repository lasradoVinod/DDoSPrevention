// Implementation of the ClientSocket class

#include "ClientSocketTCP.h"
#include "exception.hpp"


ClientSocketTcp::ClientSocketTcp ( std::string host, uint16_t port )
{
  if ( ! SocketTcp::create() )
    {
      throw Exception ( "Could not create client socket.",-1 );
    }

  if ( ! SocketTcp::connect ( host, port ) )
    {
      throw Exception ( "Could not bind to port.",-1 );
    }

}


const ClientSocketTcp& ClientSocketTcp::operator << ( const std::string& s ) const
{
  if ( ! SocketTcp::send ( s ) )
    {
      throw Exception ( "Could not write to socket.",-1 );
    }

  return *this;

}


const ClientSocketTcp& ClientSocketTcp::operator >> ( std::string& s ) const
{
  if ( ! SocketTcp::recv ( s ) )
    {
      throw Exception ( "Could not read from socket.",-1 );
    }

  return *this;
}

const void ClientSocketTcp::send ( const unsigned char * s, int size ) 
{
  if ( ! SocketTcp::send ( s , size ) )
    {
      throw Exception ( "Could not write to socket.",-1 );
    }
}


const int ClientSocketTcp::recv ( unsigned char * s , int size ) 
{
  int readSize;
  readSize = SocketTcp::recv ( s,size );
  return readSize;
}

const void ClientSocketTcp::close ()
{
  SocketTcp::close();
}
