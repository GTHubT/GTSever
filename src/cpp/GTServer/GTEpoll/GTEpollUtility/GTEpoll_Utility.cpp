#include "GTEpoll_Utility.h"


namespace GT{

	namespace EpollUtil{
	
		void GTEpoll_Util::setsocket2noblock(int sockfd){
			int flags = fcntl(sockfd, F_GETFL);
			fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
			return;
		}

	}
}
