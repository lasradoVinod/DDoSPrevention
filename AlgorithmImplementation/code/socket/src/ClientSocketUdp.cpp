// Implementation of the ClientSocket class

#include <ClientSocketUdp.h>
#include "exception.hpp"


ClientSocketUdp::ClientSocketUdp ( std::string host, uint16_t port )
{
  if ( ! SocketUdp::create() )
    {
      throw Exception ( "Could not create client socket.",-1 );
    }

  if ( ! SocketUdp::connect ( host, port ) )
    {
      throw Exception ( "Could not bind to port.",-1 );
    }

}


const ClientSocketUdp& ClientSocketUdp::operator << ( const std::string& s ) const
{
  if ( ! SocketUdp::send ( s ) )
    {
      throw Exception ( "Could not write to socket.",-1 );
    }

  return *this;

}


const ClientSocketUdp& ClientSocketUdp::operator >> ( std::string& s ) const
{
  if ( ! SocketUdp::recv ( s ) )
    {
      throw Exception ( "Could not read from socket.",-1 );
    }

  return *this;
}

const void ClientSocketUdp::send ( const unsigned char * s, int size ) 
{
  if ( ! SocketUdp::send ( s , size ) )
    {
      throw Exception ( "Could not write to socket.",-1 );
    }
}


const int ClientSocketUdp::recv ( unsigned char * s , int size, std::string & ip, uint16_t &port) 
{
  int readSize;
  readSize = SocketUdp::recv ( s,size,ip,port );    
  return readSize;
}

const void ClientSocketUdp::close ()
{
  SocketUdp::close(); 
}
