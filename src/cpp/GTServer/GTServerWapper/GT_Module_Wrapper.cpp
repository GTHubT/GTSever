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

		//void GT_Module_Wrapper::InitLogService(std::string cfg_pth /* = DEFAULT_CFG_PATH */) {
		//	if (cfg_pth.empty()) {
		//		GT_LOG_INFO("config path empty, use default config path.");
		//	}
		//	//GTIOCP_InitLogService(cfg_pth);
		//}

		bool GT_Module_Wrapper::Initialize(std::string cfg_path) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("initialize GT service...");
			if (is_module_initted_) {
				GT_LOG_INFO("GT Service already initted!");
				return is_module_initted_;
			}

			GT_ERROR_CODE errcode = GT_ERROR_CODE::GT_ERROR_FAILED;
			if (module_type_ == GT_IOCP) {

				errcode = GTIOCP_Initialize(cfg_path);
				/* register callback function */
				GTIOCP_RegisterEventCallBack(IO_EVENT_READ, ReadCallback);
				GTIOCP_RegisterEventCallBack(IO_EVENT_WRITE, WriteCallback);
				
			}
			else {
				//TODO:
				bool ret = GTSelect_Initialize(cfg_path);
				GTSelect_RegisterCallBack(EVENT_READ, ReadCallback);
				GTSelect_RegisterCallBack(EVENT_WRITE, WriteCallback);
				ret ? errcode = GT_ERROR_CODE::GT_ERROR_SUCCESS : GT_ERROR_CODE::GT_ERROR_FAILED;
			}
			errcode == GT_ERROR_FAILED ? is_module_initted_ = false : is_module_initted_ = true;
			return is_module_initted_;
		}

		void GT_Module_Wrapper::SetModuleType(MODULE_TYPE type) {
			module_type_ = type;
		}

		bool GT_Module_Wrapper::StartGTService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Start GT Service...");

			GT_ERROR_CODE errcode = GT_ERROR_CODE::GT_ERROR_FAILED;
			if (module_type_ == GT_IOCP) {
				 errcode = GTIOCP_StartService();
				 errcode == GT_ERROR_FAILED ? is_module_initted_ = false : is_module_initted_ = true;
			}
			else if (module_type_ == GT_Select){
				//TODO:
				GTSelect_StartGTService();
			}
			return is_module_initted_;
		}

		void GT_Module_Wrapper::ExitGTService() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Now Stopping GT Service...");

			if (module_type_ == GT_IOCP) {
				GTIOCP_UnRegisterEventCallBack(IO_EVENT_READ);
				GTIOCP_UnRegisterEventCallBack(IO_EVENT_WRITE);
				GTIOCP_Uninitialize();
			}
			else {
				GTSelect_UnRegisterCallBack(EVENT_READ);
				GTSelect_UnRegisterCallBack(EVENT_WRITE);
				GTSelect_Finalize();
			}
		}

		GT_Module_Wrapper& GT_Module_Wrapper::GetInstance() {
			static GT_Module_Wrapper module_instance_;
			return module_instance_;
		}

		void GT_Module_Wrapper::ReadEventCallback(PULONG_PTR ptr, const char* data, int len) {
			GT_LOG_ERROR("reading data...");
            std::string recv_data(data, len);
			printf("get data from client data len = %d, content: %s \n", len, recv_data.c_str());
		}

		void GT_Module_Wrapper::WriteEventCallBack(PULONG_PTR ptr, const char* data, int len) {
			GT_LOG_ERROR("writing event complete!");
			printf("get write complete event! \n");
		}
    }
}
