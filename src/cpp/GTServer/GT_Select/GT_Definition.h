#ifndef  GT_DEFINITION_
#define  GT_DEFINITION_

#ifndef WIN32_LEAN_AND_MEAN         // The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header. An example illustrating this is shown below.
#define WIN32_LEAN_AND_MEAN         // https://msdn.microsoft.com/en-us/library/windows/desktop/ms737629(v=vs.85).aspx
#endif

#include <windows.h>

/* GT Select Event type */
enum EVENT_TYPE {		
	EVENT_READ = 0,
	EVENT_WRITE,
	EVENT_EXCEPTION,
	EVENT_ACCEPT = EVENT_READ
};


/* this idea is from libevent */
struct fd_set_pri {
    unsigned int sock_count;
    SOCKET fd_sock_array[1];
};


typedef void(*gt_event_callback)(EVENT_TYPE, ULONG_PTR fd, char* data, size_t len);

#endif //  GT_DEFINITION_
