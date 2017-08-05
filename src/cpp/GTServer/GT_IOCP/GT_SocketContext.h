#ifndef GT_NET_IOCONTEXT_H_
#define GT_NET_IOCONTEXT_H_

/************************************************************************/
/* SocketContext serve as a completion key for IOCP server              */
/************************************************************************/

#include "GT_IOContextBuffer.h"

#include <WinSock2.h>
#include <memory>
#include <chrono>
#include <map>

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
		enum SOCKET_TYPE {		/* add type for socket context collector, do not collect listen socket */
			NULL_SOCKET,
			LISTEN_SOCKET,
			ACCEPTED_SOCKET
		};

		class GT_SocketConetxt {
		public:
			GT_SocketConetxt();
			~GT_SocketConetxt();

			void SetContextSocket(SOCKET_SHAREPTR sock_ptr) { socket_shared_ptr_ = sock_ptr; }
			void SetContextSocketAddr(SOCKADDR_IN sockaddr) { socket_addr_ = sockaddr; }
			void AddIOContext2Cache(IO_BUFFER_PTR io_ptr);

            SOCKET_SHAREPTR GetContextSocketPtr() { return socket_shared_ptr_; }
            SOCKADDR_IN     GetSocketAddr() { return socket_addr_; }
            std::map<ULONG_PTR, IO_BUFFER_PTR>&   GetIOBufferCache() { return socket_io_buffer_cache_; }

			void ResetTimer() { time_control_ = std::chrono::system_clock::now(); }
			std::chrono::system_clock::time_point GetTimer() { return time_control_; }

			void SetSocketType(SOCKET_TYPE type) { sock_type_ = type; }
			SOCKET_TYPE GetSocketType() { return sock_type_; }


		private:
			SOCKET_TYPE								sock_type_;
			SOCKET_SHAREPTR							socket_shared_ptr_;
			SOCKADDR_IN								socket_addr_;
			std::map<ULONG_PTR, IO_BUFFER_PTR>		socket_io_buffer_cache_;
			/***********************************************************************************************************/
			/*another way to lean the close socket, I set timer for each completion key, every time the completion have*/
			/*send or recv event happen, reset the timer, if the completion key have not got any event within config "out_date_control" */
			/***********************************************************************************************************/
			std::chrono::system_clock::time_point	time_control_;			
		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
