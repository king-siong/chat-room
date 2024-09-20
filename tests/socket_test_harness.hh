#ifndef _SOCKET_TEST_HARNESS_HH_
#define _SOCKET_TEST_HARNESS_HH_

#include "../utils/tcp_socket.hh"
#include "../server/server.hh"
#include "../client/client.hh"

#include <thread>
#include <exception>
#include <mutex>
#include <cassert>

struct SocketTestStep {
    virtual operator std::string() const;
    virtual void execute(TCPSocketPtr &) const;
    virtual ~SocketTestStep();
};

class SocketViolation : public std::runtime_error {
  public:
    SocketViolation(const std::string &msg);

    static SocketViolation test(const std::string &test_name,
                                                   const std::string &expected,
                                                   const std::string &actual);
};

struct SocketAction : public SocketTestStep {
    operator std::string() const override;
    virtual std::string description() const;
    virtual void execute(TCPSocketPtr &) const override;
    virtual ~SocketAction() override;
};

// -----------------------------------------------------------

struct P2P : public SocketAction {
    std::vector<std::string> _client_msgs;
    std::vector<std::string> _server_msgs;

    P2P(const std::vector<std::string> &, const std::vector<std::string> &);
    std::string description() const override;
    void execute(TCPSocketPtr &) const override;
};

struct GetFd : public SocketAction {
    int _fd;

    GetFd(int);
    std::string description() const override;
    void execute(TCPSocketPtr &) const override;
};

struct GetIP : public SocketAction {
    std::string _ip;

    GetIP(const std::string &);
    std::string description() const override;
    void execute(TCPSocketPtr &) const override;
};

struct GetPort : public SocketAction {
    uint16_t _port;

    GetPort(uint16_t);
    std::string description() const override;
    void execute(TCPSocketPtr &) const override;
};

struct CheckBind : public SocketAction {
    CheckBind();
    std::string description() const override;
    void execute(TCPSocketPtr &) const override;
};

// -----------------------------------------------------------

class SocketTestHarness {
    std::string _test_name;
    TCPSocketPtr _sock;

  public:
    SocketTestHarness(const std::string &test_name, const std::string &ip, uint16_t port);

    void execute(const SocketTestStep &step);
};

#endif