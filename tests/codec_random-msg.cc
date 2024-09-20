#include "./testlib.h"
#include "./codec_test_harness.hh"

#include <utility>
#include <tuple>

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    try {

        {
            CodecTestHarness test("random-normal");

            auto gen_msgs = [&] (size_t n, size_t m, size_t l) 
            -> std::pair<std::vector<std::string>, std::vector<size_t>> {
                std::vector<std::string> msgs(n, "");
                size_t tot = 0;
                for (size_t i = 0; i < n; i++) {
                    size_t len = rnd.next(1, static_cast<int>(l));
                    for (size_t j = 0; j < len; j++) {
                        msgs[i].push_back(static_cast<char>(rnd.next(-128, 127)));
                    }
                    tot += len;
                }
                auto partition = rnd.partition(m, tot);
                return std::make_pair(msgs, partition);
            };

            std::vector<std::tuple<size_t, size_t, size_t>> v = {
                {1, 10, 1000000},
                {10, 13, 100000}, 
                {100, 77, 10000}, 
                {1000, 353, 1000}, 
                };

            for (auto [n, m, l] : v) {
                auto [msgs, partition] = gen_msgs(n, m, l);
                test.execute(NormalTest(msgs, partition));
            }
        }

        {
            CodecTestHarness test("random-edge-cut");

            auto gen_msgs = [&] (size_t n, size_t l) {
                std::vector<std::string> msgs(n, "");
                size_t tot = 0;
                for (size_t i = 0; i < n; i++) {
                    size_t len = rnd.next(1, static_cast<int>(l));
                    for (size_t j = 0; j < len; j++) {
                        msgs[i].push_back(static_cast<char>(rnd.next(-128, 127)));
                    }
                    tot += len;
                }
                return msgs;
            };

            std::vector<std::tuple<size_t, size_t>> v = {
                {10, 13}, 
                {100, 99}, 
                {1000, 998},
                {1, 1000000}
                };

            for (auto [n, l] : v) {
                auto msgs = gen_msgs(n, l);
                test.execute(EdgeCutTest(msgs));
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}