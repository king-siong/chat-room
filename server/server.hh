#ifndef _CHAT_SERVER_HH_
#define _CHAT_SERVER_HH_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <mutex>
#include <memory>

#include "../utils/tcp_socket.hh"
#include "../utils/msg_queue.hh"

class Server {
  private:
    TCPSocketPtr _sock;
    std::set<TCPSocketPtr> _client_socks;
    MSGQueue _msg_queue;
    std::mutex _mtx;

  public:
    Server(TCPSocketPtr sock);
    ~Server();

    int bind();
    int listen();
    TCPSocketPtr accept();
    std::string recv(TCPSocketPtr client_sock);
    int send(TCPSocketPtr client_sock, const std::string &msg);
    void send(std::vector<TCPSocketPtr> &client_socks, const std::string &msg);
    void send_all(const std::string &msg);
    void add_client(TCPSocketPtr client_sock);
    void del_client(TCPSocketPtr client_sock);
    bool count(TCPSocketPtr client_sock);
    int shutdown();

    void push_msg(const std::string &msg);
    std::string pop_msg();

    TCPSocketPtr sock();
};

#endif