#ifndef _MSG_QUEUE_HH_
#define _MSG_QUEUE_HH_

#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>

class MSGQueue {
  private:
    std::queue<std::string> _msg_queue;
    std::mutex _mtx;
    std::condition_variable _condition;

  public:
    MSGQueue();
    ~MSGQueue();

    void push_msg(const std::string &msg);
    std::string pop_msg();
};

#endif