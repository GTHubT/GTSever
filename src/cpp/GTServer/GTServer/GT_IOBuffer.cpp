#include "GT_IOBuffer.h"

namespace GT {

	namespace NET {

		GT_IOBuffer::GT_IOBuffer(SOCKET s, size_t len) {
			Internal = InternalHigh = 0;
			Offset = OffsetHigh = 0;
			hEvent = NULL;
			io_socket_ = s;
			io_buffer_size_ = len;
			io_wsa_buf_.buf = io_buffer_;
			io_wsa_buf_.len = io_buffer_size_;
		}

		GT_IOBuffer::~GT_IOBuffer() {
			delete[] io_buffer_;
		}

		bool GT_IOBuffer::Allocate_GT_IOBuffer() {
			io_buffer_ = new char[io_buffer_size_];
			return io_buffer_ == nullptr ? false : true;
		}
	}
}