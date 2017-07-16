#ifndef GT_NET_SOCKETPOOL_H_
#define GT_NET_SOCKETPOOL_H_

#include <deque>
#include <mutex>
#include <WinSock2.h>

/***************************************************************************************************/
/* this is a socket manager pool and there are two pools for use.                                  */
/* allocate_socket_index_pool_ is used for preallocate and for the first pool to serve the service */
/* closed_socket_reuse_pool_ is the socket which closed and to wait for reuse                      */
/***************************************************************************************************/

namespace GT {

	namespace NET {
		
		class GT_SocketPool {

		public:
			~GT_SocketPool();
			static	GT_SocketPool& GetInstance();
			bool	PreAllocateSocket();						
			SOCKET  GetNextUnuseSocket();
			void	DestroyPool();
			void	CloseSockAndPush2ReusedPool(SOCKET);	

		private:
			GT_SocketPool();
			void	UpdateSocketPool_();
			void	ReAllocateSocket4Pool_();

		private:
			static  std::mutex	socket_pool_mutex_;
			size_t	poolsize_;
			std::deque<SOCKET> socket_pool_;
			std::deque<SOCKET> socket_inuse_pool_;
			std::deque<SOCKET> tobereuse_socket_pool_;


		};
	}
}

#endif // GT_NET_SOCKETPOOL_H_
