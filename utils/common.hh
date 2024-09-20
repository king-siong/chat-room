#ifndef _UTILS_COMMON_HH_
#define _UTILS_COMMON_HH_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

std::ostream &operator<<(std::ostream &output, const sockaddr_in &addr) {
    std::string ip = inet_ntoa(addr.sin_addr);
    uint16_t port = htons(addr.sin_port);
    output << ip << ":" << port;
    return output;
}

#endif