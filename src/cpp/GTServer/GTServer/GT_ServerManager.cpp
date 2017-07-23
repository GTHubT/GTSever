#include "GT_ServerManager.h"

namespace GT {

    namespace NET {

        GT_ServerManager::GT_ServerManager()
        {
        }


        GT_ServerManager::~GT_ServerManager()
        {
        }


		void GT_ServerManager::RegisterServerCallBack(IO_EVENT_TYPE type, void(*func)(char*, int)) {
			GT_IOCPWrapper::SetCallBackFunc(type, func);
		}
    }
}
