// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "exception.hpp"


ServerSocket::ServerSocket (const char* ip, int port )
{
  if ( ! Socket::create() )
    {
      throw Exception ( "Could not create server socket.",2 );
    }

  if ( ! Socket::bind (ip, port ) )
    {
      throw Exception ( "Could not bind to port.", 2);
    }

  if ( ! Socket::listen() )
    {
      throw Exception ( "Could not listen to socket.",2 );
    }

}

ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
  if ( ! Socket::send ( s ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }

  return *this;

}


const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
  if ( ! Socket::recv ( s ) )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return *this;
}

const void ServerSocket::send ( const unsigned char * s, int size ) 
{
  if ( ! Socket::send ( s , size ) )
    {
      throw Exception ( "Could not write to socket.", 2);
    }
}


const int ServerSocket::recv ( unsigned char * s , int size ) 
{
  int readSize = Socket::recv ( s,size );
  if ( readSize < 0 )
    {
      throw Exception ( "Could not read from socket.",2 );
    }

  return readSize;
}

void ServerSocket::accept ( ServerSocket& sock )
{
  if ( ! Socket::accept ( sock ) )
    {
      throw Exception ( "Could not accept socket.",2);
    }
}

