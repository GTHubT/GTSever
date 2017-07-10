#include "IOCPWrapper.h"


namespace GT {

    namespace NET {

        bool IOCPWrapper::Initialize() {
            return true;
        }

        bool IOCPWrapper::Finalize() {
            return true;
        }

        IOCPWrapper& IOCPWrapper::GetInstance() {
            static IOCPWrapper iocpinstance;
            return iocpinstance;
        }

        bool IOCPWrapper::CreateNewIoCompletionPort() {
            return true;
        }

        bool IOCPWrapper::BindSocketToCompletionPort() {
            return true;
        }

    }
}
