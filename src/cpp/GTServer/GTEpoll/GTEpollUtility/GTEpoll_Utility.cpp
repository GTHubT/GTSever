#include "GTEpoll_Utility.h"
#include "GT_Util_CfgHelper.h"
#include "GT_Util_GlogWrapper.h"
#include <sys/socket.h>

using namespace GT::UTIL;
namespace GT{

	namespace EpollUtil{
	
		bool GTEpoll_Util::setsocket2noblock(int sockfd){
			int flags = fcntl(sockfd, F_GETFL);
            if (flags == -1){
                GT_LOG_ERROR("get socket state failed!");
                return false;
            }
			if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1){
                GT_LOG_ERROR("set socket noblock failed!");
                return false;
            }
            return true;
		}

        bool GTEpoll_Util::setsockop(int sockfd, int sockopname) {
            bool sockop = true;
            int ret = setsockopt(sockfd, SOL_SOCKET, sockopname, &sockop, sizeof(bool));
            if (ret !=0){
                GT_LOG_ERROR("set socket reuse address failed!");
                return false;
            }
            return true;
        }

	}
}
