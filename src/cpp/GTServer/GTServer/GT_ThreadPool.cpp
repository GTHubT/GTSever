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
                workpool_.push_back(std::thread(thread_func));
            }
        }

        void GT_ThreadPool::LongTimeWorker_(std::function<void()> f) {
            while (true)
            {
                try {
                    
                }
                catch (...) {

                }
            }
        }

        void GT_ThreadPool::Stop() {
            auto iter = workpool_.begin();
            while (iter != workpool_.end()) {
                if (iter->joinable()) {
                    iter->join();
                }
            }
        }
	}
}
