// Definition of the Socket class

#ifndef SOCKET_UDP
#define SOCKET_UDP

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

#define MAXRECV_UDP  1024

class SocketUdp
{
  
  public:
  SocketUdp();
  virtual ~SocketUdp();

  // Server initialization
  bool create();
  bool bind ( const char * ip, const uint16_t port );
  bool accept ( SocketUdp& ) const;

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
  sockaddr_in m_addr_recv;
  sockaddr_in m_addr;


};


#endif

