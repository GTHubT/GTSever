#ifndef GT_IOBUFFER_H_
#define GT_IOBUFFER_H_

/************************************************************************/
/* IO DATA FOR EACH SOCKET EVENT                                        */
/************************************************************************/

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include <Windows.h>
#include <WinSock2.h>

namespace GT {

	namespace NET {

		enum IO_EVENT_TYPE {
			IO_EVENT_ACCEPT_COMPLETE = 0,
			IO_EVENT_READ_COMPLETE,
			IO_EVENT_READ_REQUEST,
			IO_EVENT_WRITE_COMPLETE,
			IO_EVENT_WRITE_REQUEST
		};

		class GT_IOContextBuffer: public OVERLAPPED {

		public:
			GT_IOContextBuffer(size_t len);
			~GT_IOContextBuffer();

			bool AllocateIOBuffer();
			void SetIOBufferSocket(SOCKET s);
			void ResetBuffer();

		private:
			SOCKET	io_socket_;
			WSABUF	io_wsa_buf_;
			char*	io_buffer_;
			size_t	io_buffer_size_;
		};
	}
}

#endif // GT_IOBUFFER_H_
