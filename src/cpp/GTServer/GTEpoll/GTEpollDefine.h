#ifndef GT_EPOLL_DEFINITION_H_
#define GT_EPOLL_DEFINITION_H_


typedef void(*gtepoll_callback)(void* data, unsigned long datalen, void* reserve);

enum GTEPOLL_ERRORCODE{
	GTERROR_FAILED = -1,
	GTERROR_SUCCESS = 0
};

enum GTEPOLL_CALLBACK_TYPE{
	GTEPOLL_READ = 0,
    GTEPOLL_CONN = 1
};

#endif
