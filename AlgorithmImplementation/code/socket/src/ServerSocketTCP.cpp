// Implementation of the ServerSocket class

#include "ServerSocketTCP.h"

#include "exception.hpp"


ServerSocketTcp::ServerSocketTcp ( uint16_t port )
{
  if ( ! SocketTcp::create() )
    {
      throw Exception ( "Could not create server socket.",2 );
    }

  if ( ! SocketTcp::bind ( port ) )
    {
      throw Exception ( "Could not bind to port.", 2);
    }

  if ( ! SocketTcp::listen() )
    {
      throw Exception ( "Could not listen to socket.",2 );
    }

}

ServerSocketTcp::~ServerSocketTcp()
{
}


const ServerSocketTcp& ServerSocketTcp::operator << ( const std::string& s ) const
{
  if ( ! SocketTcp::send ( s ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }

  return *this;

}


const ServerSocketTcp& ServerSocketTcp::operator >> ( std::string& s ) const
{
  if ( ! SocketTcp::recv ( s ) )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return *this;
}

const void ServerSocketTcp::send ( const unsigned char * s, int size ) 
{
  if ( ! SocketTcp::send ( s , size ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }
}


const int ServerSocketTcp::recv ( unsigned char * s , int size ) 
{
  int readSize;
  if ( (readSize = SocketTcp::recv ( s,size )) )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return readSize;
}

void ServerSocketTcp::accept ( ServerSocketTcp& sock )
{
  if ( ! SocketTcp::accept ( sock ) )
    {
      throw Exception ( "Could not accept socket.",2);
    }
}

