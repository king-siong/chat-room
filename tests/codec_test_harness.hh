#ifndef _CODEC_TEST_HARNESS_HH_
#define _CODEC_TEST_HARNESS_HH_

#include "../utils/codec.hh"

#include <exception>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>

struct CodecTestStep {
    virtual operator std::string() const;
    virtual void execute(Codec &) const;
    virtual ~CodecTestStep();
};

class CodecViolation : public std::runtime_error {
  public:
    CodecViolation(const std::string &msg);

    static CodecViolation test(const std::string &test_name,
                                                   const std::string &expected,
                                                   const std::string &actual);
};

struct CodecTest : public CodecTestStep {
    operator std::string() const override;
    virtual std::string description() const;
    virtual void execute(Codec &) const override;
    virtual ~CodecTest() override;
};

// -----------------------------------------------------------

struct NormalTest : public CodecTest {
    bool _flag;
    std::vector<std::string> _msgs;
    std::vector<size_t> _partition;

    NormalTest(const std::vector<std::string> &, const std::vector<size_t> &, bool flag = false);
    std::string description() const override;
    void execute(Codec &) const override;
};

struct RepeatTest : public CodecTest {
    bool _flag;
    size_t _t;
    std::vector<std::string> _msgs;
    std::vector<size_t> _partition;

    RepeatTest(size_t, const std::vector<std::string> &, const std::vector<size_t> &, bool flag = false);
    std::string description() const override;
    void execute(Codec &) const override;
};

struct EdgeCutTest : public CodecTest {
    bool _flag;
    std::vector<std::string> _msgs;

    EdgeCutTest(const std::vector<std::string> &, bool flag = false);
    std::string description() const override;
    void execute(Codec &) const override;
};

// -----------------------------------------------------------

class CodecTestHarness {
    std::string _test_name;
    Codec _codec;

  public:
    CodecTestHarness(const std::string &test_name);

    void execute(const CodecTestStep &step);
};

#endif