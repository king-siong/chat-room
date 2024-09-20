#include "./testlib.h"
#include "./msg_queue_test_harness.hh"

#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

auto now() { return std::chrono::steady_clock::now(); }

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    try {

        {
            MSGQueueTestHarness test("1-producer-1-consumer");

            std::vector<std::string> msgs;
            size_t T = 10;
            for (size_t i = 0; i < T; i++) {
                msgs.emplace_back(rnd.next("[a-zA-Z0-9]{1, 33}"));
            }

            const auto start = now();
            const auto time_step = std::chrono::milliseconds(15);

            for (size_t i = 0; i < T; i++) {
                std::atomic<bool> ready(false);
                std::thread pop_th([&, i] {
                    while (!ready.load());
                    test.execute(Pop(msgs[i]));
                });
                std::thread push_th([&, i] {
                    while (!ready.load());
                    test.execute(Push(msgs[i]));
                });

                std::this_thread::sleep_for(time_step);
                ready.store(true);

                push_th.join();
                pop_th.join();
            }

        }

        {
            MSGQueueTestHarness test("9-producer-9-consumer");

            std::vector<std::vector<std::string>> msgs;

            size_t pT = 9, cT = 9;
            size_t tot = 0;
            for (size_t i = 0; i < pT; i++) {
                msgs.push_back({});
                size_t siz = rnd.next(1, 25);
                tot += siz;
                for (size_t j = 0; j < siz; j++) {
                    msgs[i].emplace_back(rnd.next("[a-zA-Z0-9]{1, 24}"));
                }
            }

            auto c_list = rnd.partition(cT, tot);

            test.execute(ParallelPushPop(cT, pT, c_list, msgs));
        }

        {
            MSGQueueTestHarness test("96-producer-33-consumer");

            std::vector<std::vector<std::string>> msgs;

            size_t pT = 96, cT = 33;
            size_t tot = 0;
            for (size_t i = 0; i < pT; i++) {
                msgs.push_back({});
                size_t siz = rnd.next(1, 6);
                tot += siz;
                for (size_t j = 0; j < siz; j++) {
                    msgs[i].emplace_back(rnd.next("[a-zA-Z0-9]{1, 10}"));
                }
            }

            auto c_list = rnd.partition(cT, tot);

            test.execute(ParallelPushPop(cT, pT, c_list, msgs));
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}