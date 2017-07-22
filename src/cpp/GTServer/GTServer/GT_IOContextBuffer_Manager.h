#ifndef GT_NET_IOBUFFER_MANAGER_H_
#define GT_NET_IOBUFFER_MANAGER_H_

#include "GT_IOContextBuffer.h"


#include <memory>
#include <deque>
#include <mutex>

namespace GT {

    namespace NET {
#ifndef IO_BUFFER_PTR
#define IO_BUFFER_PTR	std::shared_ptr<GT_IOContextBuffer>
#endif
        class GT_IOBuffer_Manager
        {
        public:
            ~GT_IOBuffer_Manager();

			bool			Initialize();
			void			Finalize();
			IO_BUFFER_PTR	GetNextIOBufferPtr();
			void			ReleaseIOBuffer(IO_BUFFER_PTR buffer_ptr);
			static GT_IOBuffer_Manager& GetInstance();

		private:
			GT_IOBuffer_Manager();
			bool			PreAllocateSomeIOBuffer_();
			bool			ReAllocateSomeIOBuffer_();
			void			CleanIOBufferCache_();

		private:
			bool			io_buffer_manager_inited_;
			size_t			io_buffer_size_;
			size_t			pre_allocate_size_;
			size_t			re_allocate_size_;
			size_t			size_need_reallocate_;
			std::deque<IO_BUFFER_PTR> io_buffer_cache_;
			std::deque<IO_BUFFER_PTR> io_buffer_in_use_;
			static std::mutex		io_buffer_manager_mutex_;
        };
    }
}


#endif // GT_NET_IOBUFFER_MANAGER_H_