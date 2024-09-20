#include "./tcp_socket.hh"

constexpr int MAX_BACKLOG = 16;
constexpr int MAX_BUFSIZE = 1024;

TCPSocket::TCPSocket() 
    : 
    _fd(-1), 
    _ip(""), 
    _port(0) {
    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0) {
        std::cerr << "socket: create error." << std::endl;
    }
}

TCPSocket::TCPSocket(const std::string &ip, uint16_t port) 
    : 
    _fd(-1), 
    _ip(ip), 
    _port(port) {
    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0) {
        std::cerr << "socket: create error." << std::endl;
    }
}

TCPSocket::TCPSocket(int fd, const std::string &ip, uint16_t port) 
    : 
    _fd(fd), 
    _ip(ip), 
    _port(port) {}

TCPSocket::TCPSocket(const TCPSocket &sock) 
    :
    _fd(sock.fd()), 
    _ip(sock.ip()), 
    _port(sock.port()) {}

TCPSocket::TCPSocket(TCPSocket &sock) 
    : 
    _fd(sock.fd()), 
    _ip(sock.ip()), 
    _port(sock.port()) {}

TCPSocket::TCPSocket(TCPSocket&& sock) 
    :
    _fd(std::move(sock._fd)), 
    _ip(std::move(sock._ip)),
    _port(std::move(sock._port)) {}

TCPSocket::~TCPSocket() { close(_fd); }

// to bind a name to the socket, and make it reusable
// return 0 on success; otherwise, return -1
int TCPSocket::socket_bind() {

    // TODO: your code here
    int ret;

    if (ret < 0) {
        std::cerr << "socket: bind error." << std::endl;
    }
    return ret;
}

// to mark the socket as ready to accept incoming connections
// please use MAX_BACKLOG as the backlog argument
// return 0 on success; otherwise, return -1
int TCPSocket::socket_listen() {

    // TODO: your code here
    int ret;

    if (ret < 0) {
        std::cerr << "socket: listen error." << std::endl;
    }
    return ret;
}

// try to connect remote socket through ``ip'' and ``port''
// return 0 on success; otherwise, return -1
int TCPSocket::socket_connect(const std::string &ip, uint16_t port) {

    // TODO: your code here

    int ret;
    if (ret < 0) {
        std::cerr << "socket: connect error." << std::endl;
    }
    return ret;
}

// to accept an incoming connection
// return a pointer to TCPSocket as the new socket
TCPSocketPtr TCPSocket::socket_accept() {

    // TODO: your code here
    int connfd;

    std::string ip = "";
    uint16_t port = 0;
    if (connfd < 0) {
        std::cerr << "socket: accept error." << std::endl;
        return std::make_shared<TCPSocket>(-1, ip, port);
    }
    return std::make_shared<TCPSocket>(connfd, ip, port);
}

// to receive a message
// please use MAX_BUFSIZE as the buffer's size
// return the msg received on success; otherwise, return an empty string
std::string TCPSocket::socket_recv() {
    char buffer[MAX_BUFSIZE] = {0};

    // TODO: your code here
    int ret;

    std::string msg = "";
    if (ret < 0) {
        std::cerr << "socket: recv error." << std::endl;
    } else {
        msg = buffer;
    }
    return msg;
}

// to send a message which is ``msg''
// return 0 on success; otherwise, return -1
int TCPSocket::socket_send(const std::string &msg) {

    // TODO: your code here
    int ret;

    if (ret < 0) {
        std::cerr << "socket: send error." << std::endl;
    }
    return ret;
}

// return the file descriptor
int TCPSocket::fd() const {
    // TODO: your code here
}

// return a string indicating the ip
std::string TCPSocket::ip() const {
    // TODO: your code here
}

// return a unsigned integer indicating the port
uint16_t TCPSocket::port() const {
    // TODO: your code here
}

int TCPSocket::socket_shutdown() {
    int ret = shutdown(_fd, SHUT_RDWR);
    return ret;
}

std::ostream& operator<<(std::ostream &output, const TCPSocket &sock) {
    output << sock.ip() << ":" << sock.port();
    return output;
}

bool TCPSocket::operator<(const TCPSocket &sock) const {
    return fd() < sock.fd();
}