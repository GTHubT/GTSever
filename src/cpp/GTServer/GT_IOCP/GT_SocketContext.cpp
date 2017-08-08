#include "GT_SocketContext.h"
#include "GT_IOContextBuffer_Manager.h"

namespace GT {

	namespace NET {

		GT_SocketConetxt::GT_SocketConetxt() {
			sock_type_ = NULL_SOCKET;
			time_control_ = std::chrono::system_clock::now();
		}

		GT_SocketConetxt::~GT_SocketConetxt() {

		}

		void GT_SocketConetxt::AddIOContext2Cache(IO_BUFFER_PTR io_ptr) {
			std::lock_guard<std::mutex> lk(sock_context_mutex_);
			socket_io_buffer_cache_.insert(std::make_pair((ULONG_PTR)io_ptr.get(), io_ptr));
		}

		void GT_SocketConetxt::ReleaseUsedIOContext(IO_BUFFER_PTR io_context) {
			std::lock_guard<std::mutex> lk(sock_context_mutex_);
			auto iter = socket_io_buffer_cache_.find((ULONG_PTR)io_context.get());
			GT_IOContextBuffer_Manager::GetInstance().ReleaseIOBuffer(io_context);
			if (iter != socket_io_buffer_cache_.end()) {
				socket_io_buffer_cache_.erase(iter);
			}
		}

		std::map<ULONG_PTR, IO_BUFFER_PTR>& GT_SocketConetxt::GetIOBufferCache() {
			std::lock_guard<std::mutex> lk(sock_context_mutex_);
			return socket_io_buffer_cache_;
		}

	}
}