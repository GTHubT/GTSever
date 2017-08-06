#ifndef GT_CLIENT_H_
#define GT_CLIENT_H_

#include <atomic>
#include <thread>

#include "GT_IOCP/GT_Definition.h"

namespace GT {

	namespace CLIENT {

		class GT_Client
		{
		public:
			GT_Client();
			~GT_Client();

			bool InitializeLog(std::string cfg_path);
			bool InitializeClient();
			void StartTest();
			void StopTest();

		private:
			void TestFunc_(std::atomic_bool& end_thread);
			GT_LOG_LEVEL LoglevelConvert(std::string loglevel);

		private:
			std::thread client_test_thread_;
			std::atomic_bool stop_thread_;
		};
	}
}

#endif	

