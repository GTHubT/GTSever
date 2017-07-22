#ifndef GT_NET_IOBUFFER_MANAGER_H_
#define GT_NET_IOBUFFER_MANAGER_H_

#include "GT_IOBuffer.h"


#include <memory>
#include <deque>

namespace GT {

    namespace NET {

        class GT_IOBuffer_Manager
        {
        public:
            GT_IOBuffer_Manager();
            ~GT_IOBuffer_Manager();

			bool PreAllocateSomeIOBuffer();

		private:
			std::deque<std::shared_ptr<GT_IOBuffer>> io_buffer_cache_;
        };
    }
}


#endif // GT_NET_IOBUFFER_MANAGER_H_