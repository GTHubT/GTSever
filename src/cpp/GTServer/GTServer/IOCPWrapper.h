#ifndef _GT_NET_IOCPWRAPPER_H_
#define _GT_NET_IOCPWRAPPER_H_

#include <Windows.h>
#include <WinSock2.h>

namespace GT {

    namespace NET {

        class IOCPWrapper
        {
        public:
            ~IOCPWrapper() {}

            bool Initialize();
            bool Finalize();
            static IOCPWrapper& GetInstance();
            bool CreateNewIoCompletionPort();
            bool BindSocketToCompletionPort();
            HANDLE GetCompletionPort() { return completion_port_; }

        private:
            IOCPWrapper():is_inited_(false){}

        private:
            HANDLE completion_port_;

        private:
            bool is_inited_;

        };
    }
}

#endif // ifndef _GT_NET_IOCPWRAPPER_H_
