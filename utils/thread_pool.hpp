// thread pool
// 
// modified from https://github.com/progschj/ThreadPool/tree/master 

#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
  private:
    std::vector<std::thread> _workers;
    std::queue<std::function<void()>> _tasks; // task queue
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _stop;

  public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template<class F, class... Args>
    decltype(auto) enqueue(F&& f, Args&&... args);  // add a new task to the task queue
};

ThreadPool::ThreadPool(size_t threads) : _stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        _workers.emplace_back(
            [this] {
                for(;;) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->_mutex);
                        // wait until this thread is notified and task queue is not empty
                        this->_condition.wait(lock,
                            [this] { return this->_stop || !this->_tasks.empty(); });
                        if(this->_stop && this->_tasks.empty())
                            return;
                        task = std::move(this->_tasks.front());
                        this->_tasks.pop();
                    }

                    task();
                }
            }
        );
    }
}

template<class F, class... Args>
decltype(auto) ThreadPool::enqueue(F&& f, Args&&... args) {
    // deduce return type
    using return_type = decltype(std::forward<F>(f)(std::forward<Args>(args)...));

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(_mutex);

        if(_stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        _tasks.emplace([task] { (*task)(); });
    }
    
    // notify a block thread
    _condition.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {

    {
        std::unique_lock<std::mutex> lock(_mutex);
        _stop = true;
    }

    // notify all of the threads to shutdown
    _condition.notify_all();

    for(std::thread &worker: _workers) {
        // block util every thread finishes its execution
        worker.join();
    }
}

#endif