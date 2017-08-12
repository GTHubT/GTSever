#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"
#include "GT_SocketPool_Manager.h"
#include "GTUtlity/GT_Util_OSInfo.h"

#include <random>
#include <chrono>
#include <algorithm>

/*  CACHE:  if the cache is not enough, manager should allocate new item, but should allocate to the new_allocate_cache_ not the in use cache */
/*  if the invoker did not get the item from context, it can new a item, and push the item to the is use cache directly, and the new_allocate_cache_ should merge to the unuse cache at a proper time*/
/*  AND A QUESTION: Why the cache use so quick, does the collector do not work??*/

using namespace GT::UTIL;
namespace GT {

	namespace NET {
#define SOCKETPOOL_LOCK_THIS_SCOPE	std::lock_guard<std::mutex> lk(GT_SocketPool_Manager::socket_pool_mutex_);

		std::mutex GT_SocketPool_Manager::socket_pool_mutex_;

		GT_SocketPool_Manager::GT_SocketPool_Manager():poolsize_(0), end_socket_clean_thread_(false){
			shared_socket_pool_.clear();
			socket_inuse_pool_.clear();
			tobereuse_socket_pool_.clear();
		}

		GT_SocketPool_Manager::~GT_SocketPool_Manager() {

		}

		GT_SocketPool_Manager& GT_SocketPool_Manager::GetInstance() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			static GT_SocketPool_Manager socketpool_;
			return socketpool_;
		}


		bool GT_SocketPool_Manager::Initilize() {
			GT_TRACE_FUNCTION;
			SOCKETPOOL_LOCK_THIS_SCOPE;

			bool ret = PreAllocateSocket_();

            /*std::function<void()> threadfunc = std::bind(&GT_SocketPool_Manager::LongTimeWork4CleanClosedSocket_, this,
                                                std::ref(end_socket_clean_thread_),
                                                std::ref(socket_pool_mutex_),
                                                std::ref(socket_inuse_pool_));
			clean_thread_ = std::thread(threadfunc);*/
            GT_LOG_INFO("socket pool initialize success!");
			return true;
		}


		/* if preallocate failed, server will start use default mode: create a new socket before send a accept event */
		bool GT_SocketPool_Manager::PreAllocateSocket_() {
			GT_TRACE_FUNCTION;

			int pre_allocate_num = GT_READ_CFG_INT("socket_pool_cfg", "pre_allocate_socket_num", 3000);
			if (pre_allocate_num <= 0) {
				GT_LOG_ERROR("illegal pool size!");
				return false;
			}
			
			while (poolsize_ < pre_allocate_num) {
				shared_socket_pool_.insert(SOCKET_SHARED_PTR(new (SOCKET)(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))));
				++ poolsize_;
			}

			return true;
		}


		std::shared_ptr<SOCKET> GT_SocketPool_Manager::GetNextUnuseSocket() {
			SOCKETPOOL_LOCK_THIS_SCOPE;
            GT_LOG_WARN("inuse socket pool size = " << socket_inuse_pool_.size());
            GT_LOG_WARN("socket pool size = " << shared_socket_pool_.size());
			if (shared_socket_pool_.size() < GT_READ_CFG_INT("socket_pool_cfg", "size_to_rellocate", 30)) {
				UpdateSocketPool_();
			}
			if (shared_socket_pool_.size() > 0) {
				auto iter = shared_socket_pool_.begin();
				socket_inuse_pool_.insert(std::make_pair((ULONG_PTR)((*iter).get()),*iter));
				SOCKET_SHARED_PTR temp_ptr = socket_inuse_pool_[(ULONG_PTR)((*iter).get())];
				shared_socket_pool_.erase(iter);
				return temp_ptr;
            }
            else {
                std::shared_ptr<SOCKET> temp_ptr(new (SOCKET)(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)));
                socket_inuse_pool_.insert(std::make_pair((ULONG_PTR)temp_ptr.get(), temp_ptr));
                return temp_ptr;
            }
			
		}

		/* if the socket pool is not enough, there two action to be done:1. move reuse pool to socket pool back  
			2. check reuse pool size if size < reallocate size will start reallcate mechanism */
		void GT_SocketPool_Manager::UpdateSocketPool_() {
			GT_TRACE_FUNCTION;

			//std::for_each(tobereuse_socket_pool_.begin(), tobereuse_socket_pool_.end(), [&](auto iter) {shared_socket_pool_.insert(iter.second); });

			if (tobereuse_socket_pool_.size() < GT_READ_CFG_INT("socket_pool_cfg", "size_to_rellocate", 30)) {
				ReAllocateSocket4Pool_();
			}

			//tobereuse_socket_pool_.clear();
		}

		/* if the socket pool is not enough, will call ReAllocateSocket4Pool to reallocate sockets for further use */
		void GT_SocketPool_Manager::ReAllocateSocket4Pool_() {
			size_t newsize_ = poolsize_ + GT_READ_CFG_INT("socket_pool_cfg", "reallocate_socket_num_pertime", 300);
			while (poolsize_ < newsize_) {
				shared_socket_pool_.insert(SOCKET_SHARED_PTR(new (SOCKET)(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))));
				++poolsize_;
			}
		}

		void GT_SocketPool_Manager::DestroyPool() {
			GT_TRACE_FUNCTION;
			SOCKETPOOL_LOCK_THIS_SCOPE;

			//end_socket_clean_thread_ = true;
			//if (clean_thread_.joinable()) {
			//	clean_thread_.join();
			//}

			std::for_each(socket_inuse_pool_.begin(), socket_inuse_pool_.end(), [] (auto iter){ closesocket(*iter.second); });

			shared_socket_pool_.clear();
			socket_inuse_pool_.clear();
			//tobereuse_socket_pool_.clear();
		}

		void GT_SocketPool_Manager::CloseSockAndPush2ReusedPool(std::shared_ptr<SOCKET> sock_ptr) {
			SOCKETPOOL_LOCK_THIS_SCOPE;
			if (sock_ptr != nullptr) {
				closesocket(*sock_ptr);
				auto iter = socket_inuse_pool_.find((ULONG_PTR)sock_ptr.get());
				if (iter != socket_inuse_pool_.end()) {
					iter->second.reset();
					socket_inuse_pool_.erase(iter);
				}
				else {
					GT_LOG_WARN("did not find the socket in the socket inuse cache!");
				}
				sock_ptr.reset(); /* release socket resource */
				/*if (GT::UTIL::GT_Util_OSInfo::GetRandomInt() > 8) {
					GT_LOG_WARN("to be use socket pool size = " << tobereuse_socket_pool_.size());
					tobereuse_socket_pool_.insert(std::make_pair((ULONG_PTR)(sock_ptr.get()), sock_ptr));
				}*/
			}
		}

		/* will not use for the cache change to hash map */
        void GT_SocketPool_Manager::CollectUnuseSocket() {
            GT_LOG_INFO("Collect Unuse Socket!");
            for (auto iter = socket_inuse_pool_.begin(); iter != socket_inuse_pool_.end();) { /* this collector is to collect the in use socket cache who has already closed */
                if (*iter->second == INVALID_SOCKET) {
                    SOCKETPOOL_LOCK_THIS_SCOPE;
					GT_LOG_INFO("release invalid socket");
                    iter = socket_inuse_pool_.erase(iter);
                }
                else {
                    ++iter;
                }
            }
        }

		/* this will be replace by GT_Resouce_Manager Resource Collector thread */
		void GT_SocketPool_Manager::LongTimeWork4CleanClosedSocket_(std::atomic<bool>& end_thread_,std::mutex& socket_lock_, std::deque<std::shared_ptr<SOCKET>>& inuse_pool_) {
			while (!end_thread_) {
				std::this_thread::sleep_for(std::chrono::milliseconds(30000));
				for (auto iter = inuse_pool_.begin(); iter < inuse_pool_.end();) {
					if (**iter == INVALID_SOCKET) {
						std::lock_guard<std::mutex> lk(socket_lock_);
						iter = inuse_pool_.erase(iter);
					}
					else {
						++iter;
					}
				}
			}
		}
	}
}