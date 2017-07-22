#ifndef GT_NET_IOCONTEXT_H_
#define GT_NET_IOCONTEXT_H_

#include "GT_IOContextBuffer.h"
#include "GT_SocketPool.h"

namespace GT {

	namespace NET {


		class GT_SocketConetxt {
		public:
			GT_SocketConetxt();
			~GT_SocketConetxt();

			void Reset();

		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
