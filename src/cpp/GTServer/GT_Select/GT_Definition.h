#ifndef  GT_DEFINITION_
#define  GT_DEFINITION_


enum EVENT_TYPE {		/* GT Select Event type */
	EVENT_ACCEPT = 0,
	EVENT_READ,
	EVENT_WRITE,
	EVENT_EXCEPTION
};


typedef void(*gt_event_callback)(EVENT_TYPE, ULONG_PTR* fd, char* data, size_t len);

#endif //  GT_DEFINITION_
