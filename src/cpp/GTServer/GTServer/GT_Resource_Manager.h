#ifndef GT_SOCKET_IOCONTEXT_MANAGER_H_
#define GT_SOCKET_IOCONTEXT_MANAGER_H_

#include "GT_SocketPool.h"
#include "GT_SocketContext.h"
#include "GT_IOContextBuffer_Manager.h"


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
#define SOCKET_SHAREPTR	std::shared_ptr<SOCKET>
#endif
		class GT_Resource_Manager {

		public:
			~GT_Resource_Manager();

			bool								Initialize();
			void								Finalize();
			void								ReleaseIOBuffer(IOCONTEXT_SHAREPTR ptr);
			void								PushIOEvent2Cache(SOCKETCONTEXT_SHAREPTR sock_ptr, IOCONTEXT_SHAREPTR io_ptr);

			SOCKET_SHAREPTR						GetSoket();
			IOCONTEXT_SHAREPTR					GetIOContextBuffer();
			SOCKETCONTEXT_SHAREPTR				CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr);
			static GT_Resource_Manager& GetInstance();

		private:
			GT_Resource_Manager();
			void								ReleaseIOBufferFromSockContextAndIOEventCache(IOCONTEXT_SHAREPTR ptr);


		private:
			bool is_enabled_;
			std::set<SOCKETCONTEXT_SHAREPTR> completion_key_ptr_cache_;
		};
	}
}

#endif // GT_SOCKET_IOCONTEXT_MANAGER_H_
