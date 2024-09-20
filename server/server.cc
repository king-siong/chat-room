#include "./server.hh"

Server::Server(TCPSocketPtr sock) 
    :
    _sock(sock), 
    _client_socks(std::set<TCPSocketPtr>()) {}

Server::~Server() {}

int Server::bind() {
    return _sock->socket_bind();
}

int Server::listen() {
    return _sock->socket_listen();
}

TCPSocketPtr Server::accept() {
    return _sock->socket_accept();
}

std::string Server::recv(TCPSocketPtr client_sock) {
    if (count(client_sock)) {
        return client_sock->socket_recv();
    } else {
        std::cerr << "server: can't find this client " << *client_sock << "." <<std::endl;
        return "";
    }
}
    
int Server::send(TCPSocketPtr client_sock, const std::string &msg) {
    if (count(client_sock)) {
        return client_sock->socket_send(msg);
    } else {
        std::cerr << "server: can't find this client " << *client_sock << "." <<std::endl;
        return -1;
    }
}

// send ``msg'' to some clients: ``client_socks''
void Server::send(std::vector<TCPSocketPtr> &client_socks, const std::string &msg) {
    // TODO: your code here
}

// send ``msg'' to all clients connected to this server
void Server::send_all(const std::string &msg) {
    // TODO: your code here
}

// add the client ``client_sock'' to ``_client_socks''
void Server::add_client(TCPSocketPtr client_sock) {
    // TODO: your code here
}

// delete the client ``client_sock'' from ``_client_socks''
// and shutdown the corresponding socket
void Server::del_client(TCPSocketPtr client_sock) {
    // TODO: your code here
}

bool Server::count(TCPSocketPtr client_sock) {
    std::unique_lock<std::mutex> lock(_mtx);
    return _client_socks.count(client_sock);
}

int Server::shutdown() {
    std::unique_lock<std::mutex> lock(_mtx);
    for (auto client : _client_socks) {
        client->socket_shutdown();
    }
    int ret = _sock->socket_shutdown();
    return ret;
}

void Server::push_msg(const std::string &msg) {
    _msg_queue.push_msg(msg);
}

std::string Server::pop_msg() {
    return _msg_queue.pop_msg();
}

TCPSocketPtr Server::sock() { return _sock; }