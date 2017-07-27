#ifndef GT_NET_IOCONTEXT_H_
#define GT_NET_IOCONTEXT_H_

/************************************************************************/
/* SocketContext serve as a completion key for IOCP server              */
/************************************************************************/

#include "GT_IOContextBuffer.h"

#include <WinSock2.h>
#include <memory>
#include <chrono>
#include <set>

/************************************************************************/
/* socket context is combined by socket pool element and IO context     */
/* serve as a completion key											*/	
/************************************************************************/

namespace GT {

	namespace NET {

#ifndef IO_BUFFER_PTR
#define IO_BUFFER_PTR	std::shared_ptr<GT_IOContextBuffer>
#endif

#ifndef SOCKET_SHAREPTR
#define SOCKET_SHAREPTR	std::shared_ptr<SOCKET>
#endif

		class GT_SocketConetxt {
		public:
			GT_SocketConetxt();
			~GT_SocketConetxt();

			void SetContextSocket(SOCKET_SHAREPTR sock_ptr) { socket_shared_ptr_ = sock_ptr; }
			void SetContextSocketAddr(SOCKADDR_IN sockaddr) { socket_addr_ = sockaddr; }
			void AddIOContext2Cache(IO_BUFFER_PTR io_ptr);

            SOCKET_SHAREPTR GetContextSocketPtr() { return socket_shared_ptr_; }
            SOCKADDR_IN     GetSocketAddr() { return socket_addr_; }
            std::set<IO_BUFFER_PTR>   GetIOBufferCache() { return socket_io_buffer_cache_; }

		private:
			SOCKET_SHAREPTR							socket_shared_ptr_;
			SOCKADDR_IN								socket_addr_;
			std::set<IO_BUFFER_PTR>					socket_io_buffer_cache_;
			std::chrono::system_clock::time_point	time_control_;			/* send a heart package, if out of time, release the context */
		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
