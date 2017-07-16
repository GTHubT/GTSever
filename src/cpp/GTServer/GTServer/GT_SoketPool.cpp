#include "GTUtlity/GT_Util_GlogWrapper.h"
#include "GT_SocketPool.h"
#include "GT_Cfg.h"

#include <algorithm>

namespace GT {

	namespace NET {
#define SOCKETPOOL_LOCK_THIS_SCOPE	std::lock_guard<std::mutex> lk(socket_pool_mutex_);

		std::mutex GT_SocketPool::socket_pool_mutex_;

		GT_SocketPool::GT_SocketPool():poolsize_(0){
			socket_pool_.clear();
			socket_inuse_pool_.clear();
			tobereuse_socket_pool_.clear();
		}

		GT_SocketPool::~GT_SocketPool() {

		}

		GT_SocketPool& GT_SocketPool::GetInstance() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			static GT_SocketPool socketpool_;
			return socketpool_;
		}


		/*if preallocate failed, server will start use default mode: create a new socket before send a accept event */
		bool GT_SocketPool::PreAllocateSocket() {
			SOCKETPOOL_LOCK_THIS_SCOPE;
			GT_TRACE_FUNCTION;

			if (PRE_ALLOCATE_SOCKET_NUM <= 0) {
				GT_LOG_ERROR("illegal pool size!");
				return false;
			}
			
			while (poolsize_ < PRE_ALLOCATE_SOCKET_NUM) {
				TCP_MODE_ENABLE ? socket_pool_.push_back(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) :
								  socket_pool_.push_back(WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED));
				++ poolsize_;
			}

			return true;
		}


		SOCKET GT_SocketPool::GetNextUnuseSocket() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			if (socket_pool_.size() < SIZEOF_USEFULL_SOCKET) {
				UpdateSocketPool_();
			}
			SOCKET s = INVALID_SOCKET;
			if (socket_pool_.size() > 0) {
				s = socket_pool_.front();
				socket_pool_.pop_front();
			}
				
			return s;
		}

		/* if the socket pool is not enough, there two action to be done:1. move reuse pool to socket pool back  
			2. check reuse pool size if size < reallocate size will start reallcate mechanism */
		void GT_SocketPool::UpdateSocketPool_() {
			GT_TRACE_FUNCTION;

			std::for_each(tobereuse_socket_pool_.begin(), tobereuse_socket_pool_.end(), [=](auto iter) {socket_pool_.push_back(iter); });

			if (tobereuse_socket_pool_.size() < SIZEOF_USEFULL_SOCKET) {
				ReAllocateSocket4Pool_();
			}

			tobereuse_socket_pool_.clear();
		}

		/* if the socket pool is not enough, will call ReAllocateSocket4Pool to reallocate sockets for further use */
		void GT_SocketPool::ReAllocateSocket4Pool_() {
			SOCKETPOOL_LOCK_THIS_SCOPE;

			size_t newsize_ = poolsize_ + REALLOCATE_SOKET_PER_SIZE;
			while (poolsize_ < newsize_) {
				TCP_MODE_ENABLE ? socket_pool_.push_back(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) :
								  socket_pool_.push_back(WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED));
				++ poolsize_;
			}
		}
	}
}