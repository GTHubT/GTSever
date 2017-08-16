#include "GT_Select_Core.h"


namespace GT {
    namespace NET {

        GT_Select_Core::GT_Select_Core()
        {
			select_cb_func_ = NULL;
        }


        GT_Select_Core::~GT_Select_Core()
        {
        }


		bool GT_Select_Core::Initialize() {
			bool ret = false;
			return ret;
		}

		void GT_Select_Core::StartService() {
			
		}

		void GT_Select_Core::Select_service_() {

		}

		void GT_Select_Core::RegisterCallback(gt_event_callback cb) {
			select_cb_func_ = cb;
		}

		void GT_Select_Core::UnRegisterCallback() {
			select_cb_func_ = NULL;
		}

		void GT_Select_Core::DispatchEvent_(EVENT_TYPE type, ULONG_PTR sock_ptr, char* data, size_t len) {

		}

		void GT_Select_Core::StopService() {

		}

		bool GT_Select_Core::Finalize() {

		}

    }
}
