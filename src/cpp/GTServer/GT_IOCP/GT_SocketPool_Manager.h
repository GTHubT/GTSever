#ifndef GT_NET_SOCKETPOOL_H_
#define GT_NET_SOCKETPOOL_H_

#include <thread>
#include <atomic>
#include <deque>
#include <vector>
#include <mutex>
#include <memory>
#include <WinSock2.h>

/***************************************************************************************************/
/* this is a socket manager pool and there are two pools for use.                                  */
/* allocate_socket_index_pool_ is used for preallocate and for the first pool to serve the service */
/* closed_socket_reuse_pool_ is the socket which closed and to wait for reuse                      */
/***************************************************************************************************/

namespace GT {

	namespace NET {
		
		class GT_SocketPool_Manager {

		public:
			~GT_SocketPool_Manager();
			static						GT_SocketPool_Manager& GetInstance();
			bool						Initilize();		
			void						DestroyPool();
			void						CloseSockAndPush2ReusedPool(std::shared_ptr<SOCKET>);
			std::shared_ptr<SOCKET>		GetNextUnuseSocket();

		private:
			GT_SocketPool_Manager();
			void		UpdateSocketPool_();
			bool		PreAllocateSocket_();	
			void		ReAllocateSocket4Pool_();
			void		LongTimeWork4CleanClosedSocket_(std::atomic<bool>&, std::mutex&, std::deque < std::shared_ptr< SOCKET> > &);

		private:
			size_t				poolsize_;
			std::thread			clean_thread_;
			static  std::mutex	socket_pool_mutex_;
			std::deque<SOCKET>	socket_pool_;
			std::deque<SOCKET>	tobereuse_socket_pool_;
			std::deque<std::shared_ptr<SOCKET>>	socket_inuse_pool_;
			std::atomic<bool>	end_socket_clean_thread_;


		};
	}
}

#endif // GT_NET_SOCKETPOOL_H_
