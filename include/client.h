#ifndef CLIENT_H
#define CLIENT_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#define CRLF     "\r\n"

#define BUF_SIZE 1024

class TCPclient
{
 public:
  TCPclient();
  ~TCPclient();
  int open_connection(const char *address, int port);
  int close_connection();
  int read(uint8_t *buffer);
  int write(const void *buffer, size_t n);

 private:
  uint8_t m_buffer[BUF_SIZE];
  SOCKET m_sock;
  
};


#endif /* guard */
