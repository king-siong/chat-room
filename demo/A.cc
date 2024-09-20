#include <iostream> 
#include <set>
#include <cstdio>
#include <string>
#include <cstring>
#include <cstdlib>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

#include "../utils/common.hh"
#include "../utils/tcp_socket.hh"
#include "../utils/thread_pool.hpp"
#include "../server/server.hh"

constexpr int NUM_THREADS = 16;
constexpr int DEFAULT_PORT = 5657;
std::string DEFAULT_IP = "127.0.0.1";

int main(int argc, char const* argv[]) { 

    TCPSocketPtr server_sock = std::make_shared<TCPSocket>(DEFAULT_IP, DEFAULT_PORT);
    Server server(server_sock);

    if (server.bind() < 0) { return -1; }

    if (server.listen() < 0) { return -1; }

    TCPSocketPtr client_sock = server.accept();

    std::cout << "client B is already connected." << std::endl;

    while (true) {
        std::string msg;
        msg = client_sock->socket_recv();

        if (msg.length() == 0) {
            std::cerr << "the connection to " << *client_sock << " is down." << std::endl;
            break;
        }

        std::cout << "message from B : " << msg << std::endl; 

        std::getline(std::cin, msg);
        while (msg.length() == 0) {
            std::getline(std::cin, msg);
        }
        client_sock->socket_send(msg);
    }

    return EXIT_SUCCESS; 
}
