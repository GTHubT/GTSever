#include "GT_Resource_Manager.h"


namespace GT {

	namespace NET {

		GT_Resource_Manager::GT_Resource_Manager():is_enabled_(false) {

		}

		GT_Resource_Manager::~GT_Resource_Manager() {
		}

		GT_Resource_Manager& GT_Resource_Manager::GetInstance() {
			GT_Resource_Manager socket_context_manager_;
			return socket_context_manager_;
		}

		bool GT_Resource_Manager::Initialize() {
			return is_enabled_;
		}
	}
}