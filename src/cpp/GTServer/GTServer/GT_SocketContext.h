#ifndef GT_NET_IOCONTEXT_H_
#define GT_NET_IOCONTEXT_H_

#include "GT_IOContextBuffer.h"

#include <WinSock2.h>
#include <memory>
#include <vector>

namespace GT {

	namespace NET {

#ifndef IO_BUFFER_PTR
#define IO_BUFFER_PTR	std::shared_ptr<GT_IOContextBuffer>
#endif
		class GT_SocketConetxt {
		public:
			GT_SocketConetxt();
			~GT_SocketConetxt();

			void SetContextSocket(std::shared_ptr<SOCKET> sock_ptr) { socket_shared_ptr_ = sock_ptr; }
			void SetContextSocketAddr(SOCKADDR_IN sockaddr) { socket_add_ = sockaddr; }
			void AddIOContext2Cache(IO_BUFFER_PTR io_ptr);

		private:
			std::shared_ptr<SOCKET>			socket_shared_ptr_;
			SOCKADDR_IN						socket_add_;
			std::vector<IO_BUFFER_PTR>		socket_io_buffer_cache_;
		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
