#include "GT_IOContextBuffer.h"

namespace GT {

	namespace NET {

		GT_IOContextBuffer::GT_IOContextBuffer(size_t len) {
			Internal = InternalHigh = 0;
			Offset = OffsetHigh = 0;
			hEvent = NULL;
			io_buffer_size_ = len;
			io_wsa_buf_.buf = io_buffer_;
			io_wsa_buf_.len = io_buffer_size_;
		}

		GT_IOContextBuffer::~GT_IOContextBuffer() {
			delete[] io_buffer_;
		}

		bool GT_IOContextBuffer::AllocateIOBuffer() {
			io_buffer_ = new char[io_buffer_size_];
			return io_buffer_ == nullptr ? false : true;
		}

		void GT_IOContextBuffer::SetIOBufferSocket(SOCKET s) {
			io_socket_ = s;
		}

		void GT_IOContextBuffer::ResetBuffer() {
			memset(io_buffer_, 0, io_buffer_size_);
		}
	}
}