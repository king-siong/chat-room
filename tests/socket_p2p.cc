#include "./testlib.h"
#include "./socket_test_harness.hh"

int main(int argc, char *argv[]) {
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
            SocketTestHarness test("p2p-3-message", ip, port);

            std::vector<std::string> client_msgs;
            std::vector<std::string> server_msgs;

            size_t T = 3;
            for (size_t i = 0; i < T; i++) {
                client_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 36}"));
                server_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 36}"));
            }

            test.execute(P2P(client_msgs, server_msgs));
        }

        {
            SocketTestHarness test("p2p-16-message", ip, port);

            std::vector<std::string> client_msgs;
            std::vector<std::string> server_msgs;

            size_t T = 16;
            for (size_t i = 0; i < T; i++) {
                client_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 24}"));
                server_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 24}"));
            }

            test.execute(P2P(client_msgs, server_msgs));
        }

        {
            SocketTestHarness test("p2p-77-message", ip, port);

            std::vector<std::string> client_msgs;
            std::vector<std::string> server_msgs;

            size_t T = 77;
            for (size_t i = 0; i < T; i++) {
                client_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 69}"));
                server_msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 69}"));
            }

            test.execute(P2P(client_msgs, server_msgs));
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}