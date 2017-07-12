#include "GT_ThreadPool.h"

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
                std::atomic<bool> end_thread_ = false;
                //workpool_.insert(std::pair<std::thread, std::atomic<bool>>(std::thread(&LongTimeWorker_, this, thread_func, end_thread_), end_thread_));
            }
        }

        void GT_ThreadPool::LongTimeWorker_(std::function<void()> f, std::atomic<bool> end_thread) {
            while (!end_thread) {
                f();
            }
            printf("thread %d exit! \n", std::this_thread::get_id());
        }

        void GT_ThreadPool::Stop() {
            std::map<std::thread, std::atomic<bool>>::iterator iter = workpool_.begin();
            while (iter != workpool_.end()) {
                iter->second = true;
                if (iter->first.joinable()) {
                    iter->first->join();
                }
                iter++;
            }
            printf("all worker exit! \n");
        }
	}
}
