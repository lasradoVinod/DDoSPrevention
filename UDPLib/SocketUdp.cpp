// Implementation of the Socket class.
#include "SocketUdp.h"
#include "string.h"
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>


SocketUdp::SocketUdp() :
  m_sock ( -1 )
{

  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

SocketUdp::~SocketUdp()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool SocketUdp::create()
{
  m_sock = socket ( AF_INET,
		    SOCK_DGRAM,
		    0 );

  if ( ! is_valid() )
    return false;

  // TIME_WAIT - argh
  int on = 1;
  if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    return false;

  return true;

}

bool SocketUdp::bind ( const char * ip, const int port )
{

  if ( ! is_valid() )
    {
      return false;
    }

  m_addr.sin_family = AF_INET;
  inet_aton(ip,&(m_addr.sin_addr));
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


bool SocketUdp::send ( const std::string s ) const
{
  socklen_t lenStruct = sizeof (struct sockaddr_in);
  int status = ::sendto ( m_sock, s.c_str(), s.size(), MSG_NOSIGNAL, (const struct sockaddr *)&m_addr, lenStruct);
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


int SocketUdp::recv ( std::string& s ) const
{

  socklen_t lenStruct = sizeof (struct sockaddr_in);
  char buf [ MAXRECV + 1 ];

  s = "";

  memset ( buf, 0, MAXRECV + 1 );

  int status = ::recvfrom ( m_sock, buf, MAXRECV, 0,  (struct sockaddr *)&m_addr, &lenStruct);

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


bool SocketUdp::send ( const unsigned char * msg, int size) const
{
  socklen_t lenStruct = sizeof (struct sockaddr_in);
  int status = ::sendto ( m_sock, msg, size, MSG_NOSIGNAL, (struct sockaddr *)&m_addr, lenStruct );
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}

int SocketUdp::recv ( unsigned char * buf, int buffSize ) const
{
  socklen_t lenStruct = sizeof (struct sockaddr_in);
  memset ( buf, 0, buffSize );

  int status = ::recvfrom ( m_sock, buf, buffSize, 0, (struct sockaddr *)&m_addr, &lenStruct );

  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in SocketUdp::recv\n";
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

bool SocketUdp::connect ( const std::string host, const int port )
{
  if ( ! is_valid() ) return false;

  m_addr.sin_family = AF_INET;
  m_addr.sin_port = htons ( port );

  int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  return true;
}

void SocketUdp::set_non_blocking ( const bool b )
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

void SocketUdp::close()
{
  ::close(m_sock);
}
