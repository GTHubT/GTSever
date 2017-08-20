
#include "GT_Select_Wrapper.h"

namespace GT {
    namespace NET {

		void SelectCallBack(EVENT_TYPE type, intptr_t ptr, char* data, int len) {
			GT_Select_Wrapper::DispatchEvent(type, ptr, data, len);
		}

		gt_event_callback GT_Select_Wrapper::read_cb = nullptr;
		gt_event_callback GT_Select_Wrapper::write_cb = nullptr;

        GT_Select_Wrapper::GT_Select_Wrapper():service_inited_(false), service_started_(false)
        {
        }


        GT_Select_Wrapper::~GT_Select_Wrapper()
        {
        }

		bool GT_Select_Wrapper::Initialize(std::string cfg_path) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Init select service...");
			
			/* load config */
			GT::UTIL::GT_Util_CfgHelper::LoadCfg(cfg_path);

			/* init log */
			std::string log_name = GT_READ_CFG_STRING("log_control", "log_name", "GT");
			std::string log_level_str = GT_READ_CFG_STRING("log_control", "log_level", "off");
			GT_LOG_LEVEL log_level = LoglevelConvert(log_level_str);
			int max_logsize_ = GT_READ_CFG_INT("log_control", "max_log_size", 50);
			GT::UTIL::GT_Util_GlogWrapper::GetInstance().GT_LogInitialize(log_name, log_level, max_logsize_);

			/* init core service */
			service_inited_ =  select_core_.Initialize();

			if (!service_inited_) {
				GT_LOG_ERROR("Service Init Failed!");
			}
			return service_inited_;
		}

		void GT_Select_Wrapper::StartService() {
			select_core_.StartService();
		}

		bool GT_Select_Wrapper::Finalize() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Service Finalize!");
			return select_core_.Finalize();
		}

		void GT_Select_Wrapper::DispatchEvent(EVENT_TYPE type, intptr_t sock_ptr, char* data, int len) {
			switch (type)
			{
			case EVENT_READ:
				read_cb == nullptr ? NULL : read_cb(type, sock_ptr, data, len);
				break;
			case EVENT_WRITE:
				write_cb == nullptr ? NULL : write_cb(type, sock_ptr, data, len);
				break;
			default:
				break;
			}
		}

		void GT_Select_Wrapper::RegisterSelectCallBack() {
			select_core_.RegisterCallback((gt_event_callback)SelectCallBack);
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

		GT_LOG_LEVEL GT_Select_Wrapper::LoglevelConvert(std::string loglevel) {
			if (loglevel == "all") {
				return GT_LOG_LEVEL_ALL;
			}
			else if (loglevel == "off") {
				return GT_LOG_LEVEL_OFF;
			}
			else if (loglevel == "warn")
			{
				return GT_LOG_LEVEL_WARNING;
			}
			else if (loglevel == "error") {
				return GT_LOG_LEVEL_ERROR;
			}
			else {
				printf("unknown log level! \n");
				return GT_LOG_LEVEL_OFF;
			}
		}
    }
}