#ifndef _SERVER_TEST_HARNESS_HH_
#define _SERVER_TEST_HARNESS_HH_

#include "../utils/tcp_socket.hh"
#include "../server/server.hh"
#include "../client/client.hh"

#include <thread>
#include <exception>
#include <mutex>
#include <cassert>

struct ServerTestStep {
    virtual operator std::string() const;
    virtual void execute(Server &) const;
    virtual ~ServerTestStep();
};

class ServerViolation : public std::runtime_error {
  public:
    ServerViolation(const std::string &msg);

    static ServerViolation test(const std::string &test_name,
                                                   const std::string &expected,
                                                   const std::string &actual);
};

struct ServerAction : public ServerTestStep {
    operator std::string() const override;
    virtual std::string description() const;
    virtual void execute(Server &) const override;
    virtual ~ServerAction() override;
};

// -----------------------------------------------------------

struct Bind : public ServerAction {
    Bind();
    std::string description() const override;
    void execute(Server &) const override;
};

struct Listen : public ServerAction {
    Listen();
    std::string description() const override;
    void execute(Server &) const override;
};

struct Accept : public ServerAction {
    bool _add;

    Accept(bool);
    std::string description() const override;
    void execute(Server &) const override;
};

struct AddClient : public ServerAction {
    TCPSocketPtr _client;

    AddClient(TCPSocketPtr);
    std::string description() const override;
    void execute(Server &) const override;
};

struct DelClient : public ServerAction {
    TCPSocketPtr _client;

    DelClient(TCPSocketPtr);
    std::string description() const override;
    void execute(Server &) const override;
};

struct Count : public ServerAction {
    TCPSocketPtr _client;
    bool _flag;

    Count(TCPSocketPtr, bool);
    std::string description() const override;
    void execute(Server &) const override;
};

struct Shutdown : public ServerAction {

    Shutdown();
    std::string description() const override;
    void execute(Server &) const override;
};

struct SendClients : public ServerAction {
    std::vector<TCPSocketPtr> _clients;
    std::string _msg;

    SendClients(const std::vector<TCPSocketPtr> &, const std::string &);
    std::string description() const override;
    void execute(Server &) const override;
};

struct SendAll : public ServerAction {
    std::string _msg;

    SendAll(const std::string &);
    std::string description() const override;
    void execute(Server &) const override;
};

// -----------------------------------------------------------

class ServerTestHarness {
    std::string _test_name;
    Server _server;

  public:
    ServerTestHarness(const std::string &test_name, const std::string &ip, uint16_t port);

    void execute(const ServerTestStep &step);
};

#endif