#ifndef GT_NET_MULITYTHREADS_H_
#define GT_NET_MULITYTHREADS_H_

#include <vector>
#include <atomic>
#include <thread>
namespace GT {
    namespace NET {
        struct thread_tuple {
            std::thread this_thread_;
            std::atomic<bool> end_thread_;
            thread_tuple::thread_tuple(std::thread t, std::atomic<bool> b) :this_thread_(std::move(t)), end_thread_(std::move(b.load())){}
        };

        class GT_ThreadPool
        {
        public:
            GT_ThreadPool();
            ~GT_ThreadPool();

        public:
            void Start(size_t poolsize, std::function<void()>);
            void Stop();

        private:
            void LongTimeWorker_(std::function<void()>, std::atomic<bool>);

        private:
            std::vector<thread_tuple> workpool_;
        };
    }
}


#endif // GT_NET_MULITYTHREADS_H_