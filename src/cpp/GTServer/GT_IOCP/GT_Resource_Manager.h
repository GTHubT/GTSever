#ifndef GT_SOCKET_IOCONTEXT_MANAGER_H_
#define GT_SOCKET_IOCONTEXT_MANAGER_H_

#include "GT_SocketContext.h"
#include "GT_SocketPool_Manager.h"
#include "GT_IOContextBuffer_Manager.h"


#include <set>
#include <atomic>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace GT {

	namespace NET {

#ifndef SOCKETCONTEXT_SHAREPTR
#define SOCKETCONTEXT_SHAREPTR	std::shared_ptr<GT_SocketConetxt>
#endif

		class GT_Resource_Manager {

		public:
			~GT_Resource_Manager();

			bool					Initialize();
			void					Finalize();
			SOCKET_SHAREPTR			GetCachedSocket();
			IO_BUFFER_PTR			GetIOContextBuffer();										// overlapped struct
			void					ReleaseIOBuffer(IO_BUFFER_PTR ptr);
			void					ReleaseSocket(SOCKET_SHAREPTR sock_ptr);
			void					SetSocketContexAddr(SOCKETCONTEXT_SHAREPTR s_ptr, SOCKADDR_IN sock_addr);
			void					PushIOEvent2CompletionKey(SOCKETCONTEXT_SHAREPTR sock_ptr, IO_BUFFER_PTR io_ptr);
			SOCKETCONTEXT_SHAREPTR	CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr);			// completion key

			static GT_Resource_Manager& GetInstance();

		private:
			GT_Resource_Manager();
			void	ClearResource_();
			void	Resource_Collect_Func_();
			void	Resource_Collect_Worker_(std::function<void()> func_, std::atomic_bool& end_thread_, std::mutex& resource_lock, std::condition_variable& source_cv, int cycle_time_);

		private:
			bool is_enabled_;
			int	 resource_collect_cycle_time_;
			std::mutex resource_mutex_;
			std::condition_variable resource_cv_;
			std::thread resource_collector_thread_;
			std::atomic_bool end_resource_collector_;
			std::set<SOCKETCONTEXT_SHAREPTR> completion_key_ptr_cache_;
		};
	}
}

#endif // GT_SOCKET_IOCONTEXT_MANAGER_H_
