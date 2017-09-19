#include <socket.h>

namespace GT{

	namespace EpollUtil{
		class GTEpoll_Util{
		public:
			static bool setsocket2noblock(int sockfd);
		}		

	}
}
