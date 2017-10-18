//
// Created by parallels on 10/18/17.
//

#ifndef GTEPOLL_THREAD_POOL_H
#define GTEPOLL_THREAD_POOL_H

#include <functional>
#include <thread>
#include <atomic>
#include <vector>


namespace GTUTIL {

    struct thread_{
        std::thread t_;
        std::thread::id tid_;
        std::atomic<bool> stop_;
        std::thread::id get_tid(){return tid_;}

        explicit thread_(std::thread& t){
            t_ = std::move(t);
            tid_ = t_.get_id();
            stop_ = false;
        };

        explicit thread_(thread_&& t){
            t_ = std::move(t.t_);
            tid_ = t.tid_;
            stop_ = t.stop_.load();
        }
        ~thread_()= default{};
    };

    class GTEpoll_thread_pool {
    public:
        explicit GTEpoll_thread_pool(std::function<void>);
        GTEpoll_thread_pool(int thread_num, std::function<void>);
        ~GTEpoll_thread_pool()= default;

        void start();
        void stop();

    private:
        void start_internal_(std::atomic<bool>&);

    private:
        int  default_thread_num_;
        std::function<void> thread_func_;
        std::vector<thread_> thread_vec_;
    };
}


#endif //GTEPOLL_GTEPOLL_THREAD_POOL_H
