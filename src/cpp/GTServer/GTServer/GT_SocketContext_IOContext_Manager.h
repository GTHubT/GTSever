#ifndef GT_SOCKET_IOCONTEXT_MANAGER_H_
#define GT_SOCKET_IOCONTEXT_MANAGER_H_

#include "GT_SocketContext.h"
#include "GT_IOContextBuffer.h"

#include <set>

namespace GT {

	namespace NET {

#ifndef IOCONTEXT_SHAREPTR
#define IOCONTEXT_SHAREPTR	std::shared_ptr<GT_IOContextBuffer>
#endif

#ifndef SOCKETCONTEXT_SHAREPTR
#define SOCKETCONTEXT_SHAREPTR	std::shared_ptr<GT_IOContextBuffer>
#endif

#ifndef SOCKET_SHAREPTR
#define SOCKET_SHAREPTR	std::shared_ptr<GT_IOContextBuffer>
#endif
		class GT_SocketContext_IOContext_Manager {

		public:
			~GT_SocketContext_IOContext_Manager();
			GT_SocketContext_IOContext_Manager& GetInstance();

			SOCKETCONTEXT_SHAREPTR CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr);
			void	PushIOEvent2Cache(IOCONTEXT_SHAREPTR ptr);
			void	ReleaseIOBufferFromSockContextAndIOEventCache(IOCONTEXT_SHAREPTR ptr);

		private:
			GT_SocketContext_IOContext_Manager();


		private:
			std::set<SOCKETCONTEXT_SHAREPTR> completion_key_ptr_cache_;
		};
	}
}

#endif // GT_SOCKET_IOCONTEXT_MANAGER_H_
