#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GTUtlity/GT_Util_CfgHelper.h"
#include "GT_SocketPool_Manager.h"

#include <chrono>
#include <algorithm>

using namespace GT::UTIL;
namespace GT {

	namespace NET {
#define SOCKETPOOL_LOCK_THIS_SCOPE	std::lock_guard<std::mutex> lk(socket_pool_mutex_);

		std::mutex GT_SocketPool_Manager::socket_pool_mutex_;

		GT_SocketPool_Manager::GT_SocketPool_Manager():poolsize_(0), end_socket_clean_thread_(false){
			socket_pool_.clear();
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
				GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) ? socket_pool_.push_back(std::move(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) :
																	   socket_pool_.push_back(std::move(WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED)));
				++ poolsize_;
			}

			return true;
		}


		std::shared_ptr<SOCKET> GT_SocketPool_Manager::GetNextUnuseSocket() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			if (socket_pool_.size() < GT_READ_CFG_INT("socket_pool_cfg", "size_to_rellocate", 30)) {
				UpdateSocketPool_();
			}
			if (socket_pool_.size() > 0) {
				socket_inuse_pool_.push_back(std::shared_ptr<SOCKET>(new SOCKET(std::move(socket_pool_.front()))));
				socket_pool_.pop_front();	
				return socket_inuse_pool_.back();
			}
			return nullptr;
			
		}

		/* if the socket pool is not enough, there two action to be done:1. move reuse pool to socket pool back  
			2. check reuse pool size if size < reallocate size will start reallcate mechanism */
		void GT_SocketPool_Manager::UpdateSocketPool_() {
			GT_TRACE_FUNCTION;

			std::for_each(tobereuse_socket_pool_.begin(), tobereuse_socket_pool_.end(), [&](auto iter) {socket_pool_.push_back(std::move(iter)); });

			if (tobereuse_socket_pool_.size() < GT_READ_CFG_INT("socket_pool_cfg", "size_to_rellocate", 30)) {
				ReAllocateSocket4Pool_();
			}

			tobereuse_socket_pool_.clear();
		}

		/* if the socket pool is not enough, will call ReAllocateSocket4Pool to reallocate sockets for further use */
		void GT_SocketPool_Manager::ReAllocateSocket4Pool_() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			size_t newsize_ = poolsize_ + GT_READ_CFG_INT("socket_pool_cfg", "reallocate_socket_num_pertime", 300);
			while (poolsize_ < newsize_) {
				GT_READ_CFG_BOOL("server_cfg", "enable_tcp_mode", 1) ? socket_pool_.push_back(std::move(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) :
																	   socket_pool_.push_back(std::move(WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED)));
				++ poolsize_;
			}
		}

		void GT_SocketPool_Manager::DestroyPool() {
			GT_TRACE_FUNCTION;
			SOCKETPOOL_LOCK_THIS_SCOPE;

			end_socket_clean_thread_ = true;
			if (clean_thread_.joinable()) {
				clean_thread_.join();
			}

			std::for_each(socket_inuse_pool_.begin(), socket_inuse_pool_.end(), [] (auto iter){ closesocket(*iter); });

			socket_pool_.clear();
			socket_inuse_pool_.clear();
			tobereuse_socket_pool_.clear();
		}

		void GT_SocketPool_Manager::CloseSockAndPush2ReusedPool(std::shared_ptr<SOCKET> sock_ptr) {
			SOCKETPOOL_LOCK_THIS_SCOPE;
			closesocket(*sock_ptr);
			tobereuse_socket_pool_.push_back(std::move(*sock_ptr));
			*sock_ptr = INVALID_SOCKET;											// a assumption, I think start a new thread to clean the INVAID_SOCKET maybe a way for performance
			//socket_inuse_pool_.erase(sock);								// FIX ME: for performance it is not reasonable for search whole inuse pool to delete the closed sock
		}


        void GT_SocketPool_Manager::CollectUnuseSocket() {
            GT_LOG_INFO("Collect Unuse Socket!");
            for (auto iter = socket_inuse_pool_.begin(); iter < socket_inuse_pool_.end();) {
                if (**iter == INVALID_SOCKET) {
                    SOCKETPOOL_LOCK_THIS_SCOPE;
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