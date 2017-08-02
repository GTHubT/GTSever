#include "GT_SocketContext.h"

namespace GT {

	namespace NET {

		GT_SocketConetxt::GT_SocketConetxt() {
			sock_type_ = NULL_SOCKET;
			time_control_ = std::chrono::system_clock::now();
		}

		GT_SocketConetxt::~GT_SocketConetxt() {

		}

		void GT_SocketConetxt::AddIOContext2Cache(IO_BUFFER_PTR io_ptr) {
			socket_io_buffer_cache_.insert(io_ptr);
		}

	}
}