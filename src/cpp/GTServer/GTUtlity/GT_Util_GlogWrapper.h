#ifndef GT_UTIL_LOG_WRAPPER_
#define GT_UTIL_LOG_WRAPPER_

#include "GTServer/GT_Definition.h"


#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#include <string>
#include <mutex>
#include <sstream>



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
			static void GT_WriteLog(std::string logevent, GT_LOG_LEVEL level);

        private:
            GT_Util_GlogWrapper();
			google::LogSeverity GTLoglevel2GoogleLoglevel(GT_LOG_LEVEL);

		private:
			std::string				log_path_;
			static GT_LOG_LEVEL		log_level_;
			static std::mutex		log_mutex_;
			static bool				is_log_initted_;
        };
    }
}


#define GT_LOG_INFO(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_ALL)
#define GT_LOG_WARN(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_WARNING)
#define GT_LOG_ERROR(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_ERROR)
#define GT_LOG_MACRO(logevent, loglevel)    {	\
	std::ostringstream ost;						\
	ost<<logevent;								\
	std::string str = ost.str();				\
	GT::UTIL::GT_Util_GlogWrapper::GT_WriteLog(str, loglevel); \
}

#endif //GT_UTIL_LOG_WRAPPER_