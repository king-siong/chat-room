#include "./codec_test_harness.hh"

#include <algorithm>

// CodecTestStep

CodecTestStep::operator std::string() const { return "CodecTestStep"; }

void CodecTestStep::execute(Codec &) const {}

CodecTestStep::~CodecTestStep() {}

// CodecViolation

CodecViolation::CodecViolation(const std::string &msg) : std::runtime_error(msg) {}

CodecViolation CodecViolation::test(const std::string &test_name,
                                                                        const std::string &expected,
                                                                        const std::string &actual) {
    return CodecViolation("Codec testing: " + test_name + ". expected " +
                                          expected + " but read " + actual);
}

// check BLE

bool chekc_ble(size_t ori_n, size_t now_n, bool flag) {
    constexpr size_t OVERFLOW_BYTES = 9;
    size_t mx = std::max(ori_n + OVERFLOW_BYTES, ori_n * 2 + 1);
    if (flag) mx = (ori_n * 3 + 1) / 2;
    if (now_n > mx) {
        return true;
    }
    return false;
}

// check validation

void check_validation(const std::vector<std::string> &msgs, const std::vector<std::string> &de_msgs) {
    if (de_msgs.size() != msgs.size()) {
        throw CodecViolation::test("decoded msg size", std::to_string(msgs.size()), std::to_string(de_msgs.size()));
    }
    for (size_t i = 0; i < msgs.size(); i++) {
        if (msgs[i] != de_msgs[i]) {
            throw CodecViolation::test("decoded msg", msgs[i], de_msgs[i]);
        }
    }
}

// CodecTest

CodecTest::operator std::string() const { return "Test: " + description(); }

std::string CodecTest::description() const { return "description missing"; }

void CodecTest::execute(Codec &) const {}

CodecTest::~CodecTest() {}

// NormalTest

NormalTest::NormalTest(const std::vector<std::string> &msgs, const std::vector<size_t> &partition, bool flag) 
    : 
    _flag(flag),
    _msgs(msgs), 
    _partition(partition)
    {}

std::string NormalTest::description() const { return "normal test"; }

void NormalTest::execute(Codec &codec) const {
    std::string en_msg = "";
    int cnt = 0;
    for (auto msg : _msgs) {
        auto tmp_msg = codec.encode(msg);
        if (chekc_ble(msg.size(), tmp_msg.size(), _flag)) {
            throw CodecViolation("encoded msg byte limit exceeded");
        }
        en_msg += tmp_msg;
    }
    size_t tot = en_msg.size(), d = 0, i = 0;
    for (auto it : _partition) d += it;

    Codec decoder;

    std::vector<std::string> de_msgs;
    for (auto it : _partition) {
        size_t l = it * tot / d;
        auto msgs = decoder.decode(en_msg.substr(i, l));
        for (auto msg : msgs) {
            de_msgs.push_back(msg);
        }
        i += l;
    }
    if (i < tot) {
        auto msgs = decoder.decode(en_msg.substr(i, tot - i));
        for (auto msg : msgs) {
            de_msgs.push_back(msg);
        }
    }

    check_validation(_msgs, de_msgs);
}

// RepeatTest

RepeatTest::RepeatTest(size_t t, const std::vector<std::string> &msgs, const std::vector<size_t> &partition, bool flag)
    :
    _flag(flag),
    _t(t), 
    _msgs(msgs), 
    _partition(partition)
    {}

std::string RepeatTest::description() const { return "repeat test"; }

void RepeatTest::execute(Codec &codec) const {
    auto msgs = _msgs;

    Codec decoder;

    for (size_t _ = 0; _ < _t; _++) {
        std::string en_msg = "";
        std::vector<std::string> tmp_msgs;
        for (auto msg : msgs) {
            auto tmp_msg = codec.encode(msg);
            if (chekc_ble(msg.size(), tmp_msg.size(), _flag)) {
                throw CodecViolation("encoded msg byte limit exceeded");
            }
            en_msg += tmp_msg;
            tmp_msgs.push_back(tmp_msg + "r");
        }
        size_t tot = en_msg.size(), d = 0, i = 0;
        for (auto it : _partition) d += it;

        std::vector<std::string> de_msgs;
        for (auto it : _partition) {
            size_t l = it * tot / d;
            auto msgs = decoder.decode(en_msg.substr(i, l));
            for (auto msg : msgs) {
                de_msgs.push_back(msg);
            }
            i += l;
        }
        if (i < tot) {
            auto msgs = decoder.decode(en_msg.substr(i, tot - i));
            for (auto msg : msgs) {
                de_msgs.push_back(msg);
            }
        }

        check_validation(msgs, de_msgs);

        msgs = std::move(tmp_msgs);
    }
}

// EdgeCutTest

EdgeCutTest::EdgeCutTest(const std::vector<std::string> &msgs, bool flag) 
    : 
    _flag(flag), 
    _msgs(msgs)
    {}

std::string EdgeCutTest::description() const { return "edge-cut test"; }

void EdgeCutTest::execute(Codec &codec) const {
    std::string en_msg = "";
    std::string pre_msg = "";
    std::vector<std::string> de_msgs;

    Codec decoder;

    for (auto msg : _msgs) {
        auto tmp_msg = codec.encode(msg);
        if (chekc_ble(msg.size(), tmp_msg.size(), _flag)) {
            throw CodecViolation("encoded msg byte limit exceeded");
        }
        size_t len = std::min(tmp_msg.length(), msg.length());
        auto msgs = decoder.decode(pre_msg + tmp_msg.substr(0, len));
        for (auto msg : msgs) {
            de_msgs.push_back(msg);
        }
        pre_msg = tmp_msg.substr(len, tmp_msg.length());
    }

    if (pre_msg.length()) {
        auto msgs = decoder.decode(pre_msg);
        for (auto msg : msgs) {
            de_msgs.push_back(msg);
        }
    }

    check_validation(_msgs, de_msgs);
}

// TestHarness

CodecTestHarness::CodecTestHarness(const std::string &test_name) : 
    _test_name(test_name)
    {}

void CodecTestHarness::execute(const CodecTestStep &step) {
    try {
        step.execute(_codec);
    } catch (const CodecViolation &e) {
        std::cerr << "Test Failure on test:\n\t" << std::string(step);
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw CodecViolation("The test \"" + _test_name + "\" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << std::string(step);
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw CodecViolation("The test \"" + _test_name +
                                             "\" caused your implementation to throw an exception!");
    }
}