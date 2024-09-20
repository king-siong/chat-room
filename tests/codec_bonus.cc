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
                test.execute(NormalTest(msgs, partition, true));
            }
        }

        {
            CodecTestHarness test("all-the-same");

            auto gen_msgs = [&] (size_t n, size_t l, char ch) 
            -> std::pair<std::vector<std::string>, std::vector<size_t>> {
                std::vector<std::string> msgs(n, "");
                size_t tot = 0;
                for (size_t i = 0; i < n; i++) {
                    size_t len = rnd.next(5, static_cast<int>(l));
                    for (size_t j = 0; j < len; j++) {
                        msgs[i].push_back(ch);
                    }
                    tot += len;
                }
                auto partition = rnd.partition(rnd.next(1, static_cast<int>(tot)), tot);
                return std::make_pair(msgs, partition);
            };

            for (int ch = -128; ch < 128; ch++) {
                if (!rnd.next(32)) {
                    auto [msgs, partition] = gen_msgs(2, 100000, static_cast<char>(ch));
                    test.execute(NormalTest(msgs, partition, true));
                } else {
                    auto [msgs, partition] = gen_msgs(2, 10, static_cast<char>(ch));
                    test.execute(NormalTest(msgs, partition, true));
                }
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

            std::vector<std::tuple<size_t, size_t, size_t>> v = {};

            for (size_t n = 1, i = 1; n <= 100000; n *= 10, i++) {
                size_t l = 100000 / n;
                size_t m = rnd.next(3, 50000);
                v.push_back({n, m, l});
            }

            for (auto [n, m, l] : v) {
                auto [msgs, partition] = gen_msgs(n, m, l);
                test.execute(RepeatTest(3, msgs, partition, true));
            }

        }

        {
            CodecTestHarness test("length-1");

            for (int ch = -128; ch < 128; ch++) {
                std::vector<std::string> msgs = {""}; 
                msgs[0].push_back(static_cast<char>(ch));
                std::vector<size_t> partition = {1};
                test.execute(NormalTest(msgs, partition, true));
            }

        }

        {
            CodecTestHarness test("length-2");

            for (int ch = -128; ch < 128; ch++) {
                std::vector<std::string> msgs = {""}; 
                msgs[0].push_back(static_cast<char>(ch)); 
                msgs[0].push_back(static_cast<char>(ch));
                std::vector<size_t> partition = {2};
                test.execute(NormalTest(msgs, partition, true));
            }

        }

        {
            CodecTestHarness test("length-3");

            for (int ch = -128; ch < 128; ch++) {
                std::vector<std::string> msgs = {""}; 
                msgs[0].push_back(static_cast<char>(ch));
                msgs[0].push_back(static_cast<char>(ch));
                msgs[0].push_back(static_cast<char>(ch));
                std::vector<size_t> partition = {3};
                test.execute(NormalTest(msgs, partition, true));
            }

        }

        {
            CodecTestHarness test("length-<5-random");

            size_t T = 1000;

            for (size_t t = 0; t < T; t++) {
                size_t len = rnd.next(1, 4);
                std::vector<std::string> msgs = {""};
                for (size_t i = 0; i < len; i++) {
                    msgs[0].push_back(static_cast<char>(rnd.next(-128, 127)));
                }
                std::vector<size_t> partition = {len};
                test.execute(NormalTest(msgs, partition, true));
            }

        }

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}