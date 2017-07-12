#include "GT_ThreadPool.h"

#include <chrono>

namespace GT {

	namespace NET {

		GT_ThreadPool::GT_ThreadPool()
		{
		}


		GT_ThreadPool::~GT_ThreadPool()
		{
		}

        void GT_ThreadPool::Start(size_t poolsize, std::function<void()> thread_func) {
            if (poolsize <= 0) {
                printf("pool size unreasonable! \n");
                return;
            }
            
            for (int index = 0; index < poolsize; index++) {
                std::atomic<bool> end_thread;
                workpool_.push_back(Thread_Tuple(std::thread(&LongTimeWorker_, this, thread_func, std::ref(end_thread)), std::move(end_thread.load()))); // to be fix: end_thread move
            }
        }

        void GT_ThreadPool::LongTimeWorker_(std::function<void()> f, std::atomic<bool> end_thread) {
            while (!end_thread) {
                f();
            }
            printf("thread %d exit! \n", std::this_thread::get_id());
        }

        void GT_ThreadPool::Stop() {
            auto iter = workpool_.begin();
            while (iter != workpool_.end()) {
                iter->end_thread_ = true;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if (iter->this_thread_.joinable()) {
                    iter->this_thread_.join();
                }
                iter++;
            }
            printf("all worker exit! \n");
        }
	}
}
