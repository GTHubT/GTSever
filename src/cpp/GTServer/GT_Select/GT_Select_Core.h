#pragma once
#ifndef GT_NET_SELECT_CORE_
#define GT_NET_SELECT_CORE_

#include "GT_Definition.h"

#include <WinSock2.h>
#include <Windows.h>
#include <thread>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

namespace GT {
    namespace NET {

        class GT_Select_Core
        {
        public:
            GT_Select_Core();
            ~GT_Select_Core();

		public:
			bool	Initialize();
			void	StartService();
			void	StopService();
			bool	Finalize();
			void	RegisterCallback(gt_event_callback cb);
			void	UnRegisterCallback();
		private:
			void	Select_service_();
			void	DispatchEvent_(EVENT_TYPE, ULONG_PTR, char*, size_t);

            void    AddEvent_(EVENT_TYPE, SOCKET);
            void    DelEvent_(EVENT_TYPE, SOCKET, int index);
			void	GrowSet_(EVENT_TYPE);

		private:
			fd_set_pri socketset[3];		/* 0: read set, 1: write set, 2: exp set*/
			int	socket_set_pos_[3];			/* record the position of the used socket in set: 0 for read , 1 for write , 2 for exp */

        private:
			gt_event_callback	select_cb_func_;
			SOCKET				listen_socket_;
			std::thread			server_thread_;
			std::atomic_bool	end_thread_;
        };
    }
}


#endif