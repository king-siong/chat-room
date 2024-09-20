#ifndef _CHAT_CLIENT_UTILS_H_
#define _CHAT_CLIENT_UTILS_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <set>
#include <sstream>
#include <sys/ioctl.h>
#include <mutex>
#include <atomic>

#include "./client.hh"
#include "../utils/codec.hh"

int draw_callback(const std::string &str, winsize *win, std::mutex *mtx) {
    std::unique_lock<std::mutex> lock(*mtx); // add a lock to ``cout''

    std::cout << "\033c"; // clear the screen
    
    std::cout << str; // output the msg
    std::cout << "\033[" << win->ws_row << ";0H"; // set the position of cursor to left-bottom
    std::cout << "> ";
    std::cout.flush();
    return 0;
}

void send_msg(Client *client, std::stringstream *out_stream, std::mutex *mtx, std::atomic<bool> *end, int (*callback)(const std::string&, winsize*, std::mutex*)) {
    if (client == nullptr) {
        std::cerr << "client send: client does not exist." << std::endl;
        return;
    }
    winsize win;

    Codec encoder;

    while (!end->load()) {
        std::string msg;
        std::getline(std::cin, msg);
        if (msg.length()) {
            msg = encoder.encode(msg);
            client->send(msg);
        } 
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
        // read blank line,
        // call callback fun to update screen
        callback(out_stream->str(), &win, mtx); 
    }
}

void recv_msg(Client *client, std::stringstream *out_stream, std::mutex *mtx, std::atomic<bool> *end, int (*callback)(const std::string&, winsize*, std::mutex*)) {
    if (client == nullptr) {
        std::cerr << "client recv: client does not exist." << std::endl;
        return;
    }
    winsize win;

    Codec decoder;

    while (true) {
        std::string msg = client->recv();
        if (msg.length()) {
            auto msgs = decoder.decode(msg);
            for (auto msg : msgs) {
                *out_stream << msg << "\n";
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
                // msg changed, 
                // call callback fun to update screen
                callback(out_stream->str(), &win, mtx); 
            }
        } else {
            std::cerr << "server is offline" << std::endl;
            end->store(true);
            break;
        }
    }
}

#endif