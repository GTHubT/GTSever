#ifndef GT_SOCKET_IOCONTEXT_MANAGER_H_
#define GT_SOCKET_IOCONTEXT_MANAGER_H_

#include "GT_SocketContext.h"
#include "GT_SocketPool_Manager.h"
#include "GT_IOContextBuffer_Manager.h"

#include <atomic>
#include <thread>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
			void					ReleaseCompletionKey(SOCKETCONTEXT_SHAREPTR sock_ptr, bool private_clean = false);
			void					SetSocketContexAddr(SOCKETCONTEXT_SHAREPTR s_ptr, SOCKADDR_IN sock_addr);
			void					PushIOEvent2CompletionKey(SOCKETCONTEXT_SHAREPTR sock_ptr, IO_BUFFER_PTR io_ptr);
			SOCKETCONTEXT_SHAREPTR	CreateNewSocketContext(SOCKET_SHAREPTR sock_ptr, SOCKET_TYPE type = NULL_SOCKET);			// completion key
			SOCKETCONTEXT_SHAREPTR GetListenSocketCompletionKey(SOCKET_SHAREPTR sock_ptr);
			std::unordered_map<ULONG_PTR, SOCKETCONTEXT_SHAREPTR>& GetCompletionKeyCache();
			SOCKETCONTEXT_SHAREPTR GetCompletionKeyPtr(ULONG_PTR);

			static GT_Resource_Manager& GetInstance();

		private:
			GT_Resource_Manager();
			void	ClearResource_();
			void	CleanCache_();
			void	Resource_Collect_Func_();
			void	Resource_Collect_Worker_(std::function<void()> func_, std::atomic_bool& end_thread_, std::mutex& resource_lock, std::condition_variable& source_cv, int cycle_time_);

			void	ResourceCollector(); /* collect resource and check the connection by timer control, if the timer is expired, will close the connection */
			void	Collect_Worker_(std::function<void()> func, std::mutex& mu, std::condition_variable& cv, std::atomic_bool& end_thread, int check_interval);
		private:
			bool is_enabled_;
			std::mutex resource_global_lock_;

			int	 resource_collect_cycle_time_;
			std::mutex resource_collector_mutex_;
			std::condition_variable resource_cv_;
			std::thread resource_collector_thread_;
			std::atomic_bool end_resource_collector_;

			std::mutex connect_check_mutex_;
			std::condition_variable connect_check_cv_;
			std::atomic_bool end_connect_check_ato_;
			int out_date_time_control_;
			int connect_check_interval_;
			std::thread connect_check_thread_;
            std::unordered_set<ULONG_PTR> completion_key_address_hash_set_;
			std::unordered_map<ULONG_PTR, SOCKETCONTEXT_SHAREPTR> completion_key_ptr_cache_; /* each thread have its own cache */
		};
	}
}

#endif // GT_SOCKET_IOCONTEXT_MANAGER_H_
