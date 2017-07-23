#include "GT_SocketContext_IOContext_Manager.h"


namespace GT {

	namespace NET {

		GT_SocketContext_IOContext_Manager::GT_SocketContext_IOContext_Manager():is_enabled_(false) {

		}

		GT_SocketContext_IOContext_Manager::~GT_SocketContext_IOContext_Manager() {
		}

		GT_SocketContext_IOContext_Manager& GT_SocketContext_IOContext_Manager::GetInstance() {
			GT_SocketContext_IOContext_Manager socket_context_manager_;
			return socket_context_manager_;
		}

		bool GT_SocketContext_IOContext_Manager::Initialize() {
			return is_enabled_;
		}
	}
}