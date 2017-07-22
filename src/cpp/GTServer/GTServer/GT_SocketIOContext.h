#ifndef GT_NET_IOCONTEXT_H_
#define GT_NET_IOCONTEXT_H_

namespace GT {

	namespace NET {

		enum IO_EVENT_TYPE {
			IO_EVENT_ACCEPT_COMPLETE = 0,
			IO_EVENT_READ_COMPLETE,
			IO_EVENT_READ_REQUEST,
			IO_EVENT_WRITE_COMPLETE,
			IO_EVENT_WRITE_REQUEST
		};

		class GT_SocketIOConetxt {


		};
	}
}

#endif // GT_NET_IOCONTEXT_H_
