#include "GT_Util_GlogWrapper.h"




namespace GT {

    namespace UTIL {
		bool GT_Util_GlogWrapper::is_log_initted_ = false;
		GT_LOG_LEVEL GT_Util_GlogWrapper::log_level_ = GT_LOG_LEVEL_OFF;
        GT_Util_GlogWrapper::GT_Util_GlogWrapper()
        {
			log_path_ = "./GT_server.log";
        }

        GT_Util_GlogWrapper::~GT_Util_GlogWrapper()
        {
        }

		GT_Util_GlogWrapper& GT_Util_GlogWrapper::GetInstance() {
			static GT_Util_GlogWrapper log_instance_;
			return log_instance_;
		}

		bool GT_Util_GlogWrapper::GT_LogInitialize(std::string logname, GT_LOG_LEVEL level) {
			if (is_log_initted_)
				return true;

			if (level == GT_LOG_LEVEL::GT_LOG_LEVEL_OFF)
				return is_log_initted_;

			log_level_ = level;
			google::LogSeverity loglevel_ = GTLoglevel2GoogleLoglevel(level);

			log_path_ = "./" + logname + ".log_";
			google::InitGoogleLogging("GTServer");
			google::SetLogDestination(GTLoglevel2GoogleLoglevel(log_level_),log_path_.c_str());
			FLAGS_log_prefix = true;								// include the time and thread id
			FLAGS_logbufsecs = 0;									// flush log to file frequency
			FLAGS_max_log_size = 50;								// max log size (MB)
			FLAGS_stop_logging_if_full_disk = true;					// if log is full stop write log
			FLAGS_colorlogtostderr = true;							// if have fatal or error log will put it to terminal with color

			is_log_initted_ = true;
			printf("GLOG Environment Init Success!\n");
			return is_log_initted_;
		}

		bool GT_Util_GlogWrapper::GT_LogUnintialize() {
			if (is_log_initted_)
				google::ShutdownGoogleLogging();
			is_log_initted_ = false;
			return true;
		}

		bool GT_Util_GlogWrapper::GT_SetLoglevel(GT_LOG_LEVEL level) {
			google::LogSeverity loglevel_ = GTLoglevel2GoogleLoglevel(level);
			if (NULL == loglevel_ || !is_log_initted_)
				return is_log_initted_;
			google::SetLogDestination(log_level_, log_path_.c_str());
			return true;
		}

		google::LogSeverity GT_Util_GlogWrapper::GTLoglevel2GoogleLoglevel(GT_LOG_LEVEL level) {
			google::LogSeverity googleloglevel_ = google::GLOG_INFO;
			switch (level) {
			case GT_LOG_LEVEL::GT_LOG_LEVEL_INFO:
				break;
			case GT_LOG_LEVEL::GT_LOG_LEVEL_WARNING:
				googleloglevel_ = google::GLOG_WARNING;
				break;
			case GT_LOG_LEVEL::GT_LOG_LEVEL_ERROR:
				googleloglevel_ = google::GLOG_ERROR;
				break;
			default:
				printf("unknown LOGLEVEL just ignore it!\n");
				break;
			}
			return	googleloglevel_;
		}


		void GT_Util_GlogWrapper::GT_WriteLog(std::string logevent, GT_LOG_LEVEL level) {
			if (!is_log_initted_ || level > log_level_ || log_level_ == GT_LOG_LEVEL::GT_LOG_LEVEL_OFF) {
				return;
			}

			switch (level)
			{
			case GT_LOG_LEVEL::GT_LOG_LEVEL_INFO:
				logevent = "[INFO] " + logevent;
				LOG(INFO) << logevent.c_str();
				break;
			case GT_LOG_LEVEL::GT_LOG_LEVEL_WARNING:
				logevent = "[WARN] " + logevent;
				LOG(WARNING) << logevent.c_str();
				break;
			case GT_LOG_LEVEL::GT_LOG_LEVEL_ERROR:
				logevent = "[ERROR] " + logevent;
				LOG(ERROR) << logevent.c_str();
				break;
			default:
				break;
			}
			return;
		}
    }
}
