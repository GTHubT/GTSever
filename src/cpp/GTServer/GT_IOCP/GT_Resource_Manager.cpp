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
            map_key_hash_set_.clear();
		}

		GT_Resource_Manager::~GT_Resource_Manager() {
		}

		GT_Resource_Manager& GT_Resource_Manager::GetInstance() {
			static GT_Resource_Manager socket_context_manager_;
			return socket_context_manager_;
		}

		bool GT_Resource_Manager::Initialize() {
			GT_TRACE_FUNCTION;
			if (is_enabled_) {
				GT_LOG_WARN("Resource manager has already inited!");
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
				//resource_collect_cycle_time_ = GT_READ_CFG_INT("resource_control", "resource_collect_cycle_time", 30000); /* (ms) */
				//resource_collector_thread_ = std::move(std::thread(&GT_Resource_Manager::Resource_Collect_Worker_, this, 
				//										std::bind(&GT_Resource_Manager::Resource_Collect_Func_, this), 
				//										std::ref(end_resource_collector_),
				//										std::ref(resource_collector_mutex_),
				//										std::ref(resource_cv_),
				//										resource_collect_cycle_time_));

				/* init out date connection checker */
				out_date_time_control_ = GT_READ_CFG_INT("server_cfg", "out_date_control", 120) * 1000;
				connect_check_interval_ = GT_READ_CFG_INT("server_cfg","connect_check_interval", 30000) * 1000;
				connect_check_thread_ = std::move(std::thread(&GT_Resource_Manager::ConnectCheckWorker, this,
												  std::bind(&GT_Resource_Manager::ConnectChecker, this),
												  std::ref(connect_check_mutex_),
												  std::ref(connect_check_cv_),
												  std::ref(end_connect_check_ato_),
												  connect_check_interval_));

                printf("GT Resource Collect Worker Start. \n");
				GT_LOG_INFO("GT Resource Collect Worker Start, Thread id = " << connect_check_thread_.get_id());
				GT_LOG_INFO("GT Resource Manager Init Success!");

			} while (0);

			return is_enabled_;
		}


		std::unordered_map<ULONG_PTR, SOCKETCONTEXT_SHAREPTR>& GT_Resource_Manager::GetCompletionKeyCache() {
			GT_RESOURCE_LOCK;
			return completion_key_ptr_cache_;
		}


		SOCKETCONTEXT_SHAREPTR GT_Resource_Manager::GetListenSocketCompletionKey(SOCKET_SHAREPTR sock_ptr) {
			SOCKETCONTEXT_SHAREPTR temp_ptr(new GT_SocketConetxt());
			temp_ptr->SetSocketType(LISTEN_SOCKET);
			temp_ptr->SetContextSocket(sock_ptr);
			return temp_ptr;
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

		void GT_Resource_Manager::ReleaseCompletionKey(SOCKETCONTEXT_SHAREPTR sockcontext_ptr) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Collect Socket Resource!");
			GT_RESOURCE_LOCK;

            /* release socket context IO buffer first */
            std::unordered_map<ULONG_PTR, IO_BUFFER_PTR>& io_ptr_set = sockcontext_ptr->GetIOBufferCache();
            std::for_each(io_ptr_set.begin(), io_ptr_set.end(), [&](auto io_ptr)->void {ReleaseIOBuffer(io_ptr.second);});
            GT_SOCKET_CACHE_MANAGER.CloseSockAndPush2ReusedPool(sockcontext_ptr->GetContextSocketPtr());
		}

		SOCKETCONTEXT_SHAREPTR GT_Resource_Manager::CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr, SOCKET_TYPE type) {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Create new completion key!");
			GT_RESOURCE_LOCK;
			SOCKETCONTEXT_SHAREPTR temp(new GT_SocketConetxt());
			temp->SetContextSocket(sock_ptr);
			temp->SetSocketType(type);
			completion_key_ptr_cache_.insert(std::make_pair((ULONG_PTR)temp.get(), temp));
            map_key_hash_set_.insert((ULONG_PTR)temp.get());
			return temp;
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
			GT_TRACE_FUNCTION;
            std::unique_lock<std::mutex> lk(source_mutex_);	/* this lock is for condition variable */
			while (!end_thread_ && source_cv_.wait_for(lk, std::chrono::milliseconds(cycle_time_)) == std::cv_status::timeout) {
				func_();
			}
			GT_LOG_INFO("Resource Collector worker exit!");
			printf("Resource Collector worker exit!\n");
		}

		void GT_Resource_Manager::Resource_Collect_Func_() { /* this function will be update later */
            GT_LOG_INFO("Collecting Unuse Resource...");

            /* socket resource should be collect in socket pool INUSE cache */
            GT_SOCKET_CACHE_MANAGER.CollectUnuseSocket();
		}

		void GT_Resource_Manager::ConnectChecker() {
            auto key_iter = map_key_hash_set_.begin();
            for (; key_iter != map_key_hash_set_.end();) {
                auto iter = completion_key_ptr_cache_.find(*key_iter);
                if (iter == completion_key_ptr_cache_.end()) {
					++key_iter;
                    continue;
                }
                auto& comp_key = completion_key_ptr_cache_[*key_iter];
				auto d = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - comp_key->GetTimer()).count();
				if (d > out_date_time_control_ && comp_key->GetSocketType() == ACCEPTED_SOCKET) /*connection have too many time uncomunication*/ {
                    if (comp_key->GetCheckTime() == 1) /* first check */
                    {
                        /* send heart beat package */
                        comp_key->IncremCheckTime();
                    }
                    else if (comp_key->GetCheckTime() == 2) { /* second check */
                        comp_key->ResetCheckTime();
                        ReleaseCompletionKey(comp_key);
                        GT_RESOURCE_LOCK;
                        iter->second.reset();
                        completion_key_ptr_cache_.erase(iter);
                        key_iter = map_key_hash_set_.erase(key_iter);
                        GT_LOG_DEBUG("delete the out date completion key!");
                        continue;
                    }
				}
                ++key_iter;
			}
		}

		void GT_Resource_Manager::ConnectCheckWorker(std::function<void()> func,
													std::mutex& mu,
													std::condition_variable& cv,
													std::atomic_bool& end_thread,
													int check_interval) {
			GT_TRACE_FUNCTION;
			std::unique_lock<std::mutex> lk(mu);
			while (!end_thread)
			{
                if (cv.wait_for(lk, std::chrono::microseconds(check_interval)) == std::cv_status::timeout) {
                    func();
                }
			}
			GT_LOG_INFO("Connection check worker exit!");
			printf("Connection check worker exit! \n");
 		}

		void GT_Resource_Manager::Finalize() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("resource manager finalize...");
			//end_resource_collector_ = true;
			//resource_cv_.notify_one();
			//if (resource_collector_thread_.joinable()) {
			//	resource_collector_thread_.join();
			//	GT_LOG_INFO("Resource Collector Thread Exit!");
			//}

			end_connect_check_ato_ = true;
			connect_check_cv_.notify_one();
			if (connect_check_thread_.joinable()) {
				connect_check_thread_.join();
				GT_LOG_INFO("Connection Checker worker exit!");
			}
			ClearResource_();
		}

		void GT_Resource_Manager::CleanCache_() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Clean the completion key cache...");
			std::for_each(completion_key_ptr_cache_.begin(), completion_key_ptr_cache_.end(), [&](auto iter)->void {ReleaseCompletionKey(iter.second); });
			
		}

		void GT_Resource_Manager::ClearResource_() {
			GT_TRACE_FUNCTION;
			GT_LOG_INFO("Clear all resource hold by resource manager!");
			CleanCache_();
            GT_SOCKET_CACHE_MANAGER.DestroyPool();
            GT_IO_BUFFER_CACHE_MANAGER.Finalize();
		}
	}
}