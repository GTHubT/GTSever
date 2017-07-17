#include "GT_Util_GlogWrapper.h"
#include "GT_Util_OSInfo.h"


namespace GT {

    namespace UTIL {

#define LOG_LOCK_THIS_SCOPE std::lock_guard<std::mutex> lk(log_mutex_);

		bool         GT_Util_GlogWrapper::is_log_initted_ = false;
		GT_LOG_LEVEL GT_Util_GlogWrapper::log_level_ = GT_LOG_LEVEL_OFF;
		std::mutex	 GT_Util_GlogWrapper::log_mutex_;

        GT_Util_GlogWrapper::GT_Util_GlogWrapper():per_log_size_(50)
        {
			log_path_ = "./temp.log";
        }

        GT_Util_GlogWrapper::~GT_Util_GlogWrapper()
        {
        }

		GT_Util_GlogWrapper& GT_Util_GlogWrapper::GetInstance() {
			LOG_LOCK_THIS_SCOPE;

			static GT_Util_GlogWrapper log_instance_;
			return log_instance_;
		}

		bool GT_Util_GlogWrapper::GT_LogInitialize(std::string logname, GT_LOG_LEVEL level, int maxlogsize) {
			LOG_LOCK_THIS_SCOPE;

			if (is_log_initted_)
				return true;

			if (IS_INVALID_LOGLEVEL(level) || maxlogsize <= 0)
				return is_log_initted_;

			per_log_size_ = maxlogsize;
			log_level_ = level;
			google::LogSeverity loglevel_ = GT_Loglevel2GoogleLoglevel_(level);

			log_path_ = GT_Util_OSInfo::GetCurrentFolder() + logname;
			google::InitGoogleLogging("GTServer");
			
			GT_SetLoglevelDestination_();
			GT_SetGlogFlags_();

			is_log_initted_ = true;
			printf("GLOG Environment Init Success!\n");
			return is_log_initted_;
		}

		void GT_Util_GlogWrapper::GT_SetLoglevelDestination_() {
			google::SetLogDestination(GT_Loglevel2GoogleLoglevel_(log_level_), log_path_.c_str());
			// set other log level destination this should be set, otherwise other log level will not be record
			for (auto le = int(GT_LOG_LEVEL_INFO); le <= GT_LOG_LEVEL_OFF; le++) {
				if (le != log_level_)
					google::SetLogDestination(le, "");
			}
		}

		void GT_Util_GlogWrapper::GT_SetGlogFlags_() {
			FLAGS_log_prefix = true;								// include the time and thread id
			FLAGS_logbufsecs = 0;									// flush log to file frequency
			FLAGS_max_log_size = per_log_size_;						// max log size (MB)
			FLAGS_stop_logging_if_full_disk = true;					// if log is full stop write log
			FLAGS_colorlogtostderr = true;							// if have fatal or error log will put it to terminal with color
		}

		bool GT_Util_GlogWrapper::GT_LogUnintialize() {
			LOG_LOCK_THIS_SCOPE;

			if (is_log_initted_)
				google::ShutdownGoogleLogging();
			is_log_initted_ = false;
			return true;
		}

		bool GT_Util_GlogWrapper::GT_SetLoglevel(GT_LOG_LEVEL level) {
			LOG_LOCK_THIS_SCOPE;

			google::LogSeverity loglevel_ = GT_Loglevel2GoogleLoglevel_(level);
			if (IS_INVALID_LOGLEVEL(level) || !is_log_initted_)
				return is_log_initted_;
			log_level_ = level;
			GT_SetLoglevelDestination_();
			return true;
		}

		google::LogSeverity GT_Util_GlogWrapper::GT_Loglevel2GoogleLoglevel_(GT_LOG_LEVEL level) {
			google::LogSeverity googleloglevel_ = google::GLOG_INFO;
			switch (level) {
			case GT_LOG_LEVEL_WARNING:
				googleloglevel_ = google::GLOG_WARNING;
				break;
			case GT_LOG_LEVEL_ERROR:
				googleloglevel_ = google::GLOG_ERROR;
				break;
			default:
				printf("unknown LOGLEVEL just ignore it!\n");
				break;
			}
			return	googleloglevel_;
		}


		void GT_Util_GlogWrapper::GT_WriteLog(std::string logevent, GT_LOG_LEVEL level, const char* filename, int line) {
			if (!is_log_initted_ || level < log_level_ || log_level_ == GT_LOG_LEVEL_OFF) {
				return;
			}

			char line_[32];
			_itoa_s(line, line_, 10);
			logevent = logevent + " [" + std::string(filename) + ":" + std::string(line_) + "]";

			switch (level)
			{
			case GT_LOG_LEVEL_INFO:
				LOG(INFO) << logevent.c_str();
				break;
			case GT_LOG_LEVEL_WARNING:
				LOG(WARNING) << logevent.c_str();
				break;
			case GT_LOG_LEVEL_ERROR:
				LOG(ERROR) << logevent.c_str();
				break;
			default:
				break;
			}
			return;
		}
    }
}
