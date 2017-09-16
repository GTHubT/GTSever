
typedef void(*gtepoll_callback)(void* data, unsigned int datalen);

enum GTEPOLL_ERRORCODE{
	GTERROR_FAILED = -1,
	GTERROR_SUCCESS = 0
};

enum GTEPOLL_CALLBACK_TYPE{
	GTEPOLL_READ = 0,
	GTEPOLL_WRITE = 1
};
