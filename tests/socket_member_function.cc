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
            SocketTestHarness test("ip-127.0.0.1", "127.0.0.1", port);
            
            test.execute(GetIP("127.0.0.1"));
        }

        {
            SocketTestHarness test("ip-0.0.0.0", "0.0.0.0", port);
            
            test.execute(GetIP("0.0.0.0"));
        }

        {
            SocketTestHarness test("port-auto", ip, port);

            test.execute(GetPort(port));
        }

        {
            SocketTestHarness test("port-56001", ip, 56001);

            test.execute(GetPort(56001));
        }

        {
            SocketTestHarness test("bind-1st", ip, port);

            test.execute(CheckBind());
        }

        {
            SocketTestHarness test("bind-2nd", ip, port);

            test.execute(CheckBind());
        }

        {
            SocketTestHarness test("bind-3rd", ip, port);

            test.execute(CheckBind());
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}