#include "GT_IOCPWrapper.h"


namespace GT {

    namespace NET {

        bool GT_IOCPWrapper::Initialize() {
            return true;
        }

        bool GT_IOCPWrapper::Finalize() {
            return true;
        }

        GT_IOCPWrapper& GT_IOCPWrapper::GetInstance() {
            static GT_IOCPWrapper iocpinstance;
            return iocpinstance;
        }

        bool GT_IOCPWrapper::CreateNewIoCompletionPort() {
            return true;
        }

        bool GT_IOCPWrapper::BindSocketToCompletionPort(SOCKET s, ULONG_PTR completionkey) {
            return true;
        }

    }
}
