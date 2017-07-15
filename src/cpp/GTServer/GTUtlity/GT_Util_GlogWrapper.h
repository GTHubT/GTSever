#ifndef GT_UTIL_LOG_WRAPPER_
#define GT_UTIL_LOG_WRAPPER_

#include "GTServer/GT_Definition.h"


#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#include <string>
#include <mutex>

namespace GT {
    
    namespace UTIL {

        class GT_Util_GlogWrapper
        {
        public:
            ~GT_Util_GlogWrapper();
			static GT_Util_GlogWrapper& GetInstance();

			bool GT_LogInitialize(std::string logname, GT_LOG_LEVEL level);
			bool GT_LogUnintialize();
			bool GT_SetLoglevel(GT_LOG_LEVEL level);
			void GT_WriteLog(std::ostream& os);

        private:
            GT_Util_GlogWrapper();
			google::LogSeverity GTLoglevel2GoogleLoglevel(GT_LOG_LEVEL);

		private:
			GT_LOG_LEVEL			log_level_;
			std::string				log_path_;
			static std::mutex		log_mutex_;
			bool					is_log_initted_;
        };
    }
}


#endif //GT_UTIL_LOG_WRAPPER_