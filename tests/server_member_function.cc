#include "./testlib.h"
#include "./server_test_harness.hh"
#include "../utils/thread_pool.hpp"

#include <chrono>
#include <atomic>
#include <string>
#include <csignal>

using ClientPtr = std::shared_ptr<Client>;

auto now() { return std::chrono::steady_clock::now(); }

void execute_send_all(const std::string &ip, uint16_t port, size_t exclude) {
    ServerTestHarness test("send-all-" + std::to_string(exclude), ip, port);

    size_t n = 10, T = 3;

    std::vector<ClientPtr> clients;

    std::vector<std::string> send_msgs;
    for (size_t i = 0; i < T; i++) {
        send_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 16}"));
    }

    std::vector<std::string> recv_msgs(n);

    {
        test.execute(Bind());
        test.execute(Listen());

        std::thread server_th([&] {
            for (size_t i = 0; i < n; i++) {
                if (i + exclude < n) {
                    test.execute(Accept(true));
                } else {
                    test.execute(Accept(false));
                }
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
            for (size_t i = 0; i < T; i++) {
                test.execute(SendAll(send_msgs[i]));
            }
            test.execute(Shutdown());
        });

        ThreadPool thread_pool(n);

        for (size_t i = 0; i < n; i++) {
            thread_pool.enqueue([&, i] {
                for (size_t j = 0; j < T; j++) {
                    auto msg = clients[i]->recv();
                    recv_msgs[i] += msg;
                }
            });
        }

        send_th.join();
    }

    for (size_t i = 0; i < n; i++) {
        std::string ans = "";
        if (i + exclude < n) {
            for (size_t j = 0; j < T; j++) {
                ans += send_msgs[j];
            }
        }
        if (recv_msgs[i] != ans) {
            throw ServerViolation::test("recv msg", ans, recv_msgs[i]);
        }
    }
}

void execute_send_clients(const std::string &ip, uint16_t port, size_t exclude) {
    try {
        TCPSocketPtr sock = std::make_shared<TCPSocket>(ip, port);
        Server server(sock);

        size_t n = 10, T = 3;

        std::vector<ClientPtr> clients;
        std::vector<TCPSocketPtr> client_socks;

        std::vector<std::string> send_msgs;
        for (size_t i = 0; i < T; i++) {
            send_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 16}"));
        }

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
                    if (i + exclude < n) {
                        server.add_client(client_sock);
                    }
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
                std::vector<TCPSocketPtr> v = {client_socks.begin() + T, client_socks.end()};
                server.send(v, "end");
                for (size_t i = 0; i < T; i++) {
                    std::vector<TCPSocketPtr> v = {client_socks.begin(), client_socks.begin() + T};
                    server.send(v, send_msgs[i]);
                    server.send(client_socks[T - i - 1], "end");
                    server.del_client(client_socks[T - i - 1]);
                }
                server.shutdown();
                for (size_t i = 0; i < n; i++) {
                    client_socks[i]->socket_shutdown();
                }
            });

            ThreadPool thread_pool(n);

            for (size_t i = 0; i < n; i++) {
                thread_pool.enqueue([&, i] {
                    for (size_t j = 0; j < T + 1; j++) {
                        auto msg = clients[i]->recv();
                        recv_msgs[i] += msg;
                    }
                });
            }

            send_th.join();
        }

        for (size_t i = 0; i < n; i++) {
            std::string ans = "";
            if (i + exclude < n) {
                for (size_t j = 0; j + i < T; j++) {
                    ans += send_msgs[j];
                }
                ans += "end";
            }
            if (recv_msgs[i] != ans) {
                throw ServerViolation::test("recv msg", ans, recv_msgs[i]);
            }
        }

    } catch (const ServerViolation &e) {
        std::cerr << "Test Failure on test:\n\t" << "send-clients";
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \" send-clients \" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << "send-clients";
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw ServerViolation("The test \"send-clients \" caused your implementation to throw an exception!");
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
            ServerTestHarness test("add-del-10-50", ip, port);
            
            size_t n = 10, T = 50;

            std::vector<TCPSocketPtr> clients;

            for (size_t i = 0; i < n; i++) {
                clients.push_back(std::make_shared<TCPSocket>(ip, port));

                test.execute(Count(clients[i], false));
                test.execute(AddClient(clients[i]));
                test.execute(Count(clients[i], true));
            }

            const auto start = now();
            const auto time_step = std::chrono::milliseconds(20);


            for (size_t i = 0; i < T; i++) {
                std::atomic<bool> ready(false);
                std::thread add_th1([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(AddClient(clients[j]));
                });
                std::thread add_th2([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(AddClient(clients[j]));
                });
                std::thread del_th1([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(DelClient(clients[j]));
                });
                std::thread del_th2([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(DelClient(clients[j]));
                });
                std::thread send_all_th1([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(SendAll("send_all"));
                });
                std::thread send_all_th2([&] {
                    size_t j = rnd.next(n);
                    while (!ready.load());
                    test.execute(SendAll("send_all"));
                });

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                ready.store(true);

                add_th1.join(); add_th2.join(); 
                del_th1.join(); del_th2.join();
                send_all_th1.join(); send_all_th2.join();
            }

            for (size_t i = 0; i < T * 2; i++) {
                if (rnd.next(2)) {
                    size_t j = rnd.next(n);
                    test.execute(AddClient(clients[j]));
                    test.execute(Count(clients[j], true));
                } else {
                    size_t j = rnd.next(n);
                    test.execute(DelClient(clients[j]));
                    test.execute(Count(clients[j], false));
                }
            }

        }

        {            
            execute_send_all(ip, port, 0);
            execute_send_all(ip, port, 3);
        }

        {
            execute_send_clients(ip, port, 0);
            execute_send_clients(ip, port, 1);
            execute_send_clients(ip, port, 3);
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}