#pragma once
#ifndef GT_NET_SELECT_CORE_
#define GT_NET_SELECT_CORE_

#include "GT_Definition.h"

#include <WinSock2.h>
#include <Windows.h>

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
			void	RegisterCallback(gt_event_callback* cb);
			void	UnRegisterCallback();
		private:
			void	Select_service_();
			void	DispatchEvent_(EVENT_TYPE, ULONG_PTR, char*, size_t);
			gt_event_callback select_cb_func_;
        };
    }
}


#endif