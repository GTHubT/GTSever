// GT_Server_ACCompany.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>


#ifndef GLOBAL_EVENT_NAME	
#define GLOBAL_EVENT_NAME	L"Global\\EVENT_GTService_Global_Exit_Event"
#endif

int main()
{
	/* create global event for exit service */
	HANDLE m_event = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, GLOBAL_EVENT_NAME);
	if (m_event == INVALID_HANDLE_VALUE) {
		printf("open global event failed!");
		return 0;
	}
	::SetEvent(m_event);

    return 0;
}

