
#include "GT_Select_Wrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"

namespace GT {
    namespace NET {

        GT_Select_Wrapper::GT_Select_Wrapper():read_cb(nullptr), write_cb(nullptr)
        {
        }


        GT_Select_Wrapper::~GT_Select_Wrapper()
        {
        }

		bool GT_Select_Wrapper::Initialize(std::string cfg_path) {

		}

		void GT_Select_Wrapper::StartService() {

		}

		bool GT_Select_Wrapper::Finalize() {

		}

		void GT_Select_Wrapper::RegisterCallBack(gt_event_callback cb, EVENT_TYPE type) {
			switch (type)
			{
			case EVENT_READ:
				read_cb = cb;
				GT_LOG_INFO("register read call back success!");
				break;
			case EVENT_WRITE:
				write_cb = cb;
				GT_LOG_INFO("register write call back success!");
				break;
			default:
				GT_LOG_DEBUG("register callback got unknow callback type!");
				break;
			}
		}
    }
}