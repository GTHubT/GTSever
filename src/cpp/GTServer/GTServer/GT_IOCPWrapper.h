#ifndef GT_NET_IOCPWRAPPER_H_
#define GT_NET_IOCPWRAPPER_H_

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include "GT_SockIOContext.h"
#include "GT_ThreadPool.h"

#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

namespace GT {

    namespace NET {

        typedef void(*Read_Ready_Event_Callback) (char* data, int datalen);
        typedef void(*Write_Ready_Event_Callback) (char* data, int datalen);

        class GT_IOCPWrapper
        {
        public:
            ~GT_IOCPWrapper() {}
            static GT_IOCPWrapper& GetInstance();

            bool	Initialize();
            bool	StopService();
            void    StartService();
            bool	BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey);
			void	GetCompletionPortStatus();

            void    SetReadEventCallBack(Read_Ready_Event_Callback);
            void    SetWriteEventCallBack(Write_Ready_Event_Callback);

        private:
            GT_IOCPWrapper();
            HANDLE	CreateNewIoCompletionPort_();
			SOCKET	CreateOverlappedSocket_(int af, int type, int protocl);
            void    ProcessAcceptEvent_();
			bool	InitializeListenSocket_();
            void    PostAcceptEvent_();
            void    PostReadEvent_();
            void    PostWriteEvent_();

        private:
            bool            is_inited_;
            HANDLE          completion_port_;
            SOCKET          listen_socket_;
            SOCKADDR_IN     serveraddr;
            GT_ThreadPool   thread_pool_;

        private:
            bool    is_read_callback_setted_;
            bool    is_write_callback_setted_;
            Read_Ready_Event_Callback   read_func_;
            Write_Ready_Event_Callback  write_func_;

        };
    }
}

#endif // ifndef _GT_NET_IOCPWRAPPER_H_
