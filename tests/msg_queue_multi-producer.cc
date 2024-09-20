#include "./testlib.h"
#include "./msg_queue_test_harness.hh"

#include <thread>
#include <mutex>

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    try {

        {
            MSGQueueTestHarness test("sequential-producer");

            std::vector<std::string> msgs;
            size_t T = 100;
            for (size_t i = 0; i < T; i++) {
                msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 33}"));
            }

            std::thread pop_th([&] {
                for (auto msg : msgs) {
                    test.execute(Pop(msg));
                }
            });

            std::thread push_th([&] {
                for (auto msg : msgs) {
                    test.execute(Push(msg));
                }
            });

            push_th.join();
            pop_th.join();
        }

        {
            MSGQueueTestHarness test("parallel-9-consumer");

            std::vector<std::vector<std::string>> msgs;
            size_t T = 9;
            for (size_t i = 0; i < T; i++) {
                msgs.push_back({rnd.next("[a-zA-Z0-9]{1, 24}")});
            }

            test.execute(ParallelPushPop(1, T, {T}, msgs));
        }

        {
            MSGQueueTestHarness test("parallel-16-consumer");

            std::vector<std::vector<std::string>> msgs;
            size_t T = 16, tot = 0;
            for (size_t i = 0; i < T; i++) {
                msgs.push_back({});
                size_t siz = rnd.next(1, 20);
                tot += siz;
                for (size_t j = 0; j < siz; j++) {
                    msgs[i].emplace_back(rnd.next("[a-zA-Z0-9]{1, 24}"));
                }
            }

            test.execute(ParallelPushPop(1, T, {tot}, msgs));
        }

        {
            MSGQueueTestHarness test("parallel-100-consumer");

            std::vector<std::vector<std::string>> msgs;
            size_t T = 100, tot = 0;
            for (size_t i = 0; i < T; i++) {
                msgs.push_back({});
                size_t siz = rnd.next(1, 7);
                tot += siz;
                for (size_t j = 0; j < siz; j++) {
                    msgs[i].emplace_back(rnd.next("[a-zA-Z0-9]{1, 24}"));
                }
            }

            test.execute(ParallelPushPop(1, T, {tot}, msgs));
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}