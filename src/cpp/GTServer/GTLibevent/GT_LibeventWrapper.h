#ifndef GT_NET_LIBEVENT_H_
#define GT_NET_LIBEVENT_H_

#include "GTLibevent/libevent/libevent_header/event2/event.h"
#include "GTLibevent/libevent/libevent_header/event2/util.h"

#include "GTUtlity/GT_Util_CfgHelper.h"

#include <string>

namespace GT {
	namespace NET {

		class GT_LibeventWrapper
		{
		public:
			~GT_LibeventWrapper();
			static GT_LibeventWrapper& GetInstance();
			void	InitKernelIOImplentmentByConfig(std::string cfg_path); /* get the kernel type from the cfg. (iocp|select) */

		private:
			GT_LibeventWrapper();

		};
	}
}

#endif