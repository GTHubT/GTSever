#include "GT_ServerManager.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <functional>

namespace GT {

    namespace NET {

#ifndef GT_IOCP
#define GT_IOCP		GT_IOCPWrapper::GetInstance()
#endif

		GT_ServerManager::GT_ServerManager() :server_manager_initted_(false),
											log_level_(GT_LOG_LEVEL_OFF),
											log_name_("GTServer"),
											cfg_path_(".//GTServer.cfg"),
											max_logsize_(50),
											read_event_call_back_(nullptr),
											write_event_call_back_(nullptr)
        {
        }


        GT_ServerManager::~GT_ServerManager()
        {
        }

		GT_ServerManager& GT_ServerManager::GetInstance() {
			static GT_ServerManager manager_instance;
			return manager_instance;
		}

		void GT_ServerManager::InitLogAndCfgSrvice(std::string cfg_path) {
			/* load config */
			cfg_path_ = cfg_path;
			GT::UTIL::GT_Util_CfgHelper::LoadCfg(cfg_path_);

			/* init log */
			log_name_ = GT_READ_CFG_STRING("log_control", "log_name", "GT");
			std::string log_level_str = GT_READ_CFG_STRING("log_control", "log_level", "off");
			log_level_ = LoglevelConvert(log_level_str);
			max_logsize_ = GT_READ_CFG_INT("log_control", "max_log_size", 50);
			GT::UTIL::GT_Util_GlogWrapper::GetInstance().GT_LogInitialize(log_name_, log_level_, max_logsize_);
		}

		bool GT_ServerManager::Initialize() {
			if (server_manager_initted_)
				return server_manager_initted_;

			server_manager_initted_ = GT_IOCP.Initialize();
			if (!server_manager_initted_) {
				GT_LOG_ERROR("IOCP init failed!");
			}
			return server_manager_initted_;
		}

		GT_LOG_LEVEL GT_ServerManager::LoglevelConvert(std::string loglevel) {
			if (loglevel == "all") {
				return GT_LOG_LEVEL_ALL;
			}
			else if (loglevel == "off") {
				return GT_LOG_LEVEL_OFF;
			}
			else if(loglevel == "warn")
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

		void GT_ServerManager::RegisterServerCallBack(IO_EVENT_TYPE type, CallBackFunc func_) {
			GT_TRACE_FUNCTION;
			switch (type)
			{
			case IO_EVENT_READ:
				read_event_call_back_ = func_;
				GT_LOG_DEBUG("register read event call back success!");
				break;
			case IO_EVENT_WRITE:
				write_event_call_back_ = func_;
				GT_LOG_DEBUG("register write event call back success!");
				break;
			default:
				GT_LOG_DEBUG("unknown io type!");
				break;
			}
		}

		void GT_ServerManager::UnRegisterServerCallBack(IO_EVENT_TYPE type) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Unregister call back function...");
			switch (type)
			{
			case IO_EVENT_READ:
				read_event_call_back_ = nullptr;
				GT_LOG_DEBUG("unregister read event call back success!");
				break;
			case IO_EVENT_WRITE:
				write_event_call_back_ = nullptr;
				GT_LOG_DEBUG("unregister write event call back success!");
				break;
			default:
				GT_LOG_DEBUG("unknown io type!");
				break;
			}
		}

		void GT_ServerManager::GTStartService() {
			GT_TRACE_FUNCTION;
			dispatch_event_func_ = std::bind(&GT_ServerManager::DispatchEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
			GT_IOCP.GTStartService(dispatch_event_func_);
		}

		void GT_ServerManager::DispatchEvent(IO_EVENT_TYPE type, SOCKETCONTEXT_SHAREPTR completion_key, IO_BUFFER_PTR io_ptr, long len) {
			switch (type)
			{
			case IO_EVENT_READ:
				read_event_call_back_ != nullptr ? read_event_call_back_((PULONG_PTR)completion_key.get(), io_ptr->GetWsaBuf().buf, len) : NULL;
				break;
			case IO_EVENT_WRITE:
				write_event_call_back_ != nullptr ? write_event_call_back_((PULONG_PTR)completion_key.get(), io_ptr->GetWsaBuf().buf, len) : NULL;
				break;
			default:
				GT_LOG_DEBUG("unknown message!");
				break;
			}
		}

		void GT_ServerManager::PostWriteEvent(const PULONG_PTR completion_key, char* data, size_t data_len) {
			GT_TRACE_FUNCTION;
			GT_IOCP.SendDataUserInterface(completion_key, data, data_len);
		}

		void GT_ServerManager::StopService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("service stopping...");
			GT_IOCP.StopService();
		}
    }
}
