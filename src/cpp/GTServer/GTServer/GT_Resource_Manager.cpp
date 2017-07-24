#include "GT_Resource_Manager.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"



namespace GT {

	namespace NET {

		GT_Resource_Manager::GT_Resource_Manager():is_enabled_(false), 
												   end_resource_collector_(false),
												   resource_collect_cycle_time_(30000){

		}

		GT_Resource_Manager::~GT_Resource_Manager() {
		}

		GT_Resource_Manager& GT_Resource_Manager::GetInstance() {
			GT_Resource_Manager socket_context_manager_;
			return socket_context_manager_;
		}

		bool GT_Resource_Manager::Initialize() {
			if (is_enabled_) {
				GT_LOG_INFO("Resource manager has already inited!");
				return is_enabled_;
			}

			do {

				/* init SOCKET cache pool manager */
				is_enabled_ = GT_SocketPool_Manager::GetInstance().Initilize();
				if (!is_enabled_) {
					GT_LOG_ERROR("SOCKET cache pool manager init fail! ");
					break;
				}

				/* init IO buffer cache manager */
				is_enabled_ = GT_IOContextBuffer_Manager::GetInstance().Initialize();
				if (!is_enabled_) {
					GT_LOG_ERROR("IO Context buffer manager init fail!");
					break;
				}

				/* init Resource Collector Worker */
				resource_collect_cycle_time_ = GT_READ_CFG_INT("resource_control", "resource_collect_cycle_time", 30000); /* (ms) */
				resource_collector_thread_ = std::move(std::thread(&GT_Resource_Manager::Resource_Collect_Worker_, this, 
														std::bind(&GT_Resource_Manager::Resource_Collect_Func_, this), 
														std::ref(end_resource_collector_),
														std::ref(resource_mutex_),
														std::ref(resource_cv_),
														resource_collect_cycle_time_));
				GT_LOG_INFO("GT Resource Collector Worker Start, Thread id = " << resource_collector_thread_.get_id());
				GT_LOG_INFO("GT Resource Manager Init Success!");

			} while (0);

			return is_enabled_;
		}

		void GT_Resource_Manager::Resource_Collect_Worker_(std::function<void()> func_, 
															std::atomic_bool& end_thread_, 
															std::mutex& source_mutex_,
															std::condition_variable& source_cv_,
															int cycle_time_) {
			while (!end_thread_) {
				std::unique_lock<std::mutex> lk(source_mutex_);
				source_cv_.wait_for(lk, std::chrono::milliseconds(cycle_time_));
				func_();
			}
		}

		void GT_Resource_Manager::Resource_Collect_Func_() {

		}

		void GT_Resource_Manager::Finalize() {
			end_resource_collector_ = true;
			resource_cv_.notify_one();
			if (resource_collector_thread_.joinable()) {
				resource_collector_thread_.join();
				GT_LOG_INFO("Resource Collector Thread Exit!");
			}

		}
	}
}