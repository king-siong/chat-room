#include "./server_test_harness.hh"

// ServerTestStep

ServerTestStep::operator std::string() const { return "ServerTestStep"; }

void ServerTestStep::execute(Server &) const {}

ServerTestStep::~ServerTestStep() {}

// ServerViolation

ServerViolation::ServerViolation(const std::string &msg) : std::runtime_error(msg) {}

ServerViolation ServerViolation::test(const std::string &test_name,
                                                                        const std::string &expected,
                                                                        const std::string &actual) {
    return ServerViolation("Server testing: " + test_name + ". expected " +
                                          expected + " but read " + actual);
}

// ServerAction

ServerAction::operator std::string() const { return "Action: " + description(); }

std::string ServerAction::description() const { return "description missing"; }

void ServerAction::execute(Server &) const {}

ServerAction::~ServerAction() {}

// Bind

Bind::Bind() {}

std::string Bind::description() const { return "bind"; }

void Bind::execute(Server &server) const {
    if (server.bind() < 0) {
        throw ServerViolation("server: bind error");
    }
}

// Listen

Listen::Listen() {}

std::string Listen::description() const { return "listen"; }

void Listen::execute(Server &server) const {
    if (server.listen() < 0) {
        throw ServerViolation("server: listen error");
    }
}

// Accept

Accept::Accept(bool add) : _add(add) {}

std::string Accept::description() const { return "accept a client"; }

void Accept::execute(Server &server) const {
    auto client = server.accept();
    if (_add) {
        server.add_client(client);
    }
}

// AddClient

AddClient::AddClient(TCPSocketPtr client) : _client(client) {}

std::string AddClient::description() const { return "add a client"; }

void AddClient::execute(Server &server) const {
    server.add_client(_client);
}

// DelClient

DelClient::DelClient(TCPSocketPtr client) : _client(client) {}

std::string DelClient::description() const { return "delete a client"; }

void DelClient::execute(Server &server) const {
    server.del_client(_client);
}

// Count

Count::Count(TCPSocketPtr client, bool flag) : _client(client), _flag(flag) {}

std::string Count::description() const { return "count"; }

void Count::execute(Server &server) const {
    bool flag = server.count(_client);
    if (flag != _flag) {
        throw ServerViolation::test("count", std::to_string(_flag), std::to_string(flag));
    }
}

// Shutdown

Shutdown::Shutdown() {}

std::string Shutdown::description() const { return "shutdown"; }

void Shutdown::execute(Server &server) const {
    server.shutdown();
}

// SendAll

SendAll::SendAll(const std::string &msg) : _msg(msg) {}

std::string SendAll::description() const { return "send to all"; }

void SendAll::execute(Server &server) const {
    server.send_all(_msg);
}

// SendClients

SendClients::SendClients(const std::vector<TCPSocketPtr> &clients, const std::string &msg) 
    : 
    _clients(clients.begin(), clients.end()),
    _msg(msg) 
    {}

std::string SendClients::description() const { return "send to clients"; }

void SendClients::execute(Server &server) const {
    std::vector<TCPSocketPtr> clients = {_clients.begin(), _clients.end()};
    server.send(clients, _msg);
}

// TestHarness

ServerTestHarness::ServerTestHarness(const std::string &test_name, const std::string &ip, uint16_t port) : 
    _test_name(test_name), 
    _server(std::make_shared<TCPSocket>(ip, port))
    {}

void ServerTestHarness::execute(const ServerTestStep &step) {
    try {
        step.execute(_server);
    } catch (const ServerViolation &e) {
        std::cerr << "Test Failure on action:\n\t" << std::string(step);
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \"" + _test_name + "\" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << std::string(step);
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \"" + _test_name +
                                             "\" caused your implementation to throw an exception!");
    }
}