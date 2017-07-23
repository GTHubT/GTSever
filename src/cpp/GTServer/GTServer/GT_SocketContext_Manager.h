#ifndef GT_SOCKET_IOCONTEXT_MANAGER_H_
#define GT_SOCKET_IOCONTEXT_MANAGER_H_

#include "GT_SocketContext.h"

namespace GT {

	namespace NET {

		class GT_SocketContext_Manager {

		public:
			~GT_SocketContext_Manager();
			GT_SocketContext_Manager& GetInstance();

			GT_SocketConetxt CreateNewSocketContext(std::shared_ptr<SOCKET> sock_ptr);


		private:
			GT_SocketContext_Manager();

		};
	}
}

#endif // GT_SOCKET_IOCONTEXT_MANAGER_H_
