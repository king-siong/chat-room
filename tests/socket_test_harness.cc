#include "./socket_test_harness.hh"
#include "../utils/thread_pool.hpp"

// SocketTestStep

SocketTestStep::operator std::string() const { return "SocketTestStep"; }

void SocketTestStep::execute(TCPSocketPtr &) const {}

SocketTestStep::~SocketTestStep() {}

// SocketViolation

SocketViolation::SocketViolation(const std::string &msg) : std::runtime_error(msg) {}

SocketViolation SocketViolation::test(const std::string &test_name,
                                                                        const std::string &expected,
                                                                        const std::string &actual) {
    return SocketViolation("TCPSocket testing: " + test_name + ". expected " +
                                          expected + " but read " + actual);
}

// SocketAction

SocketAction::operator std::string() const { return "Action: " + description(); }

std::string SocketAction::description() const { return "description missing"; }

void SocketAction::execute(TCPSocketPtr &) const {}

SocketAction::~SocketAction() {}

// P2P

P2P::P2P(const std::vector<std::string> &client_msgs, const std::vector<std::string> &server_msgs) : 
    _client_msgs(client_msgs.begin(), client_msgs.end()), 
    _server_msgs(server_msgs.begin(), server_msgs.end())
    {
        assert(_client_msgs.size() == _server_msgs.size());
    }

std::string P2P::description() const { return "p2p testing"; }

void P2P::execute(TCPSocketPtr &sock) const {
    std::string ip = sock->ip();
    uint16_t port = sock->port();

    Server server(sock);

    if (server.bind() < 0) {
        throw SocketViolation("server: bind error");
    }

    if (server.listen() < 0) {
        throw SocketViolation("server: listen error");
    }

    std::thread server_th([&] {
        TCPSocketPtr client_sock = sock->socket_accept();

        size_t _ = _server_msgs.size();

        for (size_t i = 0; i < _; i++) {
            std::string msg = client_sock->socket_recv();
            if (msg != _client_msgs[i]) {
                throw SocketViolation::test("p2p client msgs check", _client_msgs[i], msg);
            }

            client_sock->socket_send(_server_msgs[i]);
        }
    });

    std::thread client_th([&] {
        Client client(ip, port);

        if (client.connect() < 0) {
            throw SocketViolation("client: connect error");
        }

        size_t _ = _client_msgs.size();

        for (size_t i = 0; i < _; i++) {
            client.send(_client_msgs[i]);

            std::string msg = client.recv();
            if (msg != _server_msgs[i]) {
                throw SocketViolation::test("p2p server msgs check", _server_msgs[i], msg);
            }
        }
    });

    server_th.join();
    client_th.join();
}

// GetFd

GetFd::GetFd(int fd) : _fd(fd) {}

std::string GetFd::description() const { return "get file descriptor"; }

void GetFd::execute(TCPSocketPtr &sock) const {
    int fd = sock->fd();
    if (fd != _fd) {
        throw SocketViolation::test("fd", std::to_string(_fd), std::to_string(fd));
    }
}

// GetIP

GetIP::GetIP(const std::string &ip) : _ip(ip) {}

std::string GetIP::description() const { return "get ip"; }

void GetIP::execute(TCPSocketPtr &sock) const {
    auto ip = sock->ip();
    if (ip != _ip) {
        throw SocketViolation::test("ip", _ip, ip);
    }
}

// GetPort

GetPort::GetPort(uint16_t port) : _port(port) {}

std::string GetPort::description() const { return "get port"; }

void GetPort::execute(TCPSocketPtr &sock) const {
    auto port = sock->port();
    if (port != _port) {
        throw SocketViolation::test("port", std::to_string(_port), std::to_string(port));
    }
}

// CheckBind

CheckBind::CheckBind() {}

std::string CheckBind::description() const { return "bind"; }

void CheckBind::execute(TCPSocketPtr &sock) const {
    int ret = sock->socket_bind();
    if (ret < 0) {
        throw SocketViolation("bind error, make sure the socket address is reusable");
    }
}

// TestHarness

SocketTestHarness::SocketTestHarness(const std::string &test_name, const std::string &ip, uint16_t port) : 
    _test_name(test_name), 
    _sock(std::make_shared<TCPSocket>(ip, port))
    {}

void SocketTestHarness::execute(const SocketTestStep &step) {
    try {
        step.execute(_sock);
    } catch (const SocketViolation &e) {
        std::cerr << "Test Failure on action:\n\t" << std::string(step);
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw SocketViolation("The test \"" + _test_name + "\" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << std::string(step);
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw SocketViolation("The test \"" + _test_name +
                                             "\" caused your implementation to throw an exception!");
    }
}