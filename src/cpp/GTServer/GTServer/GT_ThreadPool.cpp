#include "GT_ThreadPool.h"

#include <chrono>

namespace GT {

	namespace NET {

		GT_ThreadPool::GT_ThreadPool():poolsize_(0), workpool_started_(false)
		{
			workpool_.clear();
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
				Thread_Tuple* thread_tuple = new Thread_Tuple();
                thread_tuple->this_thread_ = std::move(std::thread(&GT_ThreadPool::LongTimeWorker_, this, thread_func, std::ref(thread_tuple->end_thread_)));
                workpool_.push_back(thread_tuple);
                printf("create thread TID = %d \n", thread_tuple->this_thread_.get_id());
			}
			workpool_started_ = true;
        }


        void GT_ThreadPool::Stop() {
            auto iter = workpool_.begin();
            while (iter != workpool_.end()) {
                (*iter)->end_thread_ = true;
                if ((*iter)->this_thread_.joinable()) {
					(*iter)->this_thread_.join();
                }
                delete (*iter);
                iter++;
            }
            printf("all worker exit! \n");
        }


        void GT_ThreadPool::LongTimeWorker_(std::function<void()> f, std::atomic<bool>& end_thread) {
            while (!end_thread) {
                f();
            }
            printf("thread TID = %d exit! \n", std::this_thread::get_id());
        }


		size_t GT_ThreadPool::GetPoolSize() {
			return workpool_started_ ? poolsize_ : 0;
		}
	}
}
