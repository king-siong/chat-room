#include "./testlib.h"
#include "./msg_queue_test_harness.hh"

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    try {

        {
            MSGQueueTestHarness test("single-thread-1");

            test.execute(Push("hello"));
            test.execute(Push("nice to meet you"));
            test.execute(Pop("hello"));
            test.execute(Pop("nice to meet you"));
        }

        {
            MSGQueueTestHarness test("single-thread-2");

            test.execute(Push("hello"));
            test.execute(Pop("hello"));
            test.execute(Push("nice to meet you"));
            test.execute(Pop("nice to meet you"));
        }

        {
            MSGQueueTestHarness test("single-thread-3");
            size_t T = 1000;
            std::vector<std::string> msgs{};

            for (size_t i = 0; i < T; i++) {
                msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 1000}"));
                test.execute(Push(msgs[i]));
            }

            for (size_t i = 0; i < T; i++) {
                test.execute(Pop(msgs[i]));
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}