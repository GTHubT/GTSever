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
        std::thread th;
        std::thread::id tid;
        std::atomic<bool> stop;

        thread_():stop(false){
        }

        thread_(thread_&& t)noexcept:stop(t.stop.load()){
            th = std::move(t.th);
            tid = t.tid;
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
