#include <sys/socket.h>
#include <fcntl.h>

namespace GT{

	namespace EpollUtil{
		class GTEpoll_Util{
		public:
			static bool setsocket2noblock(int sockfd);
			static bool setsockop(int sockfd, int);
		};		

	}
}
