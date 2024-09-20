#ifndef _TCP_SOCKET_HH_
#define _TCP_SOCKET_HH_

#include <iostream>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <memory>

class TCPSocket;

using TCPSocketPtr = std::shared_ptr<TCPSocket>;

class TCPSocket {
  private:
    int _fd;              // file descriptor of the socket
    std::string _ip;      // ip that the socket will bind to
    uint16_t _port;       // port that the socket will bind to

  public:

    TCPSocket();
    TCPSocket(const std::string &ip, uint16_t port);
    TCPSocket(int fd, const std::string &ip, uint16_t port);
    TCPSocket(const TCPSocket &sock);
    TCPSocket(TCPSocket &sock);
    TCPSocket(TCPSocket&& sock);
    ~TCPSocket();

    int socket_bind();
    int socket_listen();
    int socket_connect(const std::string &ip, uint16_t port);
    TCPSocketPtr socket_accept();
    int socket_shutdown();

    std::string socket_recv();
    int socket_send(const std::string &msg);

    int fd() const ;
    std::string ip() const ;
    uint16_t port() const ;

    friend std::ostream &operator<<(std::ostream &output, const TCPSocket &sock);
    bool operator<(const TCPSocket &sock) const ;
};

#endif