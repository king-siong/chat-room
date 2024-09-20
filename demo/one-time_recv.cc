#include "../utils/common.hh"
#include "../utils/tcp_socket.hh"
#include "../utils/thread_pool.hpp"
#include "../server/server.hh"
#include "../client/client.hh"

#include <string>
#include <vector>
#include <mutex>
#include <exception>

constexpr int DEFAULT_PORT = 5657;
std::string DEFAULT_IP = "127.0.0.1";

int main() {
    TCPSocketPtr server_sock = std::make_shared<TCPSocket>(DEFAULT_IP, DEFAULT_PORT);
    Server server(server_sock);
    Client client(DEFAULT_IP, DEFAULT_PORT);

    std::vector<std::string> msgs = {"hello!", "how are you?", "this is a test."};

    std::mutex mtx;

    try {
        std::thread server_th([&] {
            if (server.bind() < 0) {
                throw "server: bind error";
            }
            if (server.listen() < 0) {
                throw "server: listen error";
            }

            TCPSocketPtr client_sock = server.accept();

            server.add_client(client_sock);

            for (const auto &msg : msgs) {
                server.send(client_sock, msg);
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "server sent a msg: " << msg << std::endl;
            }
        });

        std::thread client_th([&] {
            if (client.connect() < 0) {
                throw "client: connect error";
            }
        });

        server_th.join();
        client_th.join();

        std::thread recv_th([&] {
            auto msg = client.recv();
            std::lock_guard<std::mutex> lock(mtx);
            std::cout << "client received a msg: " << msg << std::endl;
        });

        recv_th.join();

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}