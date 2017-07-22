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

			void SetContextSocket(SOCKET& sock) { s = sock; }
			void SetContextSocketAddr(SOCKADDR_IN sockaddr) { s_add = sockaddr; }
			void AddIOContext2Cache(IO_BUFFER_PTR io_ptr);

		private:
			SOCKET			s;
			SOCKADDR_IN		s_add;
			std::vector<IO_BUFFER_PTR> socket_io_buffer_cache_;
		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
