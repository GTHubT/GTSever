#ifndef GT_NET_IOCPWRAPPER_H_
#define GT_NET_IOCPWRAPPER_H_

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include "GTUtlity/GT_Util_ThreadPool.h"
#include "GT_IOContextBuffer_Manager.h"
#include "GT_SocketContext_Manager.h"
#include "GT_SocketPool.h"
#include "GT_Definition.h"

#include <vector>
#include <Windows.h>
#include <WinSock2.h>
#include <MSWSock.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

namespace GT {

    namespace NET {

		typedef void(*Server_Event_Callback_Func) (char* data, int bufferlen);

        class GT_IOCPWrapper
        {
        public:
            ~GT_IOCPWrapper() {}
            static GT_IOCPWrapper& GetInstance();

            bool	Initialize();
            bool	StopService();
            void    StartService();
            bool	BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey);
			void	GetCompletionPortEventStatus();
            void    GetUnuseIOContext();
            void    DispatchEvent2CallBack(IO_EVENT_TYPE event_type);
			void	SetCallBackFunc(IO_EVENT_TYPE type, Server_Event_Callback_Func func);

        private:
            GT_IOCPWrapper();
            HANDLE	CreateNewIoCompletionPort_();
			SOCKET	CreateOverlappedSocket_(int af, int type, int protocl);
            void    ProcessAcceptEvent_();
			bool	InitializeListenSocket_();
            void    PostAcceptEvent_();
            void    PostReadEvent_();
            void    PostWriteEvent_();
			bool	GetAcceptEXFuncAddress_();
			bool	GetAcceptExSockAddrsFuncAddress_();

        private:
            bool                           is_inited_;
            bool                           socket_pool_enable_;
            HANDLE                         completion_port_;
            SOCKET                         listen_socket_;
            SOCKADDR_IN                    serveraddr_;
            GT::UTIL::GT_Util_ThreadPool   thread_pool_;

        private:
			int							index_allocated_used_;
            bool						is_read_callback_setted_;
            bool						is_write_callback_setted_;
			LPFN_ACCEPTEX				paccpetex_;
			LPFN_GETACCEPTEXSOCKADDRS	pgetacceptex_sockaddrs_;
            Server_Event_Callback_Func  read_request_func_;
			Server_Event_Callback_Func	read_complete_func_;
			Server_Event_Callback_Func	write_compete_func_;
            Server_Event_Callback_Func  write_request_func_;

        };
    }
}

#endif // ifndef _GT_NET_IOCPWRAPPER_H_
