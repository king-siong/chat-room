#include <iostream> 
#include <cstring> 
#include <string>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h> 
#include <thread>
#include <sys/ioctl.h>
#include <sstream>
#include <mutex>

#include "../utils/common.hh"
#include "../client/client.hh"
#include "../client/utils.hh"

constexpr int DEFAULT_PORT = 5657;
std::string DEFAULT_IP = "127.0.0.1";

int main() {

    Client client(DEFAULT_IP, DEFAULT_PORT);

    if (client.connect() < 0) {
        std::cerr << "connection failed, the server may not be online." << std::endl;
        return -1;
    }

    std::stringstream out_stream;
    std::mutex mtx;
    std::atomic<bool> end(false);

    std::thread send_th(send_msg, &client, &out_stream, &mtx, &end, draw_callback);
    std::thread recv_th(recv_msg, &client, &out_stream, &mtx, &end, draw_callback);

    // wait threads to be released
    send_th.join();
    recv_th.join();

    // close(client_sock); 
    return 0; 
}
