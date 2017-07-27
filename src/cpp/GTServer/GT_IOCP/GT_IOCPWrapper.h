#ifndef GT_NET_IOCPWRAPPER_H_
#define GT_NET_IOCPWRAPPER_H_

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include "GTUtlity/GT_Util_ThreadPool.h"
#include "GT_Resource_Manager.h"
#include "GT_Definition.h"

#include <vector>
#include <memory>
#include <Windows.h>
#include <WinSock2.h>
#include <MSWSock.h>
#include <functional>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

namespace GT {

    namespace NET {

        class GT_IOCPWrapper
        {
        public:
            ~GT_IOCPWrapper() {}
            static GT_IOCPWrapper& GetInstance();

            bool	Initialize();
            bool	StopService();
            void    StartService(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR)>& call_back_func_);
            bool	BindSocketToCompletionPort(SOCKET_SHAREPTR s_ptr, ULONG_PTR completionkey);
			void	GetCompletionPortEventStatus(std::function<void(IO_EVENT_TYPE, SOCKETCONTEXT_SHAREPTR, IO_BUFFER_PTR)>& call_back_);

			void    PostWriteRequestEvent(SOCKETCONTEXT_SHAREPTR completion_key_, IO_BUFFER_PTR io_event_);
        private:
            GT_IOCPWrapper();
            HANDLE	CreateNewIoCompletionPort_();
			SOCKET_SHAREPTR	CreateOverlappedSocket_(int af, int type, int protocl);
            bool	InitializeListenSocket_();
            void    PrePostAcceptEvent_();
            void    PostAnotherAcceptEvent_();
            void    ProcessAcceptEvent_(IO_BUFFER_PTR io_context);
            void    PostReadRequestEvent_(SOCKETCONTEXT_SHAREPTR completion_key_);
			bool	GetAcceptEXFuncAddress_();
			bool	GetAcceptExSockAddrsFuncAddress_();

        private:
            bool                           is_inited_;
            bool                           completionkey_ioevent_manager_enable_;
            HANDLE                         completion_port_;
            SOCKET_SHAREPTR                listen_socket_ptr_;
			SOCKETCONTEXT_SHAREPTR		   accept_socket_completion_key_;
            SOCKADDR_IN                    serveraddr_;
            GT::UTIL::GT_Util_ThreadPool   thread_pool_;

        private:
			int							index_allocated_used_;
            bool						is_read_callback_setted_;
            bool						is_write_callback_setted_;
			LPFN_ACCEPTEX				paccpetex_func_;
			LPFN_GETACCEPTEXSOCKADDRS	pgetacceptex_sockaddrs_func_;

        };
    }
}

#endif // ifndef _GT_NET_IOCPWRAPPER_H_
