#include "./client.hh"

Client::Client(const std::string &server_ip, uint16_t server_port)
    :
    _sock(std::make_shared<TCPSocket>()),
    _server_ip(server_ip), 
    _server_port(server_port) 
    {}
    
Client::~Client() {}

int Client::connect() {
    return _sock->socket_connect(_server_ip, _server_port);
}
    
std::string Client::recv() {
    return _sock->socket_recv();
}
    
int Client::send(const std::string &msg) {
    return _sock->socket_send(msg);
}

int Client::shutdown() {
    int ret = _sock->socket_shutdown();
    return 0;
}

TCPSocketPtr Client::sock() { return _sock; }