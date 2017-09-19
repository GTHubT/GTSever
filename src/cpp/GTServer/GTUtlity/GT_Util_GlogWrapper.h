#ifndef GT_UTIL_LOG_WRAPPER_
#define GT_UTIL_LOG_WRAPPER_

#include "GTIOCP/GT_Definition.h"


#ifndef GLOG_NO_ABBREVIATED_SEVERITIES
#define GLOG_NO_ABBREVIATED_SEVERITIES
#endif
#include <glog/logging.h>
#include <string>
#include <mutex>
#include <sstream>



namespace GT {
    
    namespace UTIL {

#define IS_INVALID_LOGLEVEL(level) ((level) >= GT_LOG_LEVEL_OFF || (level) < GT_LOG_LEVEL_INFO)

        class GT_Util_GlogWrapper
        {
        public:
            ~GT_Util_GlogWrapper();
			static GT_Util_GlogWrapper& GetInstance();

			bool GT_LogInitialize(std::string logname, GT_LOG_LEVEL level, int logsize);
			bool GT_LogUnintialize();
			bool GT_SetLoglevel(GT_LOG_LEVEL level);
			static void GT_WriteLog(std::string logevent, GT_LOG_LEVEL level, const char* filename, int line);

        private:
            GT_Util_GlogWrapper();
			void	GT_SetGlogFlags_();
			void	GT_SetLoglevelDestination_();
			static  google::LogSeverity GT_Loglevel2GoogleLoglevel_(GT_LOG_LEVEL);

		private:
			int						per_log_size_;
			std::string				log_path_;
			static GT_LOG_LEVEL		log_level_;
			static std::mutex		log_mutex_;
			static bool				is_log_initted_;
        };

    }
}


#define GT_LOG_INFO(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_ALL)
#define GT_LOG_WARN(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_WARNING)
#define GT_LOG_ERROR(logevent)				GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_ERROR)
#define GT_LOG_DEBUG(logevent)              GT_LOG_MACRO(logevent, GT_LOG_LEVEL::GT_LOG_LEVEL_WARNING)
#define GT_LOG_MACRO(logevent, loglevel)    {	\
	std::ostringstream ost;						\
	ost<<logevent;								\
	std::string str = ost.str();				\
	GT::UTIL::GT_Util_GlogWrapper::GT_WriteLog(str, loglevel, __FILE__, __LINE__); \
}

namespace GT {

	namespace UTIL {

		class GT_TraceFunction {
		public:
			GT_TraceFunction(const char* func) :_func(func) {
				GT_LOG_INFO("Entering:>>> " << _func.c_str());
			}
			~GT_TraceFunction() {
				GT_LOG_INFO("Leaving:<<< " << _func.c_str());
			}

		private:
			std::string _func;
		};
	}
}

#define GT_TRACE_FUNCTION					GT::UTIL::GT_TraceFunction trace(__FUNCTION__);

#endif //GT_UTIL_LOG_WRAPPER_