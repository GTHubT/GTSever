#ifndef GT_NET_SERVER_MANAGER_H_
#define GT_NET_SERVER_MANAGER_H_

/***********************************************************************************************/
/* this class serve as the only interface for dll interface call IOCP Service and other module */
/***********************************************************************************************/

#include "GT_Definition.h"
#include "GT_IOCPWrapper.h"

#include <string>

namespace GT {

    namespace NET {

		typedef void(*CallBackFunc)(const PULONG_PTR, const char*, int);
        class GT_ServerManager
        {
        public:
            ~GT_ServerManager();

			static GT_ServerManager& GetInstance();

			bool Initialize(std::string cfg_path);
			void GTStartService();
			void StopService();

			void RegisterServerCallBack(IO_EVENT_TYPE type, CallBackFunc call_back);
			void UnRegisterServerCallBack(IO_EVENT_TYPE);
			void DispatchEvent(IO_EVENT_TYPE type, SOCKETCONTEXT_SHAREPTR completion_key, IO_BUFFER_PTR io_ptr);
			void PostWriteEvent(const PULONG_PTR completion_key, char* data, size_t data_len);

		private:
			GT_ServerManager();

			GT_LOG_LEVEL LoglevelConvert(std::string loglevel);

		private:
			CallBackFunc  write_event_call_back_;
			CallBackFunc  read_event_call_back_;

		private:
			bool server_manager_initted_;
			std::string  cfg_path_;
			GT_LOG_LEVEL log_level_;
			std::string  log_name_;
			size_t		 max_logsize_;
        };
    }
}

#endif //GT_NET_SERVER_MANAGER_H_