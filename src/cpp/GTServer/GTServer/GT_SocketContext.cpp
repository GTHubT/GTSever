#include "GT_SocketContext.h"

namespace GT {

	namespace NET {

		GT_SocketConetxt::GT_SocketConetxt() {

		}

		GT_SocketConetxt::~GT_SocketConetxt() {

		}

		void GT_SocketConetxt::AddIOContext2Cache(IO_BUFFER_PTR io_ptr) {
			socket_io_buffer_cache_.push_back(io_ptr);
		}
	}
}