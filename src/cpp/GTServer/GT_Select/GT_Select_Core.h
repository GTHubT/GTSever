#pragma once
#ifndef GT_NET_SELECT_CORE_
#define GT_NET_SELECT_CORE_

#include "GT_Definition.h"

#include <thread>
#include <atomic>
#include <functional>

#pragma comment(lib, "Ws2_32.lib")

namespace GT {
    namespace NET {

		typedef void(*internal_call_back)(EVENT_TYPE, PULONG_PTR, const char*, int);

        class GT_Select_Core
        {
        public:
            GT_Select_Core();
            ~GT_Select_Core();

		public:
			bool	Initialize();
			void	StartGTService();
			bool	Finalize();
			void	RegisterCallback(internal_call_back cb);
			void	UnRegisterCallback();
		private:
			void	StopService_();
			void	Select_service_();
			void	CollectResource_();
			void	ProcessAcceptEvent_();
			void	ProcessReadEvent_(SOCKET&);
			void	ProcessWriteEvent_(SOCKET&);
			void	ProcessExpEvent_(SOCKET&);
			void	DispatchEvent_(EVENT_TYPE, PULONG_PTR, char*, size_t);

            void    AddEvent_(EVENT_TYPE, SOCKET);
            void    DelEvent_(EVENT_TYPE, SOCKET);
			void	GrowSet_(EVENT_TYPE);

            void    WakeupSelectThread_();

		private:
			fd_set_pri socketset[3];		/* 0: read set, 1: write set, 2: exp set*/
			int	socket_set_pos_[3];			/* record the position of the used socket in set: 0 for read , 1 for write , 2 for exp */

        private:
			internal_call_back	select_cb_func_;
			SOCKET				listen_socket_;
			std::thread			server_thread_;
			std::atomic_bool	end_thread_;
			bool				service_started_;
			bool				service_inited_;
            int                 udp_port_;
        };
    }
}


#endif