//
// Created by parallels on 10/18/17.
//

#include "GTEpoll_thread_pool.h"


namespace GTUTIL{

    GTEpoll_thread_pool::GTEpoll_thread_pool(std::function<void> func) {
        thread_func_ = func;
        default_thread_num_ = std::thread::hardware_concurrency();
    }

    GTEpoll_thread_pool::GTEpoll_thread_pool(int thread_num, std::function<void> func) {
        thread_func_ = func;
        default_thread_num_ = thread_num;
    }

    GTEpoll_thread_pool::~GTEpoll_thread_pool()=default {

    }

    void GTEpoll_thread_pool::start() {
        for(int i=0; i < default_thread_num_; i++){
            thread_ t;
            t.th = std::move(std::thread(start_internal_, t.stop));
            thread_vec_.push_back(std::move(t));
        }
    }

    void GTEpoll_thread_pool::start_internal_(std::atomic<bool>& stop_thread) {
        while (!stop_thread){
            thread_func_();
        }
    }

    void GTEpoll_thread_pool::stop() {
        for (auto& iter: thread_vec_){
            iter.stop = true;
        }
        for (auto& iter: thread_vec_){
            iter.th.join();
        }
        return;
    }
}
