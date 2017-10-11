#include <sys/socket.h>
#include <fcntl.h>

namespace GT{

	namespace EpollUtil{
		class GTEpoll_Util{
		public:
			static void setsocket2noblock(int sockfd);
		};		

	}
}
