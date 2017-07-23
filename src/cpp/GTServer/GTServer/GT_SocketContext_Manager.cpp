#include "GT_SocketContext_Manager.h"


namespace GT {

	namespace NET {

		GT_SocketContext_Manager::GT_SocketContext_Manager() {

		}

		GT_SocketContext_Manager::~GT_SocketContext_Manager() {
		}

		GT_SocketContext_Manager& GT_SocketContext_Manager::GetInstance() {
			GT_SocketContext_Manager socket_context_manager_;
			return socket_context_manager_;
		}
	}
}