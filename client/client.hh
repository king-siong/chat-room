#ifndef _CHAT_CLIENT_HH_
#define _CHAT_CLIENT_HH_

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>

#include "../utils/tcp_socket.hh"

class Client {
  private:
    TCPSocketPtr _sock;
    std::string _server_ip;
    uint16_t _server_port;

  public:
    Client(const std::string &sever_ip, uint16_t server_port);
    ~Client();

    int connect();
    std::string recv();
    int send(const std::string &msg);
    int shutdown();

    TCPSocketPtr sock();
};

#endif