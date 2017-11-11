// Implementation of the SocketTcp class.


#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <SocketTCP.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>


SocketTcp::SocketTcp() :
  m_sock ( -1 )
{

  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

SocketTcp::~SocketTcp()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool SocketTcp::create()
{
  m_sock = socket ( AF_INET,
		    SOCK_STREAM,
		    0 );

  if ( ! is_valid() )
    return false;


  // TIME_WAIT - argh
  int on = 1;
  if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;


  return true;

}



bool SocketTcp::bind (std::string ip, const uint16_t port )
{

  if ( ! is_valid() )
    {
      return false;
    }

  m_addr.sin_family = AF_INET;
  inet_aton(ip.c_str(),&(m_addr.sin_addr));
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( m_sock,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}


bool SocketTcp::listen() const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( m_sock, MAXCONNECTIONS );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool SocketTcp::accept ( SocketTcp& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}


bool SocketTcp::send ( const std::string s ) const
{
  long status = ::send ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int SocketTcp::recv ( std::string& s ) const
{
  char buf [ MAXRECV_TCP + 1 ];

  s = "";

  memset ( buf, 0, MAXRECV_TCP + 1 );

  int status = (int)::recv ( m_sock, buf, MAXRECV_TCP, 0 );

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      s = buf;
      return status;
    }
}


bool SocketTcp::send ( const unsigned char * msg, int size) const
{
  int status = (int)::send ( m_sock, msg, size, MSG_NOSIGNAL );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int SocketTcp::recv ( unsigned char * buf, int buffSize ) const
{

  memset ( buf, 0, buffSize );

  int status = (int)::recv ( m_sock, buf, buffSize,0);

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      return status;
    }
}

bool SocketTcp::connect ( const std::string host, const uint16_t port )
{
  if ( ! is_valid() ) return false;

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons ( (uint16_t)port );

  int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( status == 0 )
    return true;
  else
    return false;
}

void SocketTcp::set_non_blocking ( const bool b )
{

  int opts;

  opts = fcntl ( m_sock,
		 F_GETFL );

  if ( opts < 0 )
    {
      return;
    }

  if ( b )
    opts = ( opts | O_NONBLOCK );
  else
    opts = ( opts & ~O_NONBLOCK );

  fcntl ( m_sock,
	  F_SETFL,opts );

}

void SocketTcp::close()
{
  ::close(m_sock);
}
