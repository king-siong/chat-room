#include "./testlib.h"
#include "./codec_test_harness.hh"

#include <utility>
#include <tuple>

int main(int argc, char *argv[]) {
    registerGen(argc, argv, 1);
    try {

        {
            CodecTestHarness test("all-the-same");

            auto gen_msgs = [&] (size_t n, size_t l, char ch) 
            -> std::pair<std::vector<std::string>, std::vector<size_t>> {
                std::vector<std::string> msgs(n, "");
                size_t tot = 0;
                for (size_t i = 0; i < n; i++) {
                    size_t len = rnd.next(1, static_cast<int>(l));
                    for (size_t j = 0; j < len; j++) {
                        msgs[i].push_back(ch);
                    }
                    tot += len;
                }
                auto partition = rnd.partition(rnd.next(1, static_cast<int>(tot)), tot);
                return std::make_pair(msgs, partition);
            };

            for (int ch = -128; ch < 128; ch++) {
                auto [msgs, partition] = gen_msgs(100, 56, static_cast<char>(ch));
                test.execute(NormalTest(msgs, partition));
                test.execute(EdgeCutTest(msgs));
            }
        }

        {
            CodecTestHarness test("repeat-decoded-msg");

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
                {10, 13, 10}, 
                {100, 111, 100}, 
                {1000, 353, 1000}
                };

            for (auto [n, m, l] : v) {
                auto [msgs, partition] = gen_msgs(n, m, l);
                test.execute(RepeatTest(3, msgs, partition));
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}