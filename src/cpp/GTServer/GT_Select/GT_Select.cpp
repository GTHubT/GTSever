// GT_Select.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "GT_Select.h"
#include "GT_Select_Wrapper.h"
#include "GTUtlity/GT_Util_GlogWrapper.h"


#ifndef GT_SELECT_WRAPPER
#define GT_SELECT_WRAPPER	GT::NET::GT_Select_Wrapper::GetInstance()
#endif


GT_SELECT_API bool Initialize(std::string cfg_path) {
	bool ret = GT_SELECT_WRAPPER.Initialize(cfg_path);
	if (ret) {
		GT_LOG_INFO("GT Select Service Init Success!");
	}
	else {
		printf("GT Select Service Init Failed!");
	}
	return ret;
}


GT_SELECT_API void StartGTService() {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Now Start GT Select Service!");
	GT_SELECT_WRAPPER.StartGTService();
}


GT_SELECT_API bool Finalize() {
	GT_TRACE_FUNCTION;
	GT_LOG_INFO("Now End GT Select Service!");
	GT_SELECT_WRAPPER.Finalize();
}


GT_SELECT_API void RegisterCallBack(EVENT_TYPE type, gt_event_callback cb) {	/* should invoke after Init */
	GT_TRACE_FUNCTION;
	switch (type)
	{
	case EVENT_READ:
		GT_LOG_INFO("Now Register Read Event CallBack!");
		GT_SELECT_WRAPPER.RegisterCallBack(cb, EVENT_READ);
		break;
	case EVENT_WRITE:
		GT_LOG_INFO("Now Register Write Event CallBack!");
		GT_SELECT_WRAPPER.RegisterCallBack(cb, EVENT_WRITE);
		break;
	default:
		break;
	}
}