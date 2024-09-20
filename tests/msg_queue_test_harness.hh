#ifndef _MSG_QUEUE_TEST_HARNESS_HH_
#define _MSG_QUEUE_TEST_HARNESS_HH_

#include "../utils/msg_queue.hh"

#include <iostream>
#include <exception>
#include <mutex>

struct MSGQueueTestStep {
    virtual operator std::string() const;
    virtual void execute(MSGQueue &) const;
    virtual ~MSGQueueTestStep();
};

class MSGQueueViolation : public std::runtime_error {
  public:
    MSGQueueViolation(const std::string &msg);

    static MSGQueueViolation test(const std::string &test_name,
                                                   const std::string &expected,
                                                   const std::string &actual);
};

struct MSGQueueAction : public MSGQueueTestStep {
    operator std::string() const override;
    virtual std::string description() const;
    virtual void execute(MSGQueue &) const override;
    virtual ~MSGQueueAction() override;
};

// -----------------------------------------------------------

struct Push : public MSGQueueAction {
    std::string _msg;

    Push(const std::string &msg);
    std::string description() const override;
    void execute(MSGQueue &) const override;
};

struct Pop : public MSGQueueAction {
    std::string _msg;

    Pop(const std::string &msg);
    std::string description() const override;
    void execute(MSGQueue &) const override;
};

struct ParallelPushPop : public MSGQueueAction {
    size_t _cT, _pT;
    std::vector<size_t> _c_list;
    std::vector<std::vector<std::string>> _msgs;

    ParallelPushPop(size_t cT, 
                    size_t pT, 
                    const std::vector<size_t> &, 
                    const std::vector<std::vector<std::string>> &);
    std::string description() const override;
    void execute(MSGQueue &) const override;
};

// -----------------------------------------------------------

class MSGQueueTestHarness {
    std::string _test_name;
    MSGQueue _msg_queue;

  public:
    MSGQueueTestHarness(const std::string &test_name);

    void execute(const MSGQueueTestStep &step);
};

#endif