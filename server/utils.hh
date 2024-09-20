#ifndef _CHAT_SERVER_UTILS_H_
#define _CHAT_SERVER_UTILS_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <set>
#include <memory>

#include "./server.hh"
#include "../utils/codec.hh"

int thread_callback(Server *server, TCPSocketPtr client_sock) {
    // delete cilent socket
    server->del_client(client_sock);
    std::cerr << "client fd " << client_sock->fd() << " is released." << std::endl;
    return 0;
}

void recv_msg(Server *server, 
        TCPSocketPtr client_sock, 
        int (*callback)(Server*, TCPSocketPtr)) {
    
    server->add_client(client_sock);

    std::stringstream msg_sstream;
    msg_sstream << *client_sock << " has logged in.";
    server->push_msg(msg_sstream.str());
    msg_sstream.str("");

    Codec decoder;
    
    while (true) {
        std::string msg;
        msg = server->recv(client_sock);

        if (msg.length() == 0) {
            std::cerr << "the connection to " << *client_sock << " is down." << std::endl;
            break;
        }

        std::cout << "message from client " << *client_sock << " : " << msg << std::endl;

        auto msgs = decoder.decode(msg);

        for (const auto &msg : msgs) {
            msg_sstream.clear();
            msg_sstream << "(" << *client_sock << "): " << msg;
            server->push_msg(msg_sstream.str());
            msg_sstream.str("");
        }
        
    }
    
    // call back to delete the socket
    callback(server, client_sock);
}

void send_msg(Server *server) {
    Codec encoder;

    while (true) {
        std::string msg = server->pop_msg();
        msg = encoder.encode(msg);
        server->send_all(msg);
    }
}

#endif