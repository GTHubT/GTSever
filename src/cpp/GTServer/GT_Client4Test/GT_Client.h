#ifndef GT_CLIENT_H_
#define GT_CLIENT_H_

#include <atomic>
#include <thread>
#include <vector>

#include "GT_IOCP/GT_Definition.h"

namespace GT {

	namespace CLIENT {

        struct sock_pack {
            SOCKET sock_;
            unsigned int use_times_;
        };

		class GT_Client
		{
		public:
			GT_Client();
			~GT_Client();

			bool InitializeLog(std::string cfg_path);
			bool InitializeClient();
			void StartTest(std::string test_mode);
			void StopTest();

        private:
            void StartTestByContinuous_(std::atomic_bool& end_thread);
            void StartTestByUnContinuous_(std::atomic_bool& end_thread);
			GT_LOG_LEVEL LoglevelConvert(std::string loglevel);

		private:
			std::thread client_test_thread_;
			std::atomic_bool stop_thread_;
            std::vector<SOCKET> connect_sock_;
		};
	}
}

#endif	

