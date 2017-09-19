#include "GT_IOContextBuffer.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"

namespace GT {

	namespace NET {

		GT_IOContextBuffer::GT_IOContextBuffer(size_t len) {
			Internal = InternalHigh = 0;
			Offset = OffsetHigh = 0;
			hEvent = NULL;
			io_buffer_size_ = len;
			io_wsa_buf_.buf = io_buffer_;
			io_wsa_buf_.len = io_buffer_size_;
			io_event_type_ = IO_EVENT_NULL;
			io_socket_ptr_ = nullptr;
		}

		GT_IOContextBuffer::~GT_IOContextBuffer() {
			delete[] io_buffer_;
		}

		bool GT_IOContextBuffer::AllocateIOBuffer() {
			io_buffer_ = new char[io_buffer_size_];
			io_wsa_buf_.buf = io_buffer_;		/* WSABUF is use for WSARecv and WSASend, as a buffer */
			io_wsa_buf_.len = io_buffer_size_;
            if (nullptr != io_buffer_)
                memset(io_buffer_, 0, io_buffer_size_);
			return io_buffer_ != nullptr;
		}

		void GT_IOContextBuffer::SetIOBufferSocket(std::shared_ptr<SOCKET> s_ptr) {
			io_socket_ptr_ = s_ptr;
		}

		void GT_IOContextBuffer::SetIOBufferEventType(IO_EVENT_TYPE type) {
			io_event_type_ = type;
		}

		void GT_IOContextBuffer::ResetBuffer() {
			memset(io_buffer_, 0, io_buffer_size_);
		}

        char* GT_IOContextBuffer::GetBufferAddr() {
            return io_buffer_;
        }

        size_t GT_IOContextBuffer::GetBufferSize() {
            return io_buffer_size_;
        }

        std::shared_ptr<SOCKET> GT_IOContextBuffer::GetClientSocketPtr() {
            return nullptr != io_socket_ptr_ ? io_socket_ptr_ : nullptr;
        }

		bool GT_IOContextBuffer::AllocateIOBufferBySize(size_t len) {
			io_buffer_size_ = len;
			io_buffer_ = new char[io_buffer_size_]; 
			io_wsa_buf_.buf = io_buffer_;		/* WSABUF is use for WSARecv and WSASend, as a buffer */
			io_wsa_buf_.len = io_buffer_size_;
			if (nullptr != io_buffer_)
				memset(io_buffer_, 0, io_buffer_size_);
			return nullptr != io_buffer_;
		}
	}
}