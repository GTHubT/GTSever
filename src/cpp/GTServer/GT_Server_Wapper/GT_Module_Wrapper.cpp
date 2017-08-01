#include "stdafx.h"
#include "GT_Module_Wrapper.h"

namespace GT {
    namespace MODULE {

		void ReadCallback(PULONG_PTR ptr, const char* data, int data_len) {
			GT_Module_Wrapper::ReadEventCallback(ptr, data, data_len);
		}

		void WriteCallback(PULONG_PTR ptr, const char* data, int data_len) {
			GT_Module_Wrapper::WriteEventCallBack(ptr, data, data_len);
		}

        GT_Module_Wrapper::GT_Module_Wrapper():is_module_initted_(false),module_type_(GT_IOCP)
        {
        }


        GT_Module_Wrapper::~GT_Module_Wrapper()
        {
        }

		void GT_Module_Wrapper::InitLogService(std::string cfg_pth /* = DEFAULT_CFG_PATH */) {
			if (cfg_pth.empty()) {
				GT_LOG_INFO("config path empty, use default config path.");
			}
			GTIOCP_InitLogService(cfg_pth);
		}

		bool GT_Module_Wrapper::Initialize() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("initialize GT service...");
			if (is_module_initted_) {
				GT_LOG_INFO("GT Service already initted!");
				return is_module_initted_;
			}


			if (module_type_ == GT_IOCP) {

				GT_ERROR_CODE errcode = GTIOCP_Initialize();
				if (errcode == GT_ERROR_SUCCESS) {
					is_module_initted_ = false;
					GT_LOG_ERROR("GT service initialize failed!");
					return is_module_initted_;
				}

				/* register callback function */
				GTIOCP_RefisterEventCallBack(IO_EVENT_READ, ReadCallback);
				GTIOCP_RefisterEventCallBack(IO_EVENT_WRITE, WriteCallback);
			}
			else {
				//TODO:
			}

			GT_LOG_INFO("GT Service init success!");
			return is_module_initted_;
		}

		void GT_Module_Wrapper::SetModuleType(MODULE_TYPE type) {
			module_type_ = type;
		}

		bool GT_Module_Wrapper::StartGTService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Start GT Service...");

			if (module_type_ == GT_IOCP) {
				GT_ERROR_CODE errcode = GTIOCP_StartService();
				if (errcode == GT_ERROR_SUCCESS) {
					is_module_initted_ = false;
					GT_LOG_ERROR("GT Service Start Failed!");
				}
				else {
					is_module_initted_ = true;
					GT_LOG_INFO("GT Service start success!");
				}
			}
			else {
				//TODO:
			}

			return is_module_initted_;
		}

		void GT_Module_Wrapper::StopGTService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Now Stopping GT Service...");

			if (module_type_ == GT_IOCP) {
				GTIOCP_UnRegisterEventCallBack(IO_EVENT_READ);
				GTIOCP_UnRegisterEventCallBack(IO_EVENT_WRITE);
				GTIOCP_Uninitialize();
			}
			else {

			}
		}

		GT_Module_Wrapper& GT_Module_Wrapper::GetInstance() {
			static GT_Module_Wrapper module_instance_;
			return module_instance_;
		}

		void GT_Module_Wrapper::ReadEventCallback(PULONG_PTR ptr, const char* data, int len) {
			GT_LOG_ERROR("reading data...");
		}

		void GT_Module_Wrapper::WriteEventCallBack(PULONG_PTR ptr, const char* data, int len) {
			GT_LOG_ERROR("writing event complete!");
		}
    }
}
