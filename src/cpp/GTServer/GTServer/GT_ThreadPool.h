#ifndef GT_NET_MULITYTHREADS_H_
#define GT_NET_MULITYTHREADS_H_

#include <vector>
#include <atomic>
#include <thread>
#include <share.h>
namespace GT {
    namespace NET {
        struct Thread_Tuple {
            std::thread this_thread_;
            std::atomic<bool> end_thread_;
			Thread_Tuple::Thread_Tuple() :end_thread_(false){}
			Thread_Tuple::Thread_Tuple(Thread_Tuple& th) {
				this_thread_ = std::move(th.this_thread_);
				end_thread_ = std::move(th.end_thread_.load());
			}
			Thread_Tuple& Thread_Tuple::operator=(Thread_Tuple& th) {
				this_thread_ = std::move(th.this_thread_);
				end_thread_ = std::move(th.end_thread_.load());
				return *this;
			}
			Thread_Tuple::Thread_Tuple(Thread_Tuple&& th) {
				this_thread_ = std::move(th.this_thread_);
				end_thread_ = std::move(th.end_thread_.load());
			}
			Thread_Tuple::Thread_Tuple(std::thread& th):end_thread_(false) {
				this_thread_ = std::move(th);
			}
        };

        class GT_ThreadPool
        {
        public:
            GT_ThreadPool();
            ~GT_ThreadPool();

        public:
            void Start(size_t poolsize, std::function<void()>);
            void Stop();
			size_t GetPoolSize();

        private:
            void LongTimeWorker_(std::function<void()>, std::atomic<bool>&);

        private:
			size_t	poolsize_;
			bool	workpool_started_;
            std::vector<Thread_Tuple*> workpool_;
        };
    }
}


#endif // GT_NET_MULITYTHREADS_H_