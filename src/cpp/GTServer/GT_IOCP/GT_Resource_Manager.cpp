#include "GT_Resource_Manager.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"

#include <algorithm>

namespace GT {

	namespace NET {

#ifndef GT_SOCKET_CACHE_MANAGER
#define GT_SOCKET_CACHE_MANAGER		GT_SocketPool_Manager::GetInstance()
#endif

#ifndef GT_IO_BUFFER_CACHE_MANAGER
#define GT_IO_BUFFER_CACHE_MANAGER	GT_IOContextBuffer_Manager::GetInstance()
#endif

#ifndef GT_RESOURCE_LOCK
#define GT_RESOURCE_LOCK	std::lock_guard<std::mutex> lk(resource_global_lock_);
#endif

		GT_Resource_Manager::GT_Resource_Manager():is_enabled_(false), 
												   end_resource_collector_(false),
												   resource_collect_cycle_time_(30000),
												   out_date_time_control_(100*1000),
												   end_connect_check_ato_(false),
												   connect_check_interval_(20000){
			completion_key_ptr_cache_.clear();

		}

		GT_Resource_Manager::~GT_Resource_Manager() {
		}

		GT_Resource_Manager& GT_Resource_Manager::GetInstance() {
			GT_Resource_Manager socket_context_manager_;
			return socket_context_manager_;
		}

		bool GT_Resource_Manager::Initialize() {
			GT_TRACE_FUNCTION;
			if (is_enabled_) {
				GT_LOG_INFO("Resource manager has already inited!");
				return is_enabled_;
			}

			do {

				/* init SOCKET cache pool manager */
				is_enabled_ = GT_SOCKET_CACHE_MANAGER.Initilize();
				if (!is_enabled_) {
					GT_LOG_ERROR("SOCKET cache pool manager init fail! ");
					break;
				}

				/* init IO buffer cache manager */
				is_enabled_ = GT_IO_BUFFER_CACHE_MANAGER.Initialize();
				if (!is_enabled_) {
					GT_LOG_ERROR("IO Context buffer manager init fail!");
					break;
				}

				/* init Resource Collector Worker */
				resource_collect_cycle_time_ = GT_READ_CFG_INT("resource_control", "resource_collect_cycle_time", 30000); /* (ms) */
				resource_collector_thread_ = std::move(std::thread(&GT_Resource_Manager::Resource_Collect_Worker_, this, 
														std::bind(&GT_Resource_Manager::Resource_Collect_Func_, this), 
														std::ref(end_resource_collector_),
														std::ref(resource_collector_mutex_),
														std::ref(resource_cv_),
														resource_collect_cycle_time_));

				out_date_time_control_ = GT_READ_CFG_INT("server_cfg", "out_date_control", 120) * 1000;
				connect_check_interval_ = GT_READ_CFG_INT("server_cfg","connect_check_interval", 30000);
				connect_check_thread_ = std::move(std::thread(&GT_Resource_Manager::ConnectCheckWorker, this,
												  std::bind(&GT_Resource_Manager::ConnectChecker, this),
												  std::ref(connect_check_mutex_),
												  std::ref(connect_check_cv_),
												  std::ref(end_connect_check_ato_),
												  connect_check_interval_));


				GT_LOG_INFO("GT Resource Collector Worker Start, Thread id = " << resource_collector_thread_.get_id());
				GT_LOG_INFO("GT Resource Manager Init Success!");

			} while (0);

			return is_enabled_;
		}

		SOCKET_SHAREPTR GT_Resource_Manager::GetCachedSocket() {
			GT_LOG_INFO("Get New sokcet from cache!");
			return GT_SOCKET_CACHE_MANAGER.GetNextUnuseSocket();
		}

		IO_BUFFER_PTR GT_Resource_Manager::GetIOContextBuffer() {
			GT_LOG_INFO("Get new IO context buffer for socket!");
			return GT_IO_BUFFER_CACHE_MANAGER.GetNextIOBufferPtr();
		}

		void GT_Resource_Manager::ReleaseIOBuffer(IO_BUFFER_PTR ptr) {
			GT_LOG_INFO("Collect IO Buffer Resource!");
            GT_IO_BUFFER_CACHE_MANAGER.ReleaseIOBuffer(ptr);
		}

		void GT_Resource_Manager::ReleaseCompletionKey(SOCKETCONTEXT_SHAREPTR& sockcontext_ptr) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Collect Socket Resource!");
			GT_RESOURCE_LOCK;

            /* release socket context IO buffer first */
			closesocket(*(sockcontext_ptr->GetContextSocketPtr()));
            std::set<IO_BUFFER_PTR> io_ptr_set = sockcontext_ptr->GetIOBufferCache();
            std::for_each(io_ptr_set.begin(), io_ptr_set.end(), [&](auto io_ptr)->void {ReleaseIOBuffer(io_ptr);});
            GT_SOCKET_CACHE_MANAGER.CloseSockAndPush2ReusedPool(sockcontext_ptr->GetContextSocketPtr());
			RemoveCompletionKeyFromCache(sockcontext_ptr);
		}

		SOCKETCONTEXT_SHAREPTR GT_Resource_Manager::CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Create new completion key!");
			GT_RESOURCE_LOCK;
			SOCKETCONTEXT_SHAREPTR temp(new GT_SocketConetxt());
			temp->SetContextSocket(sock_ptr);
			completion_key_ptr_cache_.insert(temp);
			return temp;
		}

		void GT_Resource_Manager::RemoveCompletionKeyFromCache(SOCKETCONTEXT_SHAREPTR& com_ptr) {
			GT_TRACE_FUNCTION;

			std::set<SOCKETCONTEXT_SHAREPTR>::iterator del_iter = completion_key_ptr_cache_.find(com_ptr);
			if (del_iter != completion_key_ptr_cache_.end()) {
				GT_LOG_DEBUG("delete the out date completion key!");
				completion_key_ptr_cache_.erase(del_iter);
			}
			else
			{
				GT_LOG_DEBUG("did not find the completion key that need remove from the cache...");
			}
		}

		void GT_Resource_Manager::PushIOEvent2CompletionKey(SOCKETCONTEXT_SHAREPTR sock_context_ptr, IO_BUFFER_PTR io_ptr) {
			GT_LOG_INFO("Associate IO Buffer with completion key");
            sock_context_ptr->AddIOContext2Cache(io_ptr);
		}

		void GT_Resource_Manager::SetSocketContexAddr(SOCKETCONTEXT_SHAREPTR s_ptr, SOCKADDR_IN sock_addr) {
			GT_LOG_INFO("Set socket addrress!");
			s_ptr->SetContextSocketAddr(sock_addr);
		}

		void GT_Resource_Manager::Resource_Collect_Worker_(std::function<void()> func_, 
															std::atomic_bool& end_thread_, 
															std::mutex& source_mutex_,
															std::condition_variable& source_cv_,
															int cycle_time_) {

            std::unique_lock<std::mutex> lk(source_mutex_);	/* this lock is for condition variable */
			while (!end_thread_ && source_cv_.wait_for(lk, std::chrono::milliseconds(cycle_time_)) == std::cv_status::timeout) {
				func_();
			}
		}

		void GT_Resource_Manager::Resource_Collect_Func_() { /* this function will be update later */
            GT_LOG_INFO("Collecting Unuse Resource...");

            /* socket resource should be collect in socket pool INUSE cache */
            GT_SOCKET_CACHE_MANAGER.CollectUnuseSocket();
		}

		void GT_Resource_Manager::ConnectChecker() {
			GT_TRACE_FUNCTION;
			for (auto iter : completion_key_ptr_cache_) {
				if (std::chrono::system_clock::now() - iter->GetTimer() > std::chrono::microseconds(out_date_time_control_)) /*connection have too many time uncomunication*/  {
					ReleaseCompletionKey(iter);
				}
			}
		}

		void GT_Resource_Manager::ConnectCheckWorker(std::function<void()> func,
													std::mutex& mu,
													std::condition_variable& cv,
													std::atomic_bool& end_thread,
													int check_interval) {
			std::unique_lock<std::mutex> lk(mu);
			while (!end_thread && cv.wait_for(lk, std::chrono::microseconds(check_interval)) == std::cv_status::timeout)
			{
				func();
			}
 		}

		void GT_Resource_Manager::Finalize() {
			GT_TRACE_FUNCTION;
			end_resource_collector_ = true;
			resource_cv_.notify_one();
			if (resource_collector_thread_.joinable()) {
				resource_collector_thread_.join();
				GT_LOG_INFO("Resource Collector Thread Exit!");
			}

			end_connect_check_ato_ = true;
			connect_check_cv_.notify_one();
			if (connect_check_thread_.joinable()) {
				connect_check_thread_.join();
				GT_LOG_INFO("connect check worker exit!");
			}
			ClearResource_();
		}

		void GT_Resource_Manager::ClearResource_() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Clear all resource hold by resource manager!");
            GT_SOCKET_CACHE_MANAGER.DestroyPool();
            GT_IO_BUFFER_CACHE_MANAGER.Finalize();
		}
	}
}