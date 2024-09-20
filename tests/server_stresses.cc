#include "./testlib.h"
#include "./server_test_harness.hh"
#include "../utils/thread_pool.hpp"

#include <chrono>
#include <csignal>

auto now() { return std::chrono::steady_clock::now(); }

using ClientPtr = std::shared_ptr<Client>;

void execute(const std::string &ip, uint16_t port, size_t n) {
    try {
        TCPSocketPtr sock = std::make_shared<TCPSocket>(ip, port);
        Server server(sock);

        std::vector<ClientPtr> clients;
        std::vector<TCPSocketPtr> client_socks;

        std::vector<std::string> send_msgs;
        for (size_t i = 0; i < n; i++) {
            send_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 4}"));
        }

        std::vector<int> perm(n);
        for (int i = 0; i < n; i++) perm[i] = i;
        shuffle(perm.begin(), perm.end());

        std::vector<std::string> recv_msgs(n);

        {
            if (server.bind() < 0) {
                throw ServerViolation("server: bind error");
            }

            if (server.listen() < 0) {
                throw ServerViolation("server: listen error");
            }

            std::thread server_th([&] {
                for (size_t i = 0; i < n; i++) {
                    auto client_sock = server.accept();
                    server.add_client(client_sock);
                    client_socks.push_back(client_sock);
                }
            });

            std::thread client_th([&] {
                for (size_t i = 0; i < n; i++) {
                    clients.push_back(std::make_shared<Client>(ip, port));
                    if (clients[i]->connect() < 0) {
                        throw ServerViolation("client: connect error");
                    }
                }
            });

            server_th.join();
            client_th.join();

            std::thread send_th([&] {
                for (size_t i = 0; i < n; i++) {
                    server.send(client_socks, send_msgs[i]);
                    server.send(client_socks[perm[n - i - 1]], "end");
                    server.del_client(client_socks[perm[n - i - 1]]);
                }
                server.shutdown();
            });

            ThreadPool thread_pool(n);     

            for (size_t i = 0; i < n; i++) {
                thread_pool.enqueue([&, i] {
                    for (size_t j = 0; j + i < n; j++) {
                        auto msg = clients[perm[i]]->recv();
                        recv_msgs[i] += msg;
                    }
                    recv_msgs[i] += clients[perm[i]]->recv();
                });
            }

            send_th.join();
        }

        for (size_t i = 0; i < n; i++) {
            std::string ans = "";
            for (size_t j = 0; j + i < n; j++) {
                ans += send_msgs[j];
            }
            ans += "end";
            if (recv_msgs[i] != ans) {
                throw ServerViolation::test("recv msg", ans, recv_msgs[i]);
            }
        }

    } catch (const ServerViolation &e) {
        std::cerr << "Test Failure on test:\n\t" << "stress-test";
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \" stress-test " + std::to_string(n) + "\" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << "stress-test";
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \"stress-test " + std::to_string(n) + "\" caused your implementation to throw an exception!");
    }
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    registerGen(argc, argv, 1);
    uint16_t port = 50000;
    const std::string ip = "127.0.0.1";

    {
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());

        int reuse = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        int ret = -1;
        while (ret < 0 && port < 65535) {
            
            port++;
            addr.sin_port = htons(port);

            ret = bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        }

        if (ret < 0) {
            std::cerr << "no free port found" << std::endl;
            return EXIT_FAILURE;
        }

        close(fd);

    }

    try {

        {
            execute(ip, port, 3);
            execute(ip, port, 6);
            execute(ip, port, 9);
            execute(ip, port, 16);
            execute(ip, port, 33);
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}