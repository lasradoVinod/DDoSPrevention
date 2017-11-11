// Definition of the Socket class

#ifndef SOCKET_TCP_H
#define SOCKET_TCP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXCONNECTIONS = 5;
#define MAXRECV_TCP   1024

class SocketTcp
{
 public:
  SocketTcp();
  virtual ~SocketTcp();

  // Server initialization
  bool create();
  bool bind ( std::string ip, const uint16_t port );
  bool listen() const;
  bool accept ( SocketTcp& ) const;

  // Client initialization
  bool connect ( const std::string host, const uint16_t port );

  // Data Transimission
  bool send ( const std::string ) const;
  int recv ( std::string& ) const;
  
  bool send ( const unsigned char * msg, int size) const;

  int recv ( unsigned char * buf, int buffSize ) const;

  void set_non_blocking ( const bool );

  void close ();

  bool is_valid() const { return m_sock != -1; }

 private:

  int m_sock;
  sockaddr_in m_addr;


};


#endif

