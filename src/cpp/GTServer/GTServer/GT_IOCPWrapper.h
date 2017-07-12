#ifndef GT_NET_IOCPWRAPPER_H_
#define GT_NET_IOCPWRAPPER_H_

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include "GT_SockIOContext.h"

#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

namespace GT {

    namespace NET {

		typedef void(*Ready_Event_Callback) (EVENT_TYPE, int datalen, char* data);
        class GT_IOCPWrapper
        {
        public:
            ~GT_IOCPWrapper() {}
            static GT_IOCPWrapper& GetInstance();

            bool	Initialize();
            bool	Finalize();
            bool	CreateNewIoCompletionPort();
            bool	BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey);
			void	GetCompletionPortStatus(Ready_Event_Callback callback);

        private:
			GT_IOCPWrapper();
			SOCKET	CreateOverlappedSocket_(int af, int type, int protocl);
			bool	InitializeListenSocket_();

        private:
            HANDLE completion_port_;
            SOCKET listen_socket_;

        private:
            bool is_inited_;

        };
    }
}

#endif // ifndef _GT_NET_IOCPWRAPPER_H_
