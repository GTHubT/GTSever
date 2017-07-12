#include "GT_ThreadPool.h"

#include <chrono>

namespace GT {

	namespace NET {

		GT_ThreadPool::GT_ThreadPool():poolsize_(0)
		{
		}


		GT_ThreadPool::~GT_ThreadPool()
		{
		}

        void GT_ThreadPool::Start(size_t poolsize, std::function<void()> thread_func) { // if you want call thread pool please bind you thread function to std::function<void()>
            if (poolsize <= 0) {
                printf("pool size unreasonable! \n");
                return;
            }
			poolsize_ = poolsize;
			for (int index = 0; index < poolsize; index++) {
				std::atomic<bool> end_thread(false);
				std::shared_ptr<Thread_Tuple> thread_tuple(new Thread_Tuple(std::thread(&GT_ThreadPool::LongTimeWorker_, this, thread_func, std::ref(end_thread))));
				thread_tuple->end_thread_ = std::move(end_thread.load());
				printf("create thread TID = %d \n", thread_tuple->this_thread_.get_id());
				workpool_.push_back(thread_tuple);
			}
        }

        void GT_ThreadPool::LongTimeWorker_(std::function<void()> f, std::atomic<bool> end_thread) {
            while (!end_thread) {
                f();
            }
            printf("thread TID = %d exit! \n", std::this_thread::get_id());
        }

        void GT_ThreadPool::Stop() {
            auto iter = workpool_.begin();
            while (iter != workpool_.end()) {
                (*iter)->end_thread_ = true;
                //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                if ((*iter)->this_thread_.joinable()) {
					(*iter)->this_thread_.join();
                }
                iter++;
            }
            printf("all worker exit! \n");
        }
	}
}
