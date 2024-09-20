#include <iostream> 
#include <set>
#include <cstdio>
#include <string>
#include <cstring>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <memory>
#include <csignal>

#include "../utils/common.hh"
#include "../utils/tcp_socket.hh"
#include "../utils/thread_pool.hpp"
#include "../server/utils.hh"
#include "../server/server.hh"

constexpr int NUM_THREADS = 16;
constexpr int DEFAULT_PORT = 5657;
std::string DEFAULT_IP = "127.0.0.1";

int main(int argc, char const* argv[]) {
    signal(SIGPIPE, SIG_IGN);

    TCPSocketPtr server_sock = std::make_shared<TCPSocket>(DEFAULT_IP, DEFAULT_PORT);
    Server server(server_sock);

    // std::cout << server_sock << std::endl;
    std::cout << "server is running on " << *server.sock() << std::endl;

    if (server.bind() < 0) { return -1; }

    if (server.listen() < 0) { return -1; }

    ThreadPool thread_pool(NUM_THREADS);

    // send thread, use msg queue to get msg and send
    thread_pool.enqueue(send_msg, &server);

    while (true) {
        // TCPSocketPtr client_sock = std::make_shared<TCPSocket>(server.accept());
        TCPSocketPtr client_sock = server.accept();

        std::cout << "client " << *client_sock << " has joined the room." << std::endl;

        thread_pool.enqueue(recv_msg, &server, client_sock, thread_callback);
    }

    return 0; 
}
