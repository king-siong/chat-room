#include "./msg_queue_test_harness.hh"
#include "../utils/thread_pool.hpp"

// MSGQueueTestStep

MSGQueueTestStep::operator std::string() const { return "MSGQueueTestStep"; }

void MSGQueueTestStep::execute(MSGQueue &) const {}

MSGQueueTestStep::~MSGQueueTestStep() {}

// MSGQueueViolation

MSGQueueViolation::MSGQueueViolation(const std::string &msg) : std::runtime_error(msg) {}

MSGQueueViolation MSGQueueViolation::test(const std::string &test_name,
                                                                        const std::string &expected,
                                                                        const std::string &actual) {
    return MSGQueueViolation("MSGQueue testing: " + test_name + ". expected " +
                                          expected + " but read " + actual);
}

// MSGQueueAction

MSGQueueAction::operator std::string() const { return "Action: " + description(); }

std::string MSGQueueAction::description() const { return "description missing"; }

void MSGQueueAction::execute(MSGQueue &) const {}

MSGQueueAction::~MSGQueueAction() {}

// Push

Push::Push(const std::string &msg) : _msg(msg) {}

std::string Push::description() const { return "push a message"; }

void Push::execute(MSGQueue &msg_queue) const { msg_queue.push_msg(_msg); }

// Pop

Pop::Pop(const std::string &msg) : _msg(msg) {}

std::string Pop::description() const { return "pop a message"; }

void Pop::execute(MSGQueue &msg_queue) const {
    auto msg = msg_queue.pop_msg();
    if (msg != _msg) {
        throw MSGQueueViolation::test("pop_msg", _msg, msg);
    }
}

// ParallelPushPop

ParallelPushPop::ParallelPushPop(size_t cT, 
                                 size_t pT, 
                                 const std::vector<size_t> &c_list, 
                                 const std::vector<std::vector<std::string>> &msgs) : 
    _cT(cT), _pT(pT), _c_list(c_list.begin(), c_list.end()), _msgs(msgs.begin(), msgs.end()) {}

std::string ParallelPushPop::description() const { return "parallel push & pop"; }

void ParallelPushPop::execute(MSGQueue &msg_queue) const {
    std::vector<std::string> p_msgs;
    std::mutex p_mtx;

    std::vector<std::string> c_msgs;
    std::mutex c_mtx;

    size_t tot = 0;
    for (auto msg : _msgs) {
        tot += msg.size();
    }

    {
        ThreadPool c_thread_pool(_cT);
        ThreadPool p_thread_pool(_pT);

        for (size_t i = 0; i < _cT; i++) {
            c_thread_pool.enqueue([&, i] {
                size_t _ = _c_list[i];
                while (_--) {
                    std::unique_lock<std::mutex> lock(c_mtx);
                    auto msg = msg_queue.pop_msg();
                    c_msgs.emplace_back(msg);
                }
            });
        }

        for (size_t i = 0; i < _pT; i++) {
            p_thread_pool.enqueue([&, i] {
                for (auto msg : _msgs[i]) {
                    std::unique_lock<std::mutex> lock(p_mtx);
                    msg_queue.push_msg(msg);
                    p_msgs.emplace_back(msg);
                }
            });
        }
    }

    if (p_msgs.size() != tot) {
        throw MSGQueueViolation::test("pushed message size", std::to_string(tot), std::to_string(p_msgs.size()));
    }

    if (c_msgs.size() != tot) {
        throw MSGQueueViolation::test("popped message size", std::to_string(tot), std::to_string(c_msgs.size()));
    }

    for (size_t i = 0; i < tot; i++) {
        if (c_msgs[i] != p_msgs[i]) {
            throw MSGQueueViolation::test("popped message error", p_msgs[i], c_msgs[i]);
        }
    }
}

// TestHarness

MSGQueueTestHarness::MSGQueueTestHarness(const std::string &test_name) : 
    _test_name(test_name), _msg_queue(MSGQueue()) {}

void MSGQueueTestHarness::execute(const MSGQueueTestStep &step) {
    try {
        step.execute(_msg_queue);
    } catch (const MSGQueueViolation &e) {
        std::cerr << "Test Failure on action:\n\t" << std::string(step);
        std::cerr << "\n\nFailure message:\n\t" << e.what() << std::endl;
        throw MSGQueueViolation("The test \"" + _test_name + "\" failed");
    } catch (const std::exception &e) {
        std::cerr << "Test Failure on exception:\n\t" << std::string(step);
        std::cerr << "\n\nException:\n\t" << e.what() << std::endl;
        throw MSGQueueViolation("The test \"" + _test_name +
                                             "\" caused your implementation to throw an exception!");
    }
}