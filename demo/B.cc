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

constexpr int DEFAULT_PORT = 5657;
std::string DEFAULT_IP = "127.0.0.1";

int main() {

    Client client(DEFAULT_IP, DEFAULT_PORT);

    if (client.connect() < 0) {
        std::cerr << "connection: failed, the server may not be online." << std::endl;
        return -1;
    }

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        while (msg.length() == 0) {
            std::getline(std::cin, msg);
        }

        client.send(msg);

        msg = client.recv();

        std::cout << "message from A : " << msg << std::endl;
    }

    return 0; 
}
